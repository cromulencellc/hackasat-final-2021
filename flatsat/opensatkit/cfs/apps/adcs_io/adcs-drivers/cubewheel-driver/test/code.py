import board
from i2cperipheral import I2CPeripheral
from analogio import AnalogOut
from digitalio import DigitalInOut, Direction, Pull
import struct
import math
import time

regs = [0] * 16
index = 0

i2c_addr = 0x68


frame_id = 0
motor_control_mode = 0
backup_mode = 0
motor_switch_state = 0
hall_switch_state = 0
encoder_switch_state = 0
error_flag = 0
unused = 0
invalidTelemetryFlag = 0
invalidTelecommandFlag = 0
encoderError = 0
uartError = 0
i2cError = 0
canError = 0
configurationError = 0
speedError = 0

reference_speed = 0
wheel_current = 290 # mA
wheel_speed = math.floor(100/2) #rpm
wheel_duty = 5
wheel_speed_backup = wheel_speed

def send_tlm_identification():
    # print("Send TLM Identification")
    output = []
    output += bytearray([8, 0, 9, 8]) +  struct.pack("H", 1111) + struct.pack("H", 8888)
    return output

def send_tlm_identification_ext():
    # print("Send TLM Identification Ext")
    output = []
    output += struct.pack("H", 1234) + bytearray([68, 0xFF])
    return output
    
def send_tlm_status(motor_control_mode, backup_mode, motor_switch_state, hall_switch_state, encoder_switch_state, error_flag):
    # print("Send TLM Status MCM:{0:d}, BM:{1:d}, MSS:{2:d} HSS:{3:d}, ESS:{4:d}, Error Flag: {5:d}".format(motor_control_mode, backup_mode, motor_switch_state, hall_switch_state, encoder_switch_state, error_flag))
    status = 0
    status |= (backup_mode & 0x1) << 7
    status |= (motor_switch_state & 0x1) << 6
    status |= (hall_switch_state & 0x1) << 5
    status |= (encoder_switch_state & 0x1) << 4
    status |= (error_flag & 0x1) << 3
    status |= unused
    # print("Status byte: {0:d}:{1:08b}".format(status,status))
    output = []
    output = struct.pack("H", 1111) + struct.pack("H", 8888) + bytearray([0, 0, motor_control_mode, status])
    return output

def send_tlm_wheel_data_full(wheel_speed, wheel_reference_speed, wheel_current):
    # print("Send TLM Wheel Data Full")
    output = []
    output += struct.pack("h", wheel_speed) + struct.pack("h", wheel_reference_speed) + struct.pack("h", wheel_current)
    return output

def send_tlm_wheel_data_additional(wheel_duty, wheel_speed_backup):
    # print("Send TLM Wheel Data Additional")
    output = []
    output += struct.pack("h", wheel_duty) + struct.pack("h", wheel_duty)
    return output

def send_tlm_wheel_status_flags(invalidTelemetryFlag=0, invalidTelecommandFlag=0, encoderError=0, uartError=0, i2cError=0, canError=0, configurationError=0, speedError=0):
    status = 0
    status |= (invalidTelemetryFlag & 0x01)
    status |= (invalidTelecommandFlag & 0x01) << 1
    status |= (encoderError & 0x01) << 2
    status |= (uartError & 0x01) << 3
    status |= (i2cError & 0x01) << 4
    status |= (canError & 0x01) << 5
    status |= (configurationError & 0x01) << 6
    status |= (speedError & 0x01) << 7
    return bytearray([status])


def voltage_to_dac(voltage):
    return math.floor((voltage*1024)/3.3 * 64)

vout = 0.95
dac_value = voltage_to_dac(vout)
print("Set analog output for testing: {0:f} ({1:d}) V".format(vout, dac_value))
analog_out = AnalogOut(board.A0)
analog_out.value = dac_value

enable_pin = DigitalInOut(board.D8)
enable_pin.direction = Direction.INPUT
# enable_pin.pull = Pull.DOWN

print("Waiting for wheel enable")
while enable_pin.value == False:
    time.sleep(0.1)


print("Starting I2C response")
with I2CPeripheral(board.SCL, board.SDA, (i2c_addr,)) as device:
    while True:
        r = device.request()
        if not r:
            # Maybe do some housekeeping
            continue
        with r:  # Closes the transfer if necessary by sending a NACK or feeding dummy bytes
            # print("Process request")
            # print("I2C Addr: 0x{0:02X}, Is Read {1:d}, Is Restart {2:d}".format(r.address, r.is_read, r.is_restart))
            if r.address == i2c_addr:
                if not r.is_read:  # Main write which is Selected read
                    # print("Get Frame Id Byte")
                    b = r.read(1)
                    if b:
                        frame_id = struct.unpack("B", b)[0]
                        print("Recieved frame ID: " + str(frame_id))
                        if frame_id < 40:
                            # print("Telecommand Recieved")
                            if frame_id == 1:
                                reset_id = struct.unpack("B", r.read(1))[0]
                                # print("Reset telecommand recieved: {0:d}".format(reset_id))
                            elif frame_id == 2:
                                reference_speed = struct.unpack("h", r.read(2))[0]
                                reference_speed_rpm = float(reference_speed/2.0)
                                wheel_speed = reference_speed + 5
                                # print("Reference speed telecommand recieved. Speed: {0:d}:{1:f}".format(reference_speed, reference_speed_rpm))
                            elif frame_id == 3:
                                wheel_duty = struct.unpack("h", r.read(2))[0]
                                # print("Duty cycle command recieved. Duty Cycle: {0:d}".format(wheel_duty))
                            elif frame_id == 7:
                                motor_switch_state = r.read(1)
                                # print("Recieved motor power state command. State: {}".format(motor_switch_state))
                            elif frame_id == 8:
                                encoder_switch_state = r.read(1)
                                # print("Recieved encoder power state command. State: {}".format(encoder_switch_state))
                            elif frame_id == 8:
                                hall_switch_state = r.read(1)
                                # print("Recieved hall power state command. State: {}".format(encoder_switch_state))
                            elif frame_id == 10:
                                motor_control_mode = struct.unpack("B", r.read(1))[0]
                                # print("Control mode telecommand recieved. Mode: {0:d}".format(motor_control_mode))
                            elif frame_id == 12:
                                backup_mode = r.read(1)
                                # print("Recieved back-up mode state command. State: {}".format(backup_mode))
                            elif frame_id == 20:
                                clear_errors = r.read(1)
                                if clear_errors == 85:
                                    invalidTelemetryFlag = 0
                                    invalidTelecommandFlag = 0
                                    encoderError = 0
                                    uartError = 0
                                    i2cError = 0
                                    canError = 0
                                    configurationError = 0
                                    speedError = 0
                            elif frame_id == 31:
                                new_i2c_addr = r.read(1)
                                # print("Recieved set I2C addr command. I2C: {}".format(new_i2c_addr))
                            elif frame_id == 33:
                                new_can_mask = r.read(1)
                                # print("Recieved set CAN mask command. CAN Mask: {}".format(new_can_mask))
                            elif frame_id == 33:
                                b = r.read(3)
                                # print("Recieved PWM Gain Command: {0:s}".format(str(b)))
                            elif frame_id == 34:
                                b = r.read(6)
                                # print("Recieved Main Speed Controller Gain Command: {0:s}".format(str(b)))
                            elif frame_id == 35:
                                b = r.read(6)
                                # print("Recieved Backup Speed Controller Gain Command: {0:s}".format(str(b)))
                            else:
                                invalidTelecommandFlag = 1
                    else:
                        # print("No data to read")
                        continue
                elif r.is_restart:  # Combined transfer: This is the Main read message
                    # print("Recieved Telemetry Request")
                    n = 0
                    if frame_id == 128:
                        n = r.write(bytes(send_tlm_identification()))
                    elif frame_id == 129:
                        n = r.write(bytes(send_tlm_identification_ext()))
                    elif frame_id == 130:
                        n = r.write(bytes(send_tlm_status(motor_control_mode, backup_mode, motor_switch_state, hall_switch_state, encoder_switch_state, error_flag)))
                    elif frame_id == 133:
                        n = r.write(bytes(2))
                    elif frame_id == 134:
                        n = r.write(bytes(2))
                    elif frame_id == 135:
                        n = r.write(bytes(2))
                    elif frame_id == 137:
                        n = r.write(bytes(send_tlm_wheel_data_full(wheel_speed, reference_speed, wheel_current)))
                    elif frame_id == 138:
                        n = r.write(bytes(send_tlm_wheel_data_additional(wheel_duty, wheel_speed_backup)))
                    elif frame_id == 139:
                        n = r.write(bytearray([9,8,7]))
                    elif frame_id == 140:
                        n = r.write(bytearray([1,2,3,4,5,6]))
                    elif frame_id == 141:
                        n = r.write(bytearray([10, 11, 12, 13, 14, 15]))
                    elif frame_id == 145:
                        n = r.write(bytes(send_tlm_wheel_status_flags(invalidTelemetryFlag, invalidTelecommandFlag, encoderError, uartError, i2cError, canError, configurationError, speedError)))
                    else:
                        invalidTelemetryFlag = 1
                    # print("Wrote " + str(n) + " bytes to master")
                    