
require "cosmos/tools/cmd_tlm_server/background_task"
require "cosmos/tools/cmd_tlm_server/api"
require 'mqtt'
require 'securerandom'

$mqtt_host = ENV['MQTT_IP'] || "127.0.0.1"
$mqtt_port = ENV['MQTT_PORT'] || "1883"
$mqtt_user = ENV['MQTT_USER'] || "cosmos"
$mqtt_pass =ENV['MQTT_PASS'] || "cosmos"
 
module Cosmos
 
  class MqttJob < BackgroundTask

    def call
        @name = "MqttJob"

        run_id = SecureRandom.uuid
        Logger.instance.info("MqttJob run #{run_id}")

        client = nil
        begin
            Logger.instance.info("Connect to MQTT #{$mqtt_host}:#{$mqtt_port} with #{$mqtt_user}/#{$mqtt_pass}")
            client = MQTT::Client.connect(:host => $mqtt_host, :port => $mqtt_port.to_i, :username => $mqtt_user, :password => $mqtt_pass )
            client.subscribe( 'GAME/#' )
          rescue => e
            Logger.instance.error("MqttJob couldn't connect to mosquitto, quitting")
            Logger.instance.error("#{e.class}: #{e.message} #{e.backtrace.join("\n")}")
            return
        end
        loop do
          begin
              Logger.instance.info("MqttJob connected to mosquitto instance")

              # read and process packets of telemetry data

              client.get do |topic,message|
                if topic == "GAME/STATUS"
                  begin
                    if message == "STOPPED"
                      Logger.instance.info("Recieved command to pause or end round, disconnect CHALLENGER_COMM_ROUTER, REPORT_API_ROUTER")
                      disconnect_router("CHALLENGER_COMM_ROUTER")
                      disconnect_router("REPORT_API_ROUTER")
                    end
                    if message == "RUNNING"
                      Logger.instance.info("Recieved command to resume or start round, connect all interfaces")
                      connect_interface("FLATSAT_CFS_INT")
                      connect_router("CHALLENGER_COMM_ROUTER")
                      connect_router("REPORT_API_ROUTER")
                    end
                  rescue => e
                    Logger.instance.error("Failed while exception class #{e.class}")
                    next
                  end
                end
              end

          rescue Errno::ECONNREFUSED
              # connection to router was lost, retry
              Logger.instance.error("MQTT mosquitto connection to router was lost, giving up")
              sleep(5)
              client.connect()
              return
          rescue MQTT::Exception
              # connection to database was lost, retry
              Logger.instance.error("MQTT Error")
              client.disconnect()
              sleep(5)
              client.connect()
              next
          rescue => e
              # some other error occurred, retry
              Logger.instance.error("Failed while exception class #{e.class}")
              File.open("/tmp/fail", "a") do |f|
                  f.puts e.class
                  f.puts e.message
                  f.puts e.backtrace
              end
              sleep(5)
              client.connect()
              next
          ensure
              client.disconnect()
          end
      end
    end


end
end