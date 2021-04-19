
# Extracting Brocade Firmware

Around the v8060 codetrain and above, Brocade seems to have moved to using the standard [FIT](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842374/U-Boot+Images) image type for their firmware files. These contain the linux kernel, ramdisk, device tree, etc, all in a standard structure, so we can extract them. This is not an exhaustive or detailed guide, some knowledge of linux etc is assumed.

## Trim the firmware file
Brocade adds some Brocade-specific metadata in front of the FIT image, so it needs to be removed before FIT tools will recognize it. For most files it seems to be 512 bytes we need to remove:
```
dd if="SPR08080e.bin" of="trimmed.bin" bs=512 skip=1
```

For the newer "UFI" images (where they've combined the bootloader and OS in one file), it seems to usually be 1360 bytes:
```
dd if=SPR08091ufi.bin of="trimmed.bin" bs=1360 skip=1
```
However some images seem to need 1276 bytes removed:
```
dd if=TNR08091ufi.bin of="trimmed.bin" bs=1276 skip=1
```
To be sure, just open the firmware file with a [hex editor](https://mh-nexus.de/en/hxd/) and highlight and delete everything before the HEX values `D0 0D FE ED` (the FIT file should start with `D0 0D FE ED`)

Now install u-boot-tools:
```
apt install u-boot-tools
```
Debian 9 (and maybe others) have a very old uboot-tools version in the default repo, and does not come with the required dumpimage program. In that case, just manually install the recent tools:
```
wget http://ftp.us.debian.org/debian/pool/main/u/u-boot/u-boot-tools_2019.01+dfsg-7_amd64.deb
dpkg -i u-boot-tools_2019.01+dfsg-7_amd64.deb
```
Now use `dumpimage` to view a list of all the components in the firmware package:
```
dumpimage -l trimmed.bin
```
You'll get a list of everything in the package:
```
root@testing:~# dumpimage -l trimmed.bin
FIT description: Linux kernel and FDT blob
Created:         Fri Jun  7 00:19:54 2019
 Image 1 (fdt@1)
  Description:  ICX7650.dtb
  Created:      Fri Jun  7 00:19:54 2019
  Type:         Flat Device Tree
  Compression:  uncompressed
  Data Size:    27517 Bytes = 26.87 KiB = 0.03 MiB
  Architecture: AArch64
  Load Address: 0x81200000
  Hash algo:    crc32
  Hash value:   fd8efcc8
 Image 3 (ramdisk@1)
  Description:  Ramdisk -rootfs
  Created:      Fri Jun  7 00:19:54 2019
  Type:         RAMDisk Image
  Compression:  lzma compressed
  Data Size:    57735137 Bytes = 56381.97 KiB = 55.06 MiB
  Architecture: AArch64
  OS:           Linux
  Load Address: 0x00000000
  Entry Point:  0x00000000
  Hash algo:    crc32
  Hash value:   0749eb07
  -----trimmed-----
```

Choose a part to extract to a separate file, putting the image number after the `-p` argument.

**Note:** The `-T` argument must always be set to `-T flat_dt`, even if you are extracting a different image type, only change the `-p` number:
```
dumpimage -T flat_dt -p 3 -i trimmed.bin ramdisk.zip
```
The above example extracts the ramdisk from the above firmware file, which is the main filesystem for the switch. If you open up the resulting `ramdisk.zip` with 7zip or similar, you'll be able to explore the switch filesystem, including the interesting init scripts under `/etc/`:

![7zip](https://fohdeesha.com/data/other/7zip.png)

**Note:** Notice `FastIron.xz` - this is the FastIron binary that gets uncompressed and ran after Linux boots, and this is what provides the FastIron interface you're used to.

## Packaging Modifications
With the appropriate tools (such as `mkimage`) you can modify the filesystem/ramdisk, kernel, etc after extraction, and repackage them for flashing back to the switch. You will most likely need to update the checksum for the new FIT package in the brocade metadata. More info on packaging FIT images [here.](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842374/U-Boot+Images)

## Bonus: Viewing FDTs

If you extract firmware device trees, like below for example:
```
dumpimage -T flat_dt -p 1 -i trimmed.bin ICX7650.bin
```
 and want to see them in human-readable form, run the following:
```
apt install device-tree-compiler
fdtdump ICX7650.bin
```

You'll get a nice human-readable view of the device tree.
