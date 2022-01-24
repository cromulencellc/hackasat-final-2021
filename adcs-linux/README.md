# Petalinux on MYC-Y7Z010

## Setup Petalinux

### Obtain Petalinux Installer

https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html
 
### Install Petalinux

Dependencies

```bash
sudo apt-get install -y iproute2 gcc g++ net-tools libncurses5-dev zlib1g:i386 libssl-dev flex bison libselinux1 \
gnupg wget xterm autoconf texinfo zlib1g-dev gcc-multilib build-essential screen pax gawk python3 python3-pexpect python3-pip \
python3-git socat chrpath python3-jinja2 xz-utils debianutils iputils-ping libegl1-mesa libsdl1.2-dev pylint3 \
cpio pax unzip gzip libtool

cd ${HOME}
./petalinux-v2020.2-final-installer.run -p "arm" -d petalinux
```

From <https://forums.xilinx.com/t5/Embedded-Linux/petalinux-2019-1-installation-on-ubuntu-18-04-02-Error/td-p/994695> 

See docs

# Setup Instructions

## Use ADCS Petalinux Project (adcs-linux)

1. Open your favorite terminal and type the following:

```bash
source petalinux/settings.sh
./build_project.sh
```
2. Insert SD Card with boot and rootfs partitions.
3. Copy build artifacts to SD CArd

```bash
source petalinux/settings.sh
./copy_to_sd.sh
```

## PROJECT BUILD DOCUMETNATION (NOT NEEDED if cloning repo)
### Project Creation Instructions (if rebuild needed)

Open your favorite terminal and type the following:

```bash
source /home/$USER/petalinux/settings.sh
petalinux-create --type project --template zynq --name adcs-linux
cd adcs-linux
petalinux-config --get-hw-description /home/$USER/adcs-zynq/hw/
```
Set the following hardware/boot settings to setup SD boot and external rootfs (Ubuntu 18.05.5)
- Subsystem AUTO Hardware Seetings -> Advanced bootable images storage Settings -> boot image settings
    - Set 'image storage media' = 'primary sd'
- Subsystem AUTO Hardware Settings -> Advanced bootable images storage Settings -> kernel image settings
    - Set 'images storage media' = 'primary sd'
- Subsystem AUTO Hardware Settings -> Advanced bootable images storage Settings -> dtb image settings
    - Set 'image storage media' = 'primary sd'
- Image Packaging Configurations
    - Set 'Root filesystem type' = 'EXT4' (so rootfs is on SD card second partition)


Run initial project buld
```bash
petalinux-build
```

### Configure kernel
```bash
source petalinux/settings.sh
cd adcs-linux
petalinux-config -c kernel
```

Extra Kernel Settings Enabled:
- CONFIG_POSIX_MQUEUE=y
- CONFIG_POSIX_MQUEUE_SYSCTL=y
- CONFIG_SERIAL_UARTLITE=y
- CONFIG_SERIAL_UARTLITE_CONSOLE=y
- CONFIG_I2C_XILINX=y
- CONFIG_UIO_PDRV_GENIRQ=y
- CONFIG_IRQ_POLL=y

Commit Kernel Updates:

```bash
source petalinux/settings.sh
petalinux-devtool finish linux-xlnx /home/${USER}/adcs-linux/project-spec/meta-user/
```

### Run Complete Clean Build

```bash
source petalinux/settings.sh
petalinux-build -x mrproper -f
petalinux-build
```

### Package Build Ouputs

```bash
source petalinux/settings.sh
petalinux-package --boot --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system.bit --u-boot --force
```

### Create SD Card
Open gparted tool
```bash
sudo gparted
```
1. Select the SD card in gparted
2. Make sure its unmounted and delete the partition of the SD card so that it displays ‘unallocated’ in gparted
3. Right click the unallocated space and create a new partition with following settings, Free Space Proceeding (MiB): 4, New Size (MiB) : 1024, File System : FAT32, Label : BOOT. Don’t change other settings and click Add to finish.
4. Right click the remaining unallocated space and create a new partition with following settings, Free Space Proceeding (MiB): 0, Free Space Following(MiB): 0, File System : ext4, Label : rootfs. Don’t change other settings and click Add to finish.
5. Apply all changes to create the partitions.

### Build Ubuntu Rootfs
1. Change dir to packer-bulder-arm
```bash
cd ../packer-builder-arm
```
2. Run packer-builder-arm to create rootfs This takes like 20 minutes.
```bash
cd packer-builder-arm
docker run --rm --privileged -v /dev:/dev -v ${PWD}:/build packer-builder-arm build boards/hack-a-sat/adcs_ubuntu_base_18.04_armhf.json
```
3. Copy resulting rootfs tar ball to adcs-linux
```bash
cp adcs-001.tar.gz /home/${USER}/adcs-linux/rootfs/
```

### Build SDCard Image and VMDK image from kernel and rootfs outputs
1. Return to adcs-linux and run packing script
```bash

cd adcs-linux
source petalinux/settings.sh
petalinux-package --wic --wks petalinux-ubuntu.wks --rootfs-file adcs-001.tar.gz
```
2. This results in two blob images, petalinux-sdimage.wic (burn to SD)


### Burn SDCard Image
1. Slow way
```bash
sudo dd if=images/linux/petalinux-sdimage.wic of=/dev/sdb bs=32M
```
2. Faster (more reliable)
- Download balena etcher (used commonly for rasperry pi SD card burns)
- Run
- Follow instrutctions to burn petalinux-sdimage.wic to SD Card.

### Manual Deployment of Petalinux Build Outputs and Rootfs to SD boot partions
1. Copy kernel, uboot, and device tree blob
```bash
cp images/linux/BOOT.BIN /media/${USER}/BOOT
cp images/linux/boot.scr /media/${USER}/BOOT
cp images/linux/system.dtb /media/${USER}/BOOT
cp images/linux/image.ub /media/${USER}/BOOT
```
2. Get linux headers for kernel build by petalinux
```bash
cd images/linux
mkdir -p tmp
tar xf rootfs.tar.gz -C tmp
cd tmp
sudo cp -r lib/modules/* //media/${USER}/rootfs/lib/modules
```


## Resources
https://www.xilinx.com/support/documentation/sw_manuals/xilinx2020_2/ug1144-petalinux-tools-reference-guide.pdf