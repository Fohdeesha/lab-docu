#!/bin/bash
#FreeDOS IMG Mounter
losetup -fP /root/dos-builder/CDROOT/isolinux/good.img
sleep 1
mount -o loop /dev/loop0p1 /root/dos-builder/dump/
sleep 1
echo "put desired files in /root/dos-builder/dump/"
echo "then run build.sh to unmount and build final ISO"