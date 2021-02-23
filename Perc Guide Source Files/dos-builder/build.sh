#!/bin/bash
#FreeDOS ISO Builder
umount /root/dos-builder/dump/
sleep 1
losetup -d /dev/loop0
sleep 1
xorrisofs -o dosdisk.iso -p "Fohdeesha" -publisher "www.fohdeesha.com" -V "LSI_FREEDOS" -b isolinux/isolinux.bin -isohybrid-mbr isohdpfx.bin -no-emul-boot -boot-load-size 4 -boot-info-table -N -J -r -c boot.catalog -hide boot.catalog -hide-joliet boot.catalog CDROOT
sleep 1
echo "final ISO built at /root/dos-builder/dosdisk.iso"