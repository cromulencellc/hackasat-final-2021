
import struct, csv, pprint


def calculate_mode_mask(index, ttc_comm, adcs, rw, imu, st, mtr, css, fss, cp):
    mode_value = 0
    mode_value |= (ttc_comm & 0x1) << 0
    mode_value |= (adcs & 0x1) << 1
    mode_value |= (rw & 0x1) << 2
    mode_value |= (imu & 0x1) << 3
    mode_value |= (st & 0x1) << 4
    mode_value |= (mtr & 0x1) << 5
    mode_value |= (css & 0x1) << 6
    mode_value |= (fss & 0x1) << 7
    mode_value |= (cp & 0x1) << 8
    mode_mask = struct.pack("<h", mode_value)
    print(f"Mode Mask Index {index}: Decimal: {mode_value}, Hex: 0x{mode_mask.hex()}")
    return mode_mask.hex()


def read_mode_csv(mode_table_file):
    with open(mode_table_file, newline='') as csvfile:
        data = csv.DictReader(csvfile)
        for row in data:
            # pprint.pprint(row)
            calculate_mode_mask(index=int(row["MODE"]), ttc_comm=int(row["TTC"]), adcs=int(row["ADCS"]), rw=int(row["RW"]), 
                                imu=int(row["IMU"]), st=int(row["ST"]), mtr=int(row["MTR"]), 
                                css=int(row["CSS"]), fss=int(row["FSS"]), cp=int(row["CP"]))
    # pprint.pprint(data)

# calculate_mode_mask(ttc_comm=1, adcs=1, rw=1, imu=1, st=0, mtr=0, css=1, fss=0, cp=0)

print("Modes for Correct Operation")
read_mode_csv("modes.csv")

print("\n")
print("Modes for Broken Startup")
read_mode_csv("modes_broken.csv")

print("\n")
print("Modes for FSS + Payload")
read_mode_csv("modes_fss_payload.csv")