require 'cosmos/script/telemetry'
require 'cosmos/config/config_parser'
require 'cosmos/interfaces/protocols/protocol'


module Cosmos
 
    class  IgnoreTargetProtocol < Protocol
        
        # @param target_name [String] Target name

        def initialize(target_name, allow_empty_data = false)

            super(allow_empty_data)
            @target_name = target_name

        end

        def write_packet(packet)
            # Need to make sure packet is identified and defined
            target_names = nil
            target_names = @interface.target_names if @interface
            identified_packet = System.telemetry.identify_and_define_packet(packet, target_names)
            if identified_packet
              if identified_packet.target_name == @target_name
                # puts "Successfully ignored pkt for " + @target_name
                return :STOP
              end
            end
            return super(packet)
          end
      
          def read_packet(packet)
            return :STOP if packet.target_name == @target_name
            return super(packet)
          end
    end
    
end