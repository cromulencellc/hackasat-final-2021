puts "Challenge 1 Recover Satellite Solver"
puts "Mode to SAFE since SEPERATION doesn't have wheels on"

display("EPS_MGR EPS_MGR_FSW_TLM")

cmd("EPS_MGR SET_MODE with CCSDS_STREAMID 6416, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 2, CCSDS_FUNCCODE 4, CCSDS_CHECKSUM 0, MODE SAFE")

wait_check("EPS_MGR FSW_TLM_PKT WHEEL_SWITCH == 'ON'", 5)

puts "Let spacecraft attitude recover and settle for 60 seconds"
wait(60)

