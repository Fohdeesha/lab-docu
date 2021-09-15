# Setting up Brocade FCX & ICX6610

## Preparation
If you haven't already, visit the [Brocade Overview](brocade-overview.md) page to download the latest guide package and get it ready to be served via TFTP - this unified package includes all the latest firmware, licenses, and documentation.  

**Connect** to the serial/console port on the front using a program like Putty (9600 8N1), then connect the **management ethernet port** to your network (do NOT use any of the regular switch ports yet).

Power on the switch while watching your serial terminal - it will have a prompt saying ```Hit b to enter the boot monitor``` - press ```b``` quickly and you'll be dropped into the bootloader prompt, now we can upgrade the software. If you missed the prompt and it boots the OS instead, pull power and try again.

At the boot prompt, we need to give it a temporary IP in the subnet of your network, so it can talk to your TFTP server. This IP will not be used again after it leaves the bootloader. Replace the IP in the example below with an unused IP on your subnet:

```
ip address 192.168.1.50/24
```
Now we can update the bootloader. Replace the IP with the IP of your tftp server:
```
copy tftp flash 192.168.1.8 ICX6610-FCX/grz10100.bin boot
```

After a few seconds it should finish, then we can flash the main OS. Replace the IP with the IP of your tftp server:
```
copy tftp flash 192.168.1.8 ICX6610-FCX/FCXR08030u.bin primary
```
It will take a little longer. When it finishes, we need to erase the current config & any passwords, then tell the switch to reboot into the full OS:

```
factory set-default
reset
```
**Note:** If you get an error stating `factory set-default` is not a valid command, this means your switch has a very old bootloader. In that case since we just flashed the latest bootloader, we just have to tell the switch to reset. Then it will load the new bootloader, and the command will work:
```
#only follow this section if factory set-default did not work
reset
#hit b a bunch so it drops into the bootloader again
#now it's booted the latest bootloader, so the command will work:
factory set-default
reset
#let it fully boot like normal and move to the next section
```

**Note:** If your switch says that it is part of a stack even after doing the above, then you will need to unstack it before being able to configure and update it. You'll see several prompts when booting that it is a stack member and the command menu will show stack specific commands (ex: stack, simulate-non-stacking-unit). You can unstack your switch by running the following command:
```
stack unconfigure clean
# hit y at the prompt
```
You can then continue on to the below.


## Initial Configuration

Now that it's booted into the full OS you may get  **_TFTP timed out_**  errors in the console, this is normal. just hit enter until they go away. We'll fix that in the next section. Now to make any changes we must enter the enable level:
```
enable
```
Now we enter the configure terminal level to make config changes:

```
configure terminal
```
Now we turn off the DHCP client, so it doesn't automatically grab an IP and look for a TFTP config (the cause of the earlier timeout messages):
```
ip dhcp-client disable
```
Now we give the switch its permanent IP address. By default, all ports are in VLAN 1, so it will behave like a typical switch. First we need to give VLAN 1 its own virtual interface:

```
vlan 1
router-interface ve 1
exit
```
Now we need to assign that virtual interface an address. Choose an IP that is unused in your subnet, and out of your DHCP server range (ping it first to be sure it's unused):
```
interface ve 1
ip address 192.168.1.55/24
exit
write mem
```
Now your switch is set up for basic switching, and has an IP address you can use to manage the switch. But you're not done yet!  
>**Note:**  You should now unplug your ethernet connection from the dedicated management port, and instead connect it to any of the  **regular switch ports**.  If you skip this, the rest of the guides will not work.

## Update PoE Firmware
If your switch is the PoE model, you need to update the PoE controller firmware. If it's a non-PoE model, skip this section. Assuming you completed the previous section and the switch now has in-band network access (one of the regular ports are connected), just do the following:
```
exit
inline power install-firmware stack-unit 1 tftp 192.168.1.8 ICX6610-FCX/fcx_poeplus_02.1.0.b004.fw
#after a few seconds, hit enter to return to cli
#save changes you made from the previous section
write memory
#reload the switch
reload
#you'll probably get a message that it hasn't finished. it can take up to 10 minutes
#run "show log" occasionally to monitor the update progress
#try the "reload" command again once it's reached 100%
#once you've rebooted back into the OS:
enable
configure terminal
```
Now you have the latest PoE firmware, and can continue on.

## Noise Info (ICX6610)
The ICX66610 power supply bricks came in 3 revisions: A, B, and C. These will have an affect on how loud the switch is. Generally with an A revision PSU anywhere, even combined with a B or C revision in the other PSU slot, the PSU fans will run louder.

The switch will be much quieter if you can find and run B and/or C revision bricks. There's no difference noise wise between B and C. Most sellers do not list the revision so you have to check the ebay pictures [for the revision marking.](store/psu-rev.jpg)

Also, it seems the PSU and fan tray position also affect fan speed. If you are running 1 PSU and fan tray, position them in the furthest left slots, when looking at the switch from the rear. If you have 2 PSUs and 1 fan tray, position the fan tray in the furthest left fan tray slot when looking at the rear of the switch.  

## Licensing Info
Like other EoL ICX6xxx series switches, the ICX6610 requires license file unlocks for 10gbE port functionality as well as some advanced software features. These licenses are no longer sold. To prevent e-waste and save these from landfills, I am providing free unlocks for use in a homelab or personal environment. Once you've finished the update and configuration process, proceed to the [ICX6610 Licensing](6610.md) section to fully license and unlock your switch. If you have an FCX, you can use the [FCX Licensing](fcxlic.md) section.