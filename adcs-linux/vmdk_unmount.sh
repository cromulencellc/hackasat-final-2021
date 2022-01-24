#!/bin/bash
# this assumes only 1 item is mounted!
MNT=$( mount | grep vmdk | awk '{print$3}' ) 
if [ "$MNT" ] ; then
    echo "unmounting  $MNT"
    # we need to run as root since mounted FS 
    # is creatd by root.
    export LIBGUESTFS_BACKEND=direct
    sudo guestunmount  $MNT
else
    echo "no mount found"
fi


