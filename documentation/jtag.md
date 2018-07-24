# PowerPC JTAG Recovery
This is a brief guide to recover corrupt bootloaders on PowerPC 85xx switches using a BDI2000. 

## Preparation
Start by downloading [this ZIP](http://fohdeesha.com/data/other/jtag-recovery.zip).

Put all the files from the zip into the root of your TFTP server - the JTAG unit has been preprogrammed to look for a TFTP server at 192.168.0.1  - make sure the TFTP server is running at this address.

If the TFTP server is running on linux, you need to create an empty file in the TFTP root called `badboot.bin` - and give it write permission by running `CHMOD 777 badboot.bin` - this allows the JTAG unit to write to this file. if you are using a windows TFTP server, this shouldn't be necessary.

## Connections
If you are outside the US, please plug the power brick into the wall first WITHOUT CONNECTING IT TO THE BDI2000! This is to test the power adapter, if it blows up from 220 volts, it won't take the BDI2000 with it.  

If you plug it into the wall and nothing happens (it shouldn't), unplug it from the wall. Now plug your ethernet cable into the BDI2000 (which should still be powered off).  

Now very carefully connect the other end of the power brick to the BDI2000 (the power brick should not be plugged into the wall yet) - plug it into the top set of power pins, matching the red and green stripes. MAKE SURE TO GET THIS RIGHT! See the following picture for the correct way:  

Once you are sure it is plugged in correctly, and it is also connected to your network via ethernet, plug the power brick into an outlet - the BDI2000 should power up.  

Using a telnet capable program like putty, telnet to the BDI2000 unit - it has been preconfigured with an IP of 192.168.0.2 - You should get output like the below, saying it's waiting for a target:

```
TARGET: waiting for target Vcc
TARGET: waiting for target Vcc
TARGET: waiting for target Vcc
```

Now you must connect the BDI2000 to your switch. Make sure the switch is unplugged and powered off! Find the 16-pin JTAG header on the switch. On the JTAG header, `1` should be printed on one side of the header to indicate pin number 1. Find which side of the header that `1` is on.  

Orient, then plug in the cable so pin #1 is on the left side, like this:


On an LB6M, the connection should look like this:

Now that everything is plugged in and the BDI2000 is powered, power up the switch.  

**Startup Order Summary:**  
1. Connect ethernet to the BDI2000  
2. Connect the power brick to the BDI2000  
3. Connect the BDI2000 to the switch JTAG header  
4. Power on the switch  

## JTAG Commands

When the switch powers up, you should get some new output in your telnet connection to the unit, similar to the below:

```
...
- TARGET: resetting target passed  
- TARGET: processing target startup ....  
- TARGET: processing target startup passed
```
Make sure you got `processing target startup passed` at the end, this means the CPU is successfully halted. Test the JTAG session by reading the beginning bootloader area of flash:

```
md 0xfff80000
```
Depending on how corrupt your bootloader area is, you should see something that looks similar to the u-boot or brocade beginning contents below (might not match exactly, depending on what bootloader you are running):
```
fff80000: 4d554348 02057be5 0005a2d6 00004058    MUCH..{.......@X
fff80010: 00000000 00012f2c 0004d880 00600028    ....../,.....`.(
fff80020: 00030030 0004ffff ffffffff 00000000    ...0............
fff80030: 4e6ab6ae 07030000 74727a30 37333030    Nj......trz07300
```

If you get something similar to above, the JTAG session is working.  

Now we tell it to copy the bootloader section of flash to a file on your TFTP server named `badboot.bin` - this is an important step as it will let me see exactly what in the bootloader got corrupted and how. Remember that for a tftp server on linux, this file needs to already exist (but be empty) and have permissions of 777.  

Once that finishes, we can write the new bootloader. First we must erase the bootloader section of flash properly before we can write to it:
```
erase 0xfff80000 0x20000 4
```
Now we copy the bootloader from the TFTP server to onboard flash:
```
prog 0xfff80000 brocadeboot.bin bin
```
When that finishes, connect a serial console cable to the LB6M switch (while it is still running and connected to JTAG). Open up a serial program so you can watch it's output. We will tell the switch to reset and run like normal (run the following command in the JTAG telnet console):
```
reset run
```
The switch should boot normally and you should see it's usual boot text in your serial console. If you don't see any text in your serial window and the switch seems like it's still bricked, halt it again from the JTAG window by running `reset halt` and follow the above erasing and flashing steps again as you must have skipped something.  

Now that it's working and running, unplug the switch power first! When the switch is off and powered down, then unplug the power cable from the BDI2000, then disconnect it from the JTAG header, and you are done. **PLEASE SEND ME THE badboot.bin FILE FROM YOUR TFTP SERVER!** This will allow me to see exactly what got corrupted on your switch.

**Shutdown Order Summary:**  
1. Power down the switch (unplug power)  
2. Power down the BDI2000 (unplug power brick)  
3. Disconnect the BDI2000 16 pin cable from the JTAG header   
