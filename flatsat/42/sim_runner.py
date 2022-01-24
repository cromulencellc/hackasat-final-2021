import subprocess
import pathlib
from shutil import copyfile
import math
import numpy as np
import matplotlib.pyplot as plt
import time

cmd_42 = ["./42", "HAS2"]
cmd_fs = ["./FlatSatFswApp"]

def run_iteration(wc, vmax, amax, total_time, impulse_time, impulse_mag, impulse_axis, output_dir):
    procs = []
    cmd_42_str = " ".join(cmd_42)
    print(cmd_42_str)
    procs.append(subprocess.Popen(cmd_42_str, shell=True))
    time.sleep(0.2)
    cmd_fs_with_args = cmd_fs + [f"{wc}", f"{vmax}", f"{amax}", f"{total_time}", f"{impulse_time}", f"{impulse_mag}", f"{impulse_axis}"]
    cmd_fs_str = " ".join(cmd_fs_with_args)
    print(cmd_fs_str)
    procs.append(subprocess.Popen(cmd_fs_str, shell=True))
    procs.reverse()
    for p in procs:
        p.wait()
        print("processed finished {}".format(p.returncode))

    print("Get output file")
    outputFile = pathlib.Path("FswResults.txt")
    outputFileRenamed = outputFile.parent / output_dir / (outputFile.stem + f"_{wc}_{vmax}_{amax}_{total_time}_{impulse_time}_{impulse_mag}_{impulse_axis}.txt")
    copyfile(outputFile, outputFileRenamed )
    return outputFileRenamed

def plot_axis(ax, time, values, ylabel, labels):
    for i in range(values.shape[1]):
        ax.plot(time, values[:,i], label = labels[i])
    ax.grid()   
    ax.set_xlabel("Time (s)")
    ax.set_ylabel(ylabel)
    ax.legend(loc="upper left")

def plot_results(results_file):
    data = np.genfromtxt(results_file.as_posix(), delimiter=',', dtype=float)
    fig = plt.figure(figsize=(20,15))
    ax1 = fig.add_subplot(321)
    ax2 = fig.add_subplot(322)
    ax3 = fig.add_subplot(323)
    ax4 = fig.add_subplot(324)
    ax5 = fig.add_subplot(325)
    ax6 = fig.add_subplot(326)
    # ax = fig.add_subplot(121)
    # pp.pprint(data)
    # print(str(data))

    time = data[:,0] - data[0, 0]
    # pp.pprint(time)
    theta_error = data[:,1:4]
    w_err = data[:, 4:7]
    theta_actual = data[:, 7:10]
    w_actual = data[:, 10:13]
    w_speed = data[:, 13:16]
    t_cmd = data[:, 16:] * 1000
    labels=["x", "y", "z"]
    plot_axis(ax1, time, theta_error, "Theta Error (deg)", labels)
    plot_axis(ax2, time, w_err, "Rate Error (deg/s)", labels)
    plot_axis(ax3, time, theta_actual, "Theta (deg)", labels)
    plot_axis(ax4, time, w_actual, "Body Rates (deg/s)", labels)
    plot_axis(ax5, time, w_speed, "Wheel Spped (rpm)", labels)
    plot_axis(ax6, time, t_cmd, "Torque (m*Nm)", labels)
    plt.ion()
    plt.show(block=True)
    fig.savefig((results_file.parent / (results_file.stem + ".png")).as_posix())
    print("Finished with plot")


if __name__ == "__main__":
    output_dir = "sim_results"
    output_path = pathlib.Path.cwd() / output_dir
    output_path.mkdir(exist_ok=True)


    amax = 0.5
    vmax = 5
    wc_list = [0.1 * 2 * np.pi]
    wc = 0.28
    # wc_list = [0.28]
    # wc_list = np.arange(0.01, 0.5, 0.01)

    impulse_axis = 1
    impulse_size = 45
    # impulse_size_list = np.arange(5, 180, 10)
    impulse_size_list = [45]
    impulse_time = 10
    total_time = 100


    output_files = []
    # for wc in wc_list:
    #     output_files.append(run_iteration(wc, vmax, amax, total_time, impulse_time, impulse_size, impulse_axis, output_dir))

    for impulse_size in impulse_size_list:
        output_files.append(run_iteration(wc, vmax, amax, total_time, impulse_time, impulse_size, impulse_axis, output_dir))
    for output in output_files:
        plot_results(output)

