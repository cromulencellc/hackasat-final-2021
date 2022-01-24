
puts "Dump Current EPS Configuration Table"

cmd("EPS_MGR DUMP_TBL with CCSDS_STREAMID 6416, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 67, CCSDS_FUNCCODE 3, CCSDS_CHECKSUM 0, ID 0, TYPE 0, FILENAME '/cf/eps_cfg_tbl_d.json'")
wait_time=15
wait(wait_time)

puts "Playback Dumped EPS Config File to Ground"

# if(File.file?("/cosmos/downloads/eps_cfg_tbl_d.json"))
#   puts "Delete old downlinked table file"
#   File.delete("/cosmos/downloads/eps_cfg_tbl_d.json")
# end
filetime = Time.now.to_i
filedl = "/cosmos/downloads/eps_cfg_tbl_d_#{filetime}.json"
cmd("CF2 PLAYBACK_FILE with CCSDS_STREAMID 6339, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 149, CCSDS_FUNCCODE 2, CCSDS_CHECKSUM 0, CLASS 2, CHANNEL 0, PRIORITY 0, PRESERVE 0, PEER_ID '0.21', SRC_FILENAME '/cf/eps_cfg_tbl_d.json', DEST_FILENAME '#{filedl}'")
puts "Wait  #{wait_time} seconds for file playback to finish"
wait(wait_time)

puts "Teams analyze dumped table, fix then prep new table for upload"

puts "Upload Corrected File to Spacecraft"
cmd("CFDP SEND_FILE with CLASS 2, DEST_ID '24', SRCFILENAME '/cosmos/procedures/hack-a-sat/eps_mgr_cfg_tbl_working.json', DSTFILENAME '/cf/eps_cfg_up.json', CPU 2")
puts "Wait #{wait_time} seconds for file upload to finish"
wait(wait_time)

puts "Load new EPS Configuration Table"
cmd("EPS_MGR LOAD_TBL with CCSDS_STREAMID 6416, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 67, CCSDS_FUNCCODE 2, CCSDS_CHECKSUM 0, ID 0, TYPE 0, FILENAME '/cf/eps_cfg_up.json'")

puts "Wait  #{wait_time} for table load to complete"
wait(wait_time)

puts "Mode spacecraft into nominal mode and let the packets flow"
cmd("EPS_MGR SET_MODE with CCSDS_STREAMID 6416, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 2, CCSDS_FUNCCODE 4, CCSDS_CHECKSUM 0, MODE NOMINAL_OPS_PAYLOAD_ON")
wait_check("EPS_MGR FSW_TLM_PKT COMM_PAYLOAD_SWITCH == 'ON'", 3)
wait(1)
wait_check("EPS_MGR FSW_TLM_PKT COMM_PAYLOAD_SWITCH == 'ON'", 1)
