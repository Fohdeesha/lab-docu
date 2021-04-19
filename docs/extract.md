# Extracting Licenses

**Note:** This is intended for those who are curious, or who have licenses you legitimately own installed on a switch, but have lost the original license files. This is for the ICX6xxx series and is unnecessary on the ICX7xxx series with honor-based licensing (no license files to lose).

When licenses are purchased and used to unlock a switch, the license string is stored on the switch. To extract licenses from a switch back to a license file, follow the below.

## For ARM Models
Thankfully on ARM architecture Brocade devices (ICX6430, ICX6450, any ICX7xxx or newer), you can extract any applied licenses without special JTAG hardware. Just use the **Accessing Linux** guide to boot into linux.

Now that you're booted into a linux shell:  There is a `system` file in the `/fast_iron/sys/` directory which contains the license strings if the switch has been licensed:
```
cd /fast_iron/sys/
cat system
```
`cat`'ing the file, you'll get some strange characters with any present license strings buried inside (example strings randomly generated, not legit license strings):

```
LMLSk*B LoXskI065X5NfpQuE3SUCsnoadI656ajE:IIlOsgywL,UrHduINy6k5wz,:IlzS0r5S9w,sg69l0I848tL5m5SLUdf9dmIzkCtIIu5sRrL032UUNSsX6bfI6fXdkbeSE68uV?=2*B L2CrsQxIn9LdEa6NP5k10d2Rs8Tdf6l0dk0t:0jH9rSALukn99a9ygb1xIl0IdxEEssSNkUOvOPl0rosRxAtLLStLv0k22kv0lkkAXrrpM5rA0SuA6urzEIIdIaqMU20Me:p#%%%%?/ #
```
To easier discern where the license strings begin and end, you need to get the `system` file off the box and onto your desktop. Plug the management port into your network, then give the linux system a temporary IP:
```
ifconfig br0 192.168.1.56 netmask 255.255.255.0 up
```
Give it a default gateway as well:
```
route add default gw 192.168.1.1
```
Now assuming you're still in the `/fast_iron/sys/` directory, TFTP the file off the box onto your TFTP server:
```
tftp -p -l system 192.168.1.8
```
>Note: if you are running your TFTP server on linux, you need to create an empty file with this filename (system) in your tftp root directory, and chmod it with 777 so the switch can write to said empty file. Otherwise the switch will give a "file not found" error.

Open the downloaded file on your desktop with something like NotePad++, and it should look like [This](https://fohdeesha.com/data/other/system.png). License strings start with `*B ` and are followed by a string (note, there must be a space after `*B`), and the license string ends where the `NUL` characters begin.

So applying this knowledge, we can extract the following two licenses from that system file in the screenshot. You'll need to remove the newline after the `*B`, so the `*B` and then the string are on the same line. Make sure to leave a space between `*B` and the string as well:

```
#first license (example is randomly generated, not a legit license)
*B 9iCL80dHxLmvid3SxhIuk9zsalx630zHSpadlLlIfSTblqCSlSejIw5zibzBIyafEOvCrobl:OL8lwN4Rzd9zQCNIhsd,ASV0Bzgks5rlLMCwk,kA:mkdOCzZvVENVUluHX2

#second license (example is randomly generated, not a legit license)
*B pBNnASnpOSSmd55l:OUZUrU,5k8M5lLvOQCNuH6IUn:u:MbU0kSkNdrCCdIyEAIAki99zICC8dvA9LtnnLbwrAAy1OohuhnCkLb0dUHEv4xCInr:y8OBwLLso:,df3ORy5bd
```

Now you just need to stick those strings in an XML file, like `newlicense.xml` - Using the template below, you can see where to stick the license string. Note: No other fields in this file matter or are even checked, they can be left alone. Just insert the string into the `<licKey>` field:

```
<?xml version="1.0" encoding="ISO-8859-1"?>
<lic:licenseInfo xmlns:lic="http://license.brocade.com/licensefile">
  <licenseHeader>
    <serialNumber>CgHlvVduSMX</serialNumber>
    <productNumber>ICX6450-PREM-LIC-SW</productNumber>
    <description>ICX6450-PREM-LIC-SW</description>
    <version xsi:nil="true" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"/>
    <dateGenerated>20121212120317372</dateGenerated>
  </licenseHeader>
  <license>
    <featureName>ICX6450-PREM-LIC-SW</featureName>
    <featureVersion>CgHlvVduSMX</featureVersion>
    <licKey>*B 9iCL80dHxLmvid3SxhIuk9zsalx630zHSpadlLlIfSTblqCSlSejIw5zibzBIyafEOvCrobl:OL8lwN4Rzd9zQCNIhsd,ASV0Bzgks5rlLMCwk,kA:mkdOCzZvVENVUluHX2</licKey>
  </license>
</lic:licenseInfo>
```

Now you have a backup of your legitimate license file just as you would receive from Brocade.

## For PowerPC Models

On PowerPC models (FCX, ICX6610) it is more complicated, as they do not run linux. These units store the license strings directly on raw system flash in random locations. To extract these strings, you pretty much have to open the switch, attach a JTAG unit (such as a BDI2000) to the JTAG header, and dump the entire switch flash to a file, then open it in a hex editor.

Once you have a file containing a dump of the entire onboard flash, and have opened it in a hex editor, you search it for the characters `*B` - you will eventually come across the license string(s) like so:

![lic strings](https://fohdeesha.com/data/other/ppclic.png)

There's our license string, starting with `*B` as always. So, extracted, it looks like this (again, not a real license):
```
*B 8:n,UyAs6b9TXB3sowQ81lk,oLxLdkH:L:SIhdOko0lOEoIeSMl4dspO898E::LlSdEutafAA,bvuzxlr2so2Uks92ObyrkMyI5:rAd25Edr29ICgkqd3a,iv:bukvbO:,eO
```
Interestingly, on PPC platforms, when a user deletes licenses from the switch, it does not remove these strings - it just moves them to another part of flash. I have purchased ICX6610's on ebay where the previous owner deleted all licenses, so it boots up unlicensed. However after dumping the flash via JTAG, I found perfectly legitimate license strings still in flash that I was able to extract.

#### PowerPC Models, Desperate Edition
If you have a non-linux based switch (eg ICX6610) and you REALLY want to extract the licenses, but have no JTAG unit, there's still a way to extract them. You need to connect to the switch over serial, and get into the bootloader. Use a program like putty, and configure it to log the serial session (in Putty, Session > Logging > select a filename/location and choose the "printable output" option).

Now run the following in the bootloader:
```
dd f4000000 8388608
```
It will start printing the raw flash contents line by line in your serial window for a second, then stop. When it stops, press enter (NOT space), to make it continue dumping without user input, so it will dump overnight, and putty will be logging it.

This will take about 10 hours, so go to bed. In the morning, reboot the switch, and take a look at your putty log file. The dump will only be in raw hex, not ASCII - so to find your license strings you need to search the dump for `2a4220` which is the license start string `*B ` in hex. Once you find that, copy the following several blocks of hex values and paste them all into an online hex to ASCII converter, and you should get your string. Make sure you don't paste the leading memory address bits like `f40001e0:` into the converter.