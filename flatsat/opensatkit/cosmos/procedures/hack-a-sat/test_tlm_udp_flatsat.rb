
puts "Start Telemetry Outpuf from CDH #{ENV["CDH_IP"]}"
cmd("KIT_TO ENABLE_TELEMETRY with CCSDS_STREAMID 6272, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 17, CCSDS_FUNCCODE 5, CCSDS_CHECKSUM 0, IP_ADDR #{ENV['HOST_IP']}")
