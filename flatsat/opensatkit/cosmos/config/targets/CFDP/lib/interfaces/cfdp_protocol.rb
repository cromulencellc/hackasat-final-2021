require "cosmos/interfaces/protocols/protocol"
require "cfdp_engine"
require 'cfdp_vars'

module Cosmos
  # Protocol which permanently overrides an item value such that reading the
  # item returns the overriden value. Methods are prefixed with underscores
  # so the API can include the original name which calls out to these
  # methods. Clearing the override requires calling normalize_tlm.
  class CfdpProtocol < Protocol

    # @param allow_empty_data [true/false] Whether STOP should be returned on empty data
    def initialize(allow_empty_data = false)

      super(allow_empty_data)
      @pdu_queue = Queue.new  #dcm
    end

    def getCFDPTask

      Cosmos::CmdTlmServer.background_tasks.all.each {|task| return task if task.is_a?(Cosmos::CfdpEngineTask)}
      return nil
    end

    def read_packet(packet)

      # update packet counter here.
      identified_packet = System.telemetry.identify!(packet.buffer, [PDU_TARGET_NAME])

      unless identified_packet.nil?

        packetPayload = identified_packet.read('PAYLOAD', :RAW)
        pdu = CFDP::PDUPacket.new(packetPayload.bytes,1)
        @pdu_queue << packetPayload.bytes   #dcm
        cfdpTask = getCFDPTask

        unless cfdpTask.break

          cfdpTask.update_received_counter(pdu.pduPayload.class) unless cfdpTask.nil?
          Thread.new do
            #dcm CFDP::CFDPEngine.instance.handlePDUReceived(packetPayload.bytes)
            CFDP::CFDPEngine.instance.handlePDUReceived(@pdu_queue,1)  #dcm
          end
        end
      end

      if identified_packet.nil?
        identified_packet = System.telemetry.identify!(packet.buffer, [PDU2_TARGET_NAME])

      unless identified_packet.nil?

        packetPayload = identified_packet.read('PAYLOAD', :RAW)
          pdu = CFDP::PDUPacket.new(packetPayload.bytes,2)
        @pdu_queue << packetPayload.bytes   #dcm
        cfdpTask = getCFDPTask

        unless cfdpTask.break

          cfdpTask.update_received_counter(pdu.pduPayload.class) unless cfdpTask.nil?
          Thread.new do
            #dcm CFDP::CFDPEngine.instance.handlePDUReceived(packetPayload.bytes)
              CFDP::CFDPEngine.instance.handlePDUReceived(@pdu_queue,2)  #dcm
            end
          end
        end
      end

      return packet
    end

    def write_packet(packet)

      # update packet counter here.
      identified_packet = System.commands.identify(packet.buffer, [PDU_TARGET_NAME])

      unless identified_packet.nil?

        if identified_packet.packet_name.eql?(PDU_SEND_TARGET_PACKET)
          pdu = CFDP::PDUPacket.new(identified_packet.read('PAYLOAD', :RAW).bytes)
          cfdpTask = getCFDPTask
          cfdpTask.update_sent_counter(pdu.pduPayload.class) unless cfdpTask.nil?
        end
      end

       # update packet counter here.
      if identified_packet.nil?
        identified_packet = System.commands.identify(packet.buffer, [PDU2_TARGET_NAME])

        unless identified_packet.nil?
  
          if identified_packet.packet_name.eql?(PDU2_SEND_TARGET_PACKET) 
            pdu = CFDP::PDUPacket.new(identified_packet.read('PAYLOAD', :RAW).bytes, 2)
            cfdpTask = getCFDPTask
            cfdpTask.update_sent_counter(pdu.pduPayload.class) unless cfdpTask.nil?
          end
        end  
      end
      return packet
    end
  end
end
