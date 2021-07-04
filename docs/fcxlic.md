# FCX Licensing
**DISCLAIMER:** This is intended for educational use only, or in a homelab type environment.  

Brocade switches used to be licensed via a physical EEPROM with a magic string written into it. When the OS booted and saw this magic string, it knew to enable advanced features.  

The FCX was the last Brocade model to use this scheme before moving to keyed software licenses. They even shipped with a blank EEPROM in the socket ready to go. This means we just need to write the magic string to this EEPROM to fully unlock licensed features.

## What Does This Do?
On the Brocade FCX, the ```ADV``` license in this guide unlocks the following:

- BGP
- VRFs
- GRE Tunnels
- IPv6-in-IPv4 tunnels - will terminate Hurricane Electric tunnels etc

## Update The Switch
To do this, you need the latest bootloader and OS - this is not optional! Follow the update guide to upgrade to the latest firmware image AND bootloader if you haven't already - [FCX Update & Start Guide](http://fohdeesha.com/docs/fcx/)  

When upgraded and configured, move on to the below.

## Enter Bootloader

We need to enter the bootloader - Connect to the switches serial/console port using a program like Putty (9600 8N1). Power on the switch while watching your serial terminal - it will have a prompt saying `Hit b to enter the boot monitor` - press `b` quickly and you'll be dropped into the bootloader prompt.

## Write The Magic String
We just need to use an `i2c write` command in the bootloader to write the unlock code to the license EEPROM inside the FCX:
```
i2c write a 0 feedface00000200ffffffffffffffff 1
```
That's it, now reboot the switch to the full OS:
```
reset
```

## Checking The License
Once it's fully booted into the OS, run `show version` - you should see the following line near the middle, look for the `PREM` and `PROM-TYPE FCX-ADV-U` - this means it's detected the license EEPROM and unlocked the `ADV` license:
```
HW: Stackable FCX648S-PREM (PROM-TYPE FCX-ADV-U)
```

If you see that, you're good to go and fully licensed.