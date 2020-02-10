# H310/H710/H710P Mini IT Mode Crossflashing
This guide allows you to crossflash 12th gen Dell Mini Mono cards to LSI IT firmware. Mini Mono refers to the small models that fit in the dedicated "storage slot" on dell servers. Because iDRAC checks the PCI vendor values of cards in this slot before allowing the server to boot, the generic full-size PERC crossflashing guides do not apply. This guide however solves that issue. [Technical explanation](https://github.com/marcan/lsirec/issues/1#issuecomment-574971959) for those curious.  **If you have the full size cards, do NOT use this guide.** The following cards are supported:

* H310 Mini Mono
* H710 Mini Mono
* H710P Mini Mono

Please note that we're not the first to do this by far, a few eBay sellers figured this out and have kept it a secret in order to crossflash these themselves, then sell hundreds of them for 4x the cost of the original cards. In the spirit of open source, this guide is intended to allow users to save money and do it themselves. If you're lazy and want a pre-flashed card, [eBay](https://www.ebay.com/sch/theartofserver/m.html) is still an option.

## Thanks
[**Jon Sands**](http://fohdeesha.com/)  
[**Arkadius**](https://phoxden.net/)  
**WildOne69**  
[**Various STH Members**](https://forums.servethehome.com/index.php?threads/perc-h710-mini-to-it-mode.25448/page-3)  
## Why

There's two main reasons to do this, assuming you don't need the hardware RAID functionalities of the stock firmware:  

**Better Performance:** the plain LSI IT firmware in this guide drastically increases the max [queue depth](https://www.settlersoman.com/what-is-storage-queue-depth-qd-and-why-is-it-so-important/) of the adapter. For instance on the H310 Mini, it goes from 25 with the stock Dell RAID firmware to 600 with the LSI IT firmware. This can mean drastic improvements in IOPS with heavy SSD configurations for example.

**Different Driver:** The stock Dell firmware (MegaRAID-based) uses the MegaRAID driver in Linux/FreeBSD/etc. In some distributions this can cause issues, for example FreeNAS has issues pulling SMART data from drives connected to a MegaRAID controller. After crossflashing to IT mode, the card is used by the OS via the much simpler mpt2sas driver.

## Preparation
Download the ZIP below which contains two ISOs. One is a freeDOS live boot image, the other is a Debian live boot image. Both come prepackaged with all the required tools and files, and they can be booted either via iDRAC virtual media or by burning them to a flash drive via [Rufus](https://rufus.ie/) or similar - up to you.  

[```Mini Mono Flashing ZIP```](https://fohdeesha.com/data/other/perc/perc-crossflash.zip)  
```Version: v1.0```  
```ZIP Updated: 02-09-2020```  
```MD5: 6f4b1c6b2f1faf4c8c5d1f647b6505f2```  

## Finding Your Card Revision
Ensure there is only one LSI-based adapter in your system. If there are others besides the mini PERC card, remove them! Boot the server off the FreeDOS ISO. Once it's booted, just run the following command, which will spit out the name and revision of your card:
```
info
```
![percflash1](https://fohdeesha.com/data/other/perc/percinfo.png)

>Note: the `grep.exe: Memory Exhausted` error is normal. This is because running `grep` under DOS goes against God's design

If it displays an **H310 Mini**, proceed to the [H310 Guide](https://fohdeesha.com/docs/H310/)  
If it displays an **H710 Mini B0** revision, proceed to the [H710 (B0) Guide](https://fohdeesha.com/docs/H710-B0/)  
If it displays an **H710 Mini D1** revision, proceed to the [H710 (D1) Guide](https://fohdeesha.com/docs/H710-D1/)  
If it displays an **H710P Mini B0** revision, proceed to the [H710P (B0) Guide](https://fohdeesha.com/docs/H710P-B0/)  
If it displays an **H710P Mini D1** revision, proceed to the [H710P (D1) Guide](https://fohdeesha.com/docs/H710P-D1/)  

If it displays anything that doesn't exactly match the above choices, [contact me](mailto:jon@fohdeesha.com?subject=PERC-Unknown) with a screenshot.

## Revision Info & Part Numbers
The main difference between the B0 and D1 revisions is the D1 will link at PCIe 3.0 speeds, instead of PCIe 2.0. This will almost certainly never cause a bottleneck unless you have every SAS port on the card connected to very fast SSDs that all get hammered at the same time. The difference with the *P* cards (H710P) is more cache (1GB vs 512MB), but this is totally irrelevant when running the card flashed to IT mode as the cache is not used.

Regardless, since they're the same price used, you may as well grab the D1 revision. Note that many ebay sellers just stick popular part numbers in the listing title, so try to verify in the actual auction images that the label states the correct part number. Note that most sellers omit the leading `0` so you'll get more results doing the same:

**H710 B0 Part Numbers:**  
- 0MCR5X  
- 0FRH64  

**H710 D1 Part Numbers:**    
- 05CT6D  

**H710P B0 Part Numbers:**  
- 0TTVVV  

**H710P D1 Part Numbers:**  
- 0TY8F9  

**Unknown:**  
- 0PK2W9  
- 0N3V6G  

### Contributing:
The markdown source for these guides is hosted on [**my Github repo.**](https://github.com/Fohdeesha/lab-docu) If you have any suggested changes or additions feel free to submit a pull request.  

```Documentation version:``` [ v1.1 (02-10-2020)](https://github.com/Fohdeesha/lab-docu/commits/master) 