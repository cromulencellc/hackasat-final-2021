puts "testing send file to cpu1"
cmd("CFDP SEND_FILE with CLASS 1, DEST_ID '24', SRCFILENAME '/cosmos/procedures/hack-a-sat/test.txt', DSTFILENAME '/cf/test.text', CPU 1")


puts "wait 80 seconds for SEND_FILE to complete"
wait(80)

puts "testing playback file from cpu1"
cmd("CF PLAYBACK_FILE with CCSDS_STREAMID 6323, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 149, CCSDS_FUNCCODE 2, CCSDS_CHECKSUM 0, CLASS 1, CHANNEL 0, PRIORITY 0, PRESERVE 1, PEER_ID '0.21', SRC_FILENAME '/cf/test.text', DEST_FILENAME '/cosmos/downloads/test_cpu1.txt'")

puts "wait 120 seconds for PLAYBACK_FILE to complete"
wait(120)


puts "testing send file to cpu2"
cmd("CFDP SEND_FILE with CLASS 1, DEST_ID '24', SRCFILENAME '/cosmos/procedures/hack-a-sat/test.txt', DSTFILENAME '/cf/test.text', CPU 2")

puts "testing playback file from cpu2 29"
cmd("CF2 PLAYBACK_FILE with CCSDS_STREAMID 6339, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 149, CCSDS_FUNCCODE 2, CCSDS_CHECKSUM 0, CLASS 2, CHANNEL 0, PRIORITY 0, PRESERVE 1, PEER_ID '0.21', SRC_FILENAME '/cf/test.text', DEST_FILENAME '/cosmos/downloads/test_cpu2.txt'")
