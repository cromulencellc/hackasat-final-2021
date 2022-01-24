#!/bin/bash
# we pressume nothing else is using /mnt/rfs*
# but check anyway
#
# VMDK points to disk image link.
VMDK=build/tmp/deploy/images/zynq-generic/petalinux-image-minimal-zynq-generic.wic.vmdk
# echo "using $VMDK"

MNT=$( mount | grep rfs | awk '{print$3}' ) 


if [[ $1 =~ ^[1-2]+$ ]] ; then
    if [ "$MNT" ] ; then
	echo ""
	echo "found existing mount: $MNT"
	echo "only 1 at a time please, run vmdk_unmount.sh"
    else
	echo "mounting $VMDK on /mnt/vmdk$1 "
	# we need to run as root since the FS being mounted 
	# is creatd by root.
	export LIBGUESTFS_BACKEND=direct
	sudo guestmount -a $VMDK -m /dev/sda$1 /mnt/vmdk$1
    fi
else
    # all in one help message:
    echo ""
    echo "This script provides access to add files and poke around"
    echo "the generated .vmdk emulation disk image"
    echo ""
    echo "Currently only the second partition is used by emulation"
    echo "Partion 1, the /boot is ignored" 
    echo "sudo is needed to run script and access mounted partiion."
    echo ""
    echo "The disk image is hard coded in script to "
    echo ""
    echo "$VMDK"
    echo ""
    echo "Remember to unmount (vmdk_unmount.sh) before running emulation !!"
    echo "======================== "
    echo ""
    echo "help for possible issues:"
    echo ""
    echo "please specify partion number 1 or 2:"
    echo "./vmdk_mount.sh 2"
    echo ""
    echo "mount point must exist: /mnt/vmdk1 or /mnt/vmdk2"
    echo "sudo mkdir /mnt/vmdk2"
    echo ""
    echo "missing 'guestmount' program is cured by"
    echo "sudo apt-get install libguestfs-tools"
fi
       
# get error if try to mount both, likely due to using /dev/fuse
# /dev/fuse on /mnt/vmdk2 type fuse (rw,nosuid,nodev,relatime,user_id=0,group_id=0)
# sudo guestmount -a vmdk.vmdk -m /dev/sda2 /mnt/vmdk2
#
# need to premake the mount point:
# mounting build/tmp/deploy/images/zynq-generic/petalinux-image-minimal-zynq-generic.wic.vmdk on /mnt/vmdk
# guestmount: access: -m: No such file or directory#
