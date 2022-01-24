
require 'mqtt'

puts "Welcome to FlatSat Integration Init Tool"

puts "Send command to start GAME"
mqtt_host = ENV['MQTT_IP'] || "127.0.0.1"
mqtt_port = ENV['MQTT_PORT'] || "1883"
mqtt_user = ENV['MQTT_USER'] || "cosmos"
mqtt_pass =ENV['MQTT_PASS'] || "cosmos"

ask_string("Start Round #1?", "yes")
MQTT::Client.connect(:host => mqtt_host, :port => mqtt_port.to_i, :username => mqtt_user, :password => mqtt_pass ) do |c|
    c.publish("GAME/CONTROL", "START_ROUND")
end

puts("Wait 5 seconds and enable tlm")
wait(5)

puts "Start Telemetry Outpuf from CDH #{ENV["FLATSAT_TO_IP"]}"
while get_tlm_cnt("KIT_TO", "HK_TLM_PKT") == 0
  cmd("KIT_TO ENABLE_TELEMETRY with CCSDS_STREAMID 6272, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 17, CCSDS_FUNCCODE 5, CCSDS_CHECKSUM 0, IP_ADDR #{ENV['HOST_IP']}")
  wait(5)
end

ask_string("Solve challenge 1: Recover Satellite", "yes")
puts "Solve challenge 1"
start("hack-a-sat/challenge_1_solver.rb")

ask_string("Solve challenge 2: EPS Modes and Payload to On", "yes")
puts "Solve challenge 2"
start("hack-a-sat/challenge_2_solver.rb")

