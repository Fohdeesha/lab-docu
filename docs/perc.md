# H310/H710/H710P/H810 Mini & Full Size IT Crossflashing  

This guide allows you to crossflash 12th gen Dell Mini Mono & full size cards to LSI IT firmware. Mini Mono refers to the small models that fit in the dedicated "storage slot" on Dell servers. Because iDRAC checks the PCI vendor values of cards in this slot before allowing the server to boot, the generic full-size PERC crossflashing guides do not apply. This guide however solves that issue. [Technical explanation](https://github.com/marcan/lsirec/issues/1#issuecomment-574971959) for those curious. The following cards are currently supported (if you have a model not listed, [contact me](mailto:jon@fohdeesha.com?subject=PERC-Unknown)):

* H310 Mini Mono
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
Ensure there is only one LSI-based adapter in your system. If there are others besides the adapter you intend to flash, remove them! Also in the server BIOS settings, under Integrated Devices, ensure both `SR-IOV Global Enable` and `I/OAT DMA Engine` are **disabled**. They are disabled by default, so unless you've changed them in the past you should be good.  

**Remove the RAID battery** from the adapter. The IT firmware has no cache for the battery to back, in fact the IT firmware will have no clue the battery is there if you leave it connected. To make matters worse, in rare cases some people observed the battery holding old Dell code in the card's RAM and it made their crossflash process a pain. Just unplug/remove the battery and store it somewhere in case you return to Dell firmware.

Download the ZIP below which contains two ISOs. One is a FreeDOS live image, the other is a Debian live image. Both come prepackaged with all the required tools and files, and they can be booted either via iDRAC virtual media or by burning them to a flash drive via [Rufus](https://rufus.ie/), using `dd`, or your favorite flashing utility - up to you.  

[```Dell Perc Flashing ZIP```](https://fohdeesha.com/data/other/perc/perc-crossflash-v1.8.zip)  
```Version: v1.8```  
```ZIP Updated: 02-23-2021```  
```MD5: 577cef6f3969f04475d50ceb71d8f52d```   

>Note: If you **know** you have an H310 Mini (**not** an H710), skip all of the below and jump right to the [H310 Mini Guide](https://fohdeesha.com/docs/H310/). Otherwise, continue below.  

## Finding Your Card Revision

Boot the server off the FreeDOS ISO. Once it's booted, just run the following command, which will spit out the name and revision of your card:
```
info
```
![percflash1](https://fohdeesha.com/data/other/perc/percinfo.png)

>WARNING: If you are flashing a card in a blade chassis (like an M620 or M820), only use the "for blades" guide for your specific card model below. For example, the **H710 Mini (D1) Blade Guide**. Following any of the "normal" guides on a blade server will brick the PERC

If it displays an **H310 Mini**, proceed to the [H310 Mini Guide](https://fohdeesha.com/docs/H310/)  
If it displays an **H710 Mini B0** revision, proceed to the [H710 Mini (B0) Guide](https://fohdeesha.com/docs/H710-B0/)  
If it displays an **H710 Mini D1** revision, proceed to the [H710 Mini (D1) Guide](https://fohdeesha.com/docs/H710-D1/)  
If it displays an **H710 Mini D1** revision **and it's in a blade server**, proceed to the [H710 Mini (D1) Blade Guide](https://fohdeesha.com/docs/H710-D1-Blade/)  
If it displays an **H710P Mini B0** revision, proceed to the [H710P Mini (B0) Guide](https://fohdeesha.com/docs/H710P-B0/)  
If it displays an **H710P Mini D1** revision, proceed to the [H710P Mini (D1) Guide](https://fohdeesha.com/docs/H710P-D1/)  
If it displays an **H710 Adapter B0** revision, proceed to the [H710 Full Size (B0) Guide](https://fohdeesha.com/docs/H710-B0-full/)  
If it displays an **H710 Adapter D1** revision, proceed to the [H710 Full Size (D1) Guide](https://fohdeesha.com/docs/H710-D1-full/)  
If it displays an **H710P Adapter B0** revision, proceed to the [H710P Full Size (B0) Guide](https://fohdeesha.com/docs/H710P-B0-full/)  
If it displays an **H710P Adapter D1** revision, proceed to the [H710P Full Size (D1) Guide](https://fohdeesha.com/docs/H710P-D1-full/)  
If it displays an **H810 Adapter B0** revision, proceed to the [H810 Full Size (B0) Guide](https://fohdeesha.com/docs/H810-B0-full/)  
If it displays an **H810 Adapter D1** revision, proceed to the [H810 Full Size (D1) Guide](https://fohdeesha.com/docs/H810-D1-full/)  

If it displays anything that doesn't exactly match the above choices, [contact me](mailto:jon@fohdeesha.com?subject=PERC-Unknown) with a screenshot. If you're impatient and pick the "closest one" instead, you'll brick your card.

## Extra: Disable ThirdPartyPCIFanResponse

>Warning: Use this at your own risk.  Modifying thermal settings can cause unforeseen circumstances.  If you are running your server in a warm environment, it is probably best to leave this alone.

The LSI firmware is not supported by Dell.  This will cause the iDRAC to no longer keep track of the drive temperatures.  This is confirmed with the error PCI3018 in the Lifecycle Log and the fans being set to a static speed of about 30%.  The fan speed acts as a failsafe to prevent any disks from possibly overheating.  

To correct this behavior, you can disable the "ThirdPartyPCIFanResponse" feature by using IPMItool or RACADM.  IPMItool is built into the live image so this will usually be the easiest option.  If you are no longer booted into the live image, use the RACADM option instead to disable via SSH.

### Option 1: Disable ThirdPartyPCIFanResponse using IPMItool via Linux Shell

IPMItool is built into the live image.  Otherwise, you will need to install IPMItool in a Linux environment first.

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

Connect to port 22 on your iDRAC's IP address using your preferred SSH client.

To disable ThirdPartyPCIFanResponse, run the following command via the SSH client:
```
racadm set system.thermalsettings.ThirdPartyPCIFanResponse 0
```

To verify if the option is enabled or not, run this command via the SSH client:
```
racadm getsystem.thermalsettings.ThirdPartyPCIFanResponse
```

If you need to enable the feature again, run this command via the SSH client:
```
racadm set system.thermalsettings.ThirdPartyPCIFanResponse 1
```

>Note: This information was collected from the Dell White Paper "Disabling a Third-Party PCIe Card Cooling Response with Dell PowerEdge Servers" which can be referenced here: https://fohdeesha.com/data/other/perc/ThirdPartyPCIFanResponse.pdf .  The relevant information can be found on pages 6 and 7.

## Extra: Revision Info & Part Numbers
The main difference between the B0 and D1 revisions is the D1 will link at PCIe 3.0 speeds, instead of PCIe 2.0. This will almost certainly never cause a bottleneck unless you have every SAS port on the card connected to very fast SSDs that all get hammered at the same time. Even then, you're likely to reach the card's processor limitations before the bus bandwidth limit. The difference with the *P* cards (H710P) is more cache (1GB vs 512MB), but this is totally irrelevant when running the card flashed to IT mode as the cache is not used.

Regardless, since they're the same price used, you may as well grab the D1 revision. Note that many ebay sellers just stick popular part numbers in the listing title, so try to verify in the actual auction images that the label states the correct part number. Note that most sellers omit the leading `0` so you'll get more results doing the same:

**H710 B0 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 017MXW  

**H710 D1 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 0PX45J  

**H710P B0 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 0V9RNC  

**H710P D1 Full Size Part Numbers:**  
- These full size cards are still $$$ for some reason  
- Unless you already have one just buy an actual LSI card for half the price  
- 07GCGT  

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

**H710 D1 Mini For Blades Part Numbers:**    
- 062P9H  

**H710P B0 Mini Part Numbers:**  
- 0TTVVV  
- 0N3V6G  

**H710P D1 Mini Part Numbers:**  
- 0TY8F9  

**Unknown:**  
- 0PK2W9  

### Contributing:
The markdown source for these guides is hosted on [**my Github repo.**](https://github.com/Fohdeesha/lab-docu) If you have any suggested changes or additions feel free to submit a pull request.  

```Documentation version:``` [ v2.4 (02-23-2021)](https://github.com/Fohdeesha/lab-docu/commits/master) 
