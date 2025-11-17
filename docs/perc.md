# H310/H710/H710P/H810 Mini & Full Size IT Crossflashing

This guide allows you to crossflash 12th gen Dell Mini Mono & full size cards to LSI IT firmware. Mini Mono refers to the small models that fit in the dedicated "storage slot" on Dell servers. Because iDRAC checks the PCI vendor values of cards in this slot before allowing the server to boot, the generic full-size PERC crossflashing guides do not apply. This guide however solves that issue. [Technical explanation](https://github.com/marcan/lsirec/issues/1#issuecomment-574971959) for those curious. The following cards are supported:

* H310 Mini Mono
* H310 Full Size
* H710 Mini Mono
* H710P Mini Mono
* H710 Full Size
* H710P Full Size
* H810 Full Size

## Thanks
[**Jon Sands**](https://fohdeesha.com/)  
[**fourlynx**](mailto:fourlynx@phoxden.net)  
**WildOne69**  
[**Various STH Members**](https://forums.servethehome.com/index.php?threads/perc-h710-mini-to-it-mode.25448/page-3)  

## Why

There's two main reasons to do this, assuming you don't need the hardware RAID functionalities of the stock firmware:

**Better Performance:** the plain LSI IT firmware in this guide drastically increases the max [queue depth](https://www.settlersoman.com/what-is-storage-queue-depth-qd-and-why-is-it-so-important/) of the adapter. For instance on the H310 Mini, it goes from 25 with the stock Dell RAID firmware to 600 with the LSI IT firmware. This can mean drastic improvements in IOPS with heavy SSD configurations for example.

**Different Driver:** The stock Dell firmware (MegaRAID-based) uses the MegaRAID driver in Linux/FreeBSD/etc. In some distributions this can cause issues, for example FreeNAS has issues pulling SMART data from drives connected to a MegaRAID controller. After crossflashing to IT mode, the card is used by the OS via the much simpler mpt3sas driver.

## Preparation
Ensure there is only one LSI-based adapter in your system. If there are others besides the adapter you intend to flash, remove them! You also need to disable a few BIOS settings. This step is not optional. In your server BIOS, **disable** all of the following:  

* Processor Settings > Virtualization Technology
* Integrated Devices > SR-IOV Global Enable
* Integrated Devices > I/OAT DMA Engine

You also **must** set the server boot mode to BIOS, not UEFI:  

* Boot Settings > Boot Mode > Set to **BIOS**  
>Note: If you're flashing a card on a non-Dell system, such as an AMD based desktop or server, make sure you find any BIOS settings related to IOMMU and Virtualization, and disable them   

When you're finished with this guide, don't forget to go back and enable Virtualization, as well as SR-IOV if you plan to use it. Switch boot mode back to UEFI as well if you were using it previously. But only once you've finished the guide!

**Remove the RAID battery** from the adapter. The IT firmware has no cache for the battery to back, in fact the IT firmware will have no clue the battery is there if you leave it connected. To make matters worse, in rare cases some people observed the battery holding old Dell code in the card's RAM and it made their crossflash process a pain. Just unplug/remove the battery and store it somewhere in case you return to Dell firmware.

**Remove all drives connected to the PERC.** This typically means any drives in the front of the server in the hot swap bays. If you forget to do this, the flashing process won't touch them or alter them, but having them connected can cause the flash process to fail and you'll be left wondering why. So pull the caddies for now!

**Download the ZIP below** which contains two ISOs. One is a FreeDOS live image, the other is a Debian live image. Both come prepackaged with all the required tools and files, and they can be booted either via iDRAC virtual media or by burning them to a flash drive via [Rufus](https://rufus.ie/) in WIndows, using `dd` in Linux, or your favorite flashing utility - up to you. If you use Ventoy, make sure to boot the FreeDOS image in memdisk mode.

[```Dell Perc Flashing ZIP```](store/perc/perc-crossflash-v2.5.zip)  
```Version: v2.5```  
```ZIP Updated: 09-28-2023```  
```MD5: d7f5f926626b295f7005c4c927158569```  

## Finding Your Card Revision

Boot the server off the FreeDOS ISO. Once it's booted just run the following command, it will spit out the name and revision of your card:
```
info
```
![percflash1](store/perc/percinfo.png)

>WARNING: If you are flashing a card in a blade chassis (like an M620 or M820), only use the "for blades" section for your card model below. Blade servers are the M620 and M820 for instance. Standard rackmount models like the R620 and R720 are **not** blade servers!

If it displays an **H310 Mini** proceed to the [H310 Mini Guide](H310.md)  
If it displays an **H310 Adapter** proceed to the [H310 Full Size Guide](H310-full.md)  
If it displays an **H710 Mini B0** proceed to the [H710 Mini (B0) Guide](H710-B0.md)  
If it displays an **H710 Mini D1** proceed to the [H710 Mini (D1) Guide](H710-D1.md)  
If it displays an **H710 Mini D1** and it's in a blade server, proceed to the [H710 Mini (D1) Blade Guide](H710-D1-Blade.md)  
If it displays an **H710P Mini B0** proceed to the [H710P Mini (B0) Guide](H710P-B0.md)  
If it displays an **H710P Mini B0** and it's in a blade server, proceed to the [H710P Mini (B0) Blade Guide](H710P-B0-Blade.md)  
If it displays an **H710P Mini D1** and it's in a blade server, proceed to the [H710P Mini (D1) Blade Guide](H710P-D1-Blade.md)  
If it displays an **H710P Mini D1** proceed to the [H710P Mini (D1) Guide](H710P-D1.md)  
If it displays an **H710 Adapter B0** proceed to the [H710 Full Size (B0) Guide](H710-B0-full.md)  
If it displays an **H710 Adapter D1** proceed to the [H710 Full Size (D1) Guide](H710-D1-full.md)  
If it displays an **H710P Adapter B0** proceed to the [H710P Full Size (B0) Guide](H710P-B0-full.md)  
If it displays an **H710P Adapter D1** proceed to the [H710P Full Size (D1) Guide](H710P-D1-full.md)  
If it displays an **H810 Adapter B0** proceed to the [H810 Full Size (B0) Guide](H810-B0-full.md)  
If it displays an **H810 Adapter D1** proceed to the [H810 Full Size (D1) Guide](H810-D1-full.md)  

On your screen, the output will probably have **Perc** before the names listed above - that's normal. After **Perc**, if it displays anything that doesn't exactly match the above choices, [contact me](mailto:jon@fohdeesha.com?subject=PERC-Unknown) with a screenshot. If you're impatient and pick the "closest one" instead, you'll brick your card.

## Extra: Disable ThirdPartyPCIFanResponse

>Warning: Use this at your own risk.  Modifying thermal settings can cause unforeseen circumstances.  If you are running your server in a hot environment, it's probably best to leave this alone

iDRAC does not expect to see a PERC card running LSI firmware - this will cause the iDRAC to no longer see the drive temperatures. In some cases, this will cause the error PCI3018 in the Lifecycle Log, and the fans will be set to a static speed of about 30%. The fan speed acts as a failsafe to prevent any disks from possibly overheating.

If you are affected by this and would like the fan behavior to return to normal, you can disable the `ThirdPartyPCIFanResponse` feature by using IPMItool or RACADM. IPMItool is built into the live image so this will usually be the easiest option.  If you don't want to use the Linux live ISO, you can use the RACADM option to disable it via SSH.

### Option 1: Disable ThirdPartyPCIFanResponse using IPMItool via Linux Shell

IPMItool is built into the Linux live ISO in this guide. Otherwise, you will need to install IPMItool in a Linux environment first.

To disable ThirdPartyPCIFanResponse, run the following command in the Linux shell:
```
ipmitool –I open raw 0x30 0xce 0 0x16 5 0 0 0 5 0 1 0 0
```

To verify if the option is enabled or not, run this command in the Linux shell:
```
ipmitool –I open raw 0x30 0xce 1 0x16 5 0 0 0
```

If you need to enable the feature again, run this command in the Linux shell:
```
ipmitool –I open raw 0x30 0xce 0 0x16 5 0 0 0 5 0 0 0 0
```

### Option 2: Disable ThirdPartyPCIFanResponse using RACADM via SSH

Connect to port 22 on your iDRAC's IP address using your preferred SSH client (you may need to enable SSH in the iDRAC web menu under iDRAC settings > Network > Services tab).

To disable ThirdPartyPCIFanResponse, run the following command via the SSH client:
```
racadm set system.thermalsettings.ThirdPartyPCIFanResponse 0
```

To verify if the option is enabled or not, run this command via the SSH client:
```
racadm get system.thermalsettings.ThirdPartyPCIFanResponse
```

If you need to enable the feature again, run this command via the SSH client:
```
racadm set system.thermalsettings.ThirdPartyPCIFanResponse 1
```

>Note: The above commands were pulled from the following Dell White Paper: [Disabling a Third-Party PCIe Card Cooling Response with Dell PowerEdge Servers](store/perc/ThirdPartyPCIFanResponse.pdf)

## Extra: Revision Info & Part Numbers
The main difference between the B0 and D1 revisions are link speeds: D1 will link at PCIe 3.0 instead of PCIe 2.0. This will almost certainly never cause a bottleneck, as you'll hit the processing limits of the SAS chipset before you run into the 20gbps full duplex bandwidth of a 4x PCIe 2.0 slot. The difference with the *P* cards (H710P) is more cache (1GB vs 512MB), but this is totally irrelevant when running the card flashed to IT mode as the cache is not used.

Regardless, since they're the same price used, you may as well grab the D1 revision. Note that many ebay sellers just stick popular part numbers in the listing title, so try to verify in the actual auction images that the label states the correct part number. Note that most sellers omit the leading `0` so you'll get more results doing the same:

**H710 B0 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 017MXW  
- 0NHD8V  

**H710 D1 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 0PX45J  
- 0VM02C

**H710P B0 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 0V9RNC  

**H710P D1 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 07GCGT  
- 0XDHXT  

**H810 B0 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 0HVCWY  

**H810 D1 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 0NDD93  

**H710 B0 Mini Part Numbers:**  
- 0MCR5X  
- 0FRH64  

**H710 D1 Mini Part Numbers:**  
- 05CT6D  

**H710 B0 Mini For Blades Part Numbers:**  
- 0WR9NT  

**H710 D1 Mini For Blades Part Numbers:**  
- 062P9H  

**H710P B0 Mini For Blades Part Numbers:**  
- 081J2H  

**H710P D1 Mini For Blades Part Numbers:**  
- 0PK2W9  

**H710P B0 Mini Part Numbers:**  
- 0TTVVV  
- 0N3V6G  

**H710P D1 Mini Part Numbers:**  
- 0TY8F9  

