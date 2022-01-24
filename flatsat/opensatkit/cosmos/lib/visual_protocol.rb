require "cosmos/interfaces/protocols/protocol"
require 'cosmos/interfaces/tcpip_server_interface'

module Cosmos
  # Protocol which 
  class VisualProtocol < Protocol

    CMD_HEADER = "SC[0].AC."
    ORB_CMD_HEADER = "Orb[0]."

    TIME_INIT  = "2021-131-00:00:00"

    # @param allow_empty_data [true/false] Whether STOP should be returned on empty data
    def initialize(allow_empty_data = false)
      @@ephem_cmd_string = ""
      @@qbn_cmd_string = ""
      @@ephem_cmds = {
          "DynTime" => ["ABSOLUTE_TIME"],
          "PosN" => ["POSN_X", "POSN_Y", "POSN_Z"],
          "VelN" => ["VELN_X", "VELN_Y", "VELN_Z"]
      }
      @@fsw_cmds = {
          "qbn" => ["QBN_X", "QBN_Y", "QBN_Z", "QBN_S"]
      }

      @@cmds = {"EPHEM" => @@ephem_cmds, "HK_FSW_TLM_PKT" => @@fsw_cmds}
      @tlm = nil
      puts TIME_INIT
      @@initial_time = DateTime.strptime(TIME_INIT, "%Y-%j-%H:%M:%S")
      self.tlm_connect
      super(allow_empty_data)
    end

    def tlm_connect
        if @tlm == nil or not @tlm.read_allowed?
            @tlm = Cosmos::TcpipServerInterface.new(2056, 2056, 10.0, nil)
            @tlm.connect
        end
    end

    def read_packet(packet)

      ephem_pkt = System.telemetry.identify!(packet.buffer, ["EPHEM"])

      unless ephem_pkt.nil? || ephem_pkt.packet_name != "EPHEM_PKT"
        @ephem_cmd_string = "" 
        @@ephem_cmds.each do |cmd, tlm_items|
          cmd_value = CMD_HEADER + cmd + " = "
          tlm_items.each do |tlm_item|
            if tlm_item == "ABSOLUTE_TIME"
              cmd_value += ephem_pkt.read(tlm_item, :RAW).to_s + " "
              dt_time = ephem_pkt.read(tlm_item, :RAW) - ephem_pkt.read("ABSOLUTE_TIME_EPOCH", :RAW)
              cmd_value += "\n"
              cmd_value += "TIME " + (@@initial_time + Rational(dt_time, 86400)).strftime("%Y-%j-%H:%M:%S.%9N")
              next
            else
              cmd_value += (ephem_pkt.read(tlm_item, :RAW) * 1000).to_s + " "
            end
          end
          @ephem_cmd_string += cmd_value + "\n"
        end
        # puts @ephem_cmd_string
      end
      
      fsw_pkt = System.telemetry.identify!(packet.buffer, ["ADCS"])

      unless fsw_pkt.nil? || fsw_pkt.packet_name != "HK_FSW_TLM_PKT"
        @fsw_cmd_string = ""
        @@fsw_cmds.each do |cmd, tlm_items|
          cmd_value = CMD_HEADER + cmd + " = "
          tlm_items.each do |tlm_item|
              cmd_value += fsw_pkt.read(tlm_item, :RAW).to_s + " "
          end
          @fsw_cmd_string += cmd_value + "\n"
        end
        if !@fsw_cmd_string.nil? && !@ephem_cmd_string.nil? && @ephem_cmd_string != "" && @fsw_cmd_string != ""
            @fsw_cmd_string = " " * 5 + @ephem_cmd_string + @fsw_cmd_string + "[EOF]\n"
            @tlm.write_raw(@fsw_cmd_string)
            @ephem_cmd_string = ""
            @fsw_cmd_string = ""
        end
      end

      return packet
    end

  end
end
