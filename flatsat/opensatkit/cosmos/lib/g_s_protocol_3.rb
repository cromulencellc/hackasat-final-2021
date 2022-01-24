require 'cosmos/script/telemetry'
require 'cosmos/config/config_parser'
require 'cosmos/interfaces/protocols/protocol'

module Cosmos
 
  class GSProtocol3 < Protocol
    FRAMESYNC = "\xDE\xAD\xBE\xEF"
    FRAMESYNC_SIZE = FRAMESYNC.size
    FRAMESYNC.force_encoding('ASCII-8BIT')

    def initialize(auth_mode = false, allow_empty_data = nil)
      @auth_mode = ConfigParser.handle_true_false(auth_mode)
      @cmd_counter = [0] 
      super(allow_empty_data)
    end
 
    def read_data(data)
      # Remove the header
      # header = FRAMESYNC
      if data.length > FRAMESYNC_SIZE
        if ((data[0..3] == FRAMESYNC))
          return super(data[4..-1])
        else
          # Header not found discard the packet this should never happen
          puts "Header not found. Data: #{data}"
          return :STOP
          # return super(data)
        end
      else
          return :STOP
      end
    end #end read_data
	   
  end # class
 
end # module
