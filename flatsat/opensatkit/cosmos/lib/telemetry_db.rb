require "cosmos"
require "cosmos/interfaces/tcpip_client_interface"
require "cosmos/tools/cmd_tlm_server/background_task"
require "socket"
require "pg"
require 'securerandom'

# this background task connects to a pre-defined router within cosmos that has forwarded telemetry messages
# it also connects to a pre-defined database where those telemetry messages should be stored
# a failed router connection is fatal (because cosmos is likely screwed up at this point)
# a failed database connection triggers a retry (because the network could be wonky, but otherwise okay)

$telemetry_db_url = 
    ENV['TELEMETRY_DB_URL'] ||
    "postgres://team1:team1@localhost/telemetry_testing"

module Cosmos
    class TelemetryDb < BackgroundTask
        def call
            @name = "TelemetryDb"

            run_id = SecureRandom.uuid
            Logger.instance.info("TelemetryDb run #{run_id}")

            db = nil
            begin
                db = PG.connect($telemetry_db_url)
            rescue => e
                Logger.instance.error("TelemetryDb couldn't connect to pg, quitting")
                Logger.instance.error("#{e.class}: #{e.message} #{e.backtrace.join("\n")}")
                return
            end
            loop do
                last_tlm_hash = nil
                begin
                    Logger.instance.info("TelemetryDb connected to telemetry database")

                    Logger.instance.info("TelemetryDb sleeping before attempting tlm connection")
                    sleep(5)
                    self.tlm_connect
                    Logger.instance.info("TelemetryDb connected to telemetry router")


                    # read and process packets of telemetry data
                    loop do
                        pkt = @tlm.read

                        received_at = Time.now

                        target_name = pkt.target_name
                        packet_name = pkt.packet_name

                        identified = System.telemetry.identify_and_define_packet(pkt, packet_name)
                        contents = Hash[identified.read_all]

                        last_tlm_hash = JSON.dump(contents)

                        full_name = "#{target_name}/#{packet_name}"

                        db.exec("SELECT parse_as_current_user($1, $2, $3, $4);",
                            [full_name, JSON.dump(contents), 
                            run_id, received_at]) do |result|
                            Logger.instance.info("Got #{result[0]['parse_as_current_user']} for #{full_name}")
                        end
                    end
                rescue Errno::ECONNREFUSED
                    # connection to router was lost, retry
                    Logger.instance.error("TelemetryDb connection to router was lost, giving up")
                    return
                rescue PG::UnableToSend
                    # connection to database was lost, retry
                    Logger.instance.error("TelemetryDb connection to database was lost, retrying...")
                    db.reset
                    sleep(5)
                    next
                rescue => e
                    # some other error occurred, retry
                    Logger.instance.error("Failed while reading telemetry with exception class #{e.class}")
                    File.open("/tmp/fail", "a") do |f|

                        f.puts last_tlm_hash
                        f.puts e.class
                        f.puts e.message
                        f.puts e.backtrace
                    end
                    sleep(5)
                    next
                ensure
                    self.tlm_disconnect
                end
            end
        end

        def read_int(pkt, name)
            # read the field from the packet
            val = pkt.read(name)

            # handle all the weird state values cosmos can return from reading an integer
            # (you should be able to do "pkt.read(name, :RAW) i think? but, it doesn't seem to work...)
            if val == "ON"
                return 1
            elsif val == "OFF"
                return 0
            elsif val == "TRUE"
                return 1
            elsif val == "FALSE"
                return 0
            elsif val == "POS"
                return 1
            elsif val == "NEG"
                return -1
            else
                return val
            end
        end

        def read_float(pkt, name)
            # read the field from the packet
            val = pkt.read(name)

            # handle NaNs before they reach the database
            if not val.nan?
                return val
            else
                return 0.0
            end
        end

        def read_string(pkt, name)
            # read the field from the packet
            val = pkt.read(name)

            # convert to ASCII because we often get bad unicode and I dunno what else to do with it
            return val.encode("ASCII", "UTF-8", invalid: :replace, undef: :replace)
        end

        def tlm_connect
            if @tlm == nil or not @tlm.read_allowed?
                @tlm = Cosmos::TcpipClientInterface.new("localhost", 7779, 7779, 10.0, nil, "PREIDENTIFIED")
                @tlm.connect
            end
        end

        def tlm_disconnect
            return if @tlm.nil?
            @tlm.disconnect
            @tlm = nil
        end

    end
end

# uncomment the following two lines to run this manually (e.g. while using a replay)
#lol = Cosmos::TelemetryDb.new
#lol.call

