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
## Configuration Details

Now your switch is set up for basic switching, and has an IP address you can use to manage the switch.  **Note:**  You should now unplug your ethernet connection from the dedicated management port, and instead connect it to any of the  **regular switch ports**. Then continue below to give the switch a friendly name:
```
hostname intertubes
```
Now tell it to generate an RSA keypair - this is the first step to enable SSH access:
```
crypto key zeroize
crypto key generate rsa modulus 2048
```

## Update PoE Firmware
If your switch is the PoE model, you need to update the PoE controller firmware. If it's a non-PoE model, skip this section. Assuming you completed the previous section and the switch now has in-band network access, just do the following:
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

## If Access Protection Is NOT Required
If you do **not** want to password protect access to the switch (you're using it in a lab), follow this section. If you'd like to password protect it, skip this section.

Allow SSH login with no passwords configured:
```
ip ssh permit-empty-passwd yes
```

## If Access Protection IS Required (or WEB-UI Access)
If you do want to secure access to the switch, or use the (limited) web UI, follow this section. If not, skip it.

To secure the switch, we need to create an account - "root" can be any username string you wish:
```
username root password yourpasshere
```
We also need to tell it to use our new local user account(s) to authorize attempts to log in or access the web UI:
```
aaa authentication login default local
aaa authentication web default local
```
If you wanted to use the web UI, you can now log into it using the credentials you created above.

You should enable authentication for telnet access as well:
```
enable telnet authentication
```
If your switch is outside of your home, or accessible by others in any way, telnet should be disabled entirely, and access to the serial console should also be password protected. Otherwise skip this step at your discretion:

```
no telnet server
enable aaa console
```
### OPTIONAL: Key Based SSH Access
If you have followed the above to set up authentication, and also wish to disable password-based SSH login and set up a key pair instead, follow this section. If not, skip it. Enable key login, and disable password login:
```
ip ssh key-authentication yes
ip ssh password-authentication no
```
Now we have to generate our key pair with [puttygen](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) on windows or ```ssh-keygen -t rsa``` on linux. The default settings of RSA @ 2048 bits works without issue. Generate the pair and save out both the public and private key.  
>The ICX6xxx series do *not* support 4096 bit keys - when using `ssh-keygen` or `puttygen` etc, you must create 2048 bit keys.

Copy the public key file to your TFTP server. Then use the following command to import it into your switch:
```
ip ssh pub-key-file tftp 192.168.1.8 public.key
```
You shouldn't need to be told basic key management if you're following this section, but just in case - copy your private key to the proper location on the *nix machine you'll be SSH'ing from, or if you're on windows, load it using [pageant](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html). Now when you SSH to the switch, it will authenticate using your private key.

## Saving & Conclusions
Whenever you make changes (like above) they take effect immediately, however they are not saved to onboard flash. So if you reboot the switch, they will be lost. To permanently save them to onboard flash, use the following command:
```
write memory
```
Your switch now has a basic configuration, as well as an IP address you can telnet or SSH to for further configuration.

Some more useful general commands:

Show chassis information like fan and temperature status:
```
show chassis
```

Show a table of all interfaces:
```
show interface brief
```
To show one interface in detail:
```
show interfaces ethernet 1/1/1
#Also works for virtual interfaces:
show interfaces ve 1
```
Give a port a friendly name:
```
interface ethernet 1/1/1
port-name freenas
show interfaces brief ethernet 1/1/1
exit
```
Show the running configuration:
```
show run
```
Show the system log:
```
show log
```

To remove configuration options, put a ```no``` in front of them at the appropriate CLI level:
```
no hostname beefbox
```


## Tips
To exit the CLI level you are at, use `exit`. So assuming you are still at the ```configure terminal``` level, type the following to exit back to the ```enable``` level:
```
exit
```
Commands can also be shortened, as long as they are still unique. So to re-enter the configure terminal level, Instead of typing the entirety of ```configure terminal```, the following will also work:
```
conf t
```
There is also tab help and completion. To see all the commands available at the current CLI level, just hit tab. To see the options available for a certain command, just type that command (like ```ip```) then hit tab a couple times.

## Advanced Configuration
### Default Route & DNS
To give the switch a default route and a DNS server so it can reach external hostnames and IP's (to ping external servers or to update time via NTP etc), do the following. replace the IP with the IP of your gateway/router/etc. Assuming you are still at the ```configure terminal``` level:

```
ip dns server-address 192.168.1.1
ip route 0.0.0.0/0 192.168.1.1
```
### NTP
To have the switch keep its time synced via NTP (so its logs make more sense), use the following. If you live in an area that doesn't use Daylight Savings, skip the ```clock summer-time``` command. Use tab completion for the timezone command to see what's available. The IP's in the following example are google's NTP servers and work well for most cases:
```
clock summer-time
clock timezone gmt GMT-05
ntp
disable serve
server 216.239.35.0
server 216.239.35.4
exit
```
### SNMP

To quickly enable SNMPv2 (read only), follow the below. SNMP v3 is available but you'll have to refer to the included documentation:
```
snmp-server community public ro
```

### Saving
If you made any of the above extra changes, remember they have not been saved to onboard flash yet. Do so:
```
write memory
```

## SFP/Optics Information
Brocade does not restrict the use of optics by manufacturer, they'll take anything given it's the right protocol. However optical monitoring information is disabled unless it sees Brocade or Foundry optics.

So if you want to see information like this :

```
telnet@Route2(config)#sh optic 5
 Port  Temperature   Tx Power     Rx Power       Tx Bias Current
+----+-----------+--------------+--------------+---------------+
5       32.7460 C  -002.6688 dBm -002.8091 dBm    5.472 mA
        Normal      Normal        Normal         Normal
```
You'll need to pick up some official Brocade or Foundry optics on ebay, or buy some flashed optics from FiberStore.  

## Noise Info (ICX6610)
The ICX66610 power supply bricks came in 3 revisions: A, B, and C. These will have an affect on how loud the switch is. Generally with an A revision PSU anywhere, even combined with a B or C revision in the other PSU slot, the PSU fans will run louder.

The switch will be much quieter if you can find and run B and/or C revision bricks. There's no difference noise wise between B and C. Most sellers do not list the revision so you have to check the ebay pictures [for the revision marking.](store/psu-rev.jpg)

Also, it seems the PSU and fan tray position also affect fan speed. If you are running 1 PSU and fan tray, position them in the furthest left slots, when looking at the switch from the rear. If you have 2 PSUs and 1 fan tray, position the fan tray in the furthest left fan tray slot when looking at the rear of the switch.  

## Licensing Info
Like other EoL ICX6xxx series switches, the ICX6610 requires license file unlocks for 10gbE port functionality as well as some advanced software features. These licenses are no longer sold. To prevent e-waste and save these from landfills, I am providing free unlocks for use in a homelab or personal environment. Once you've finished the update and configuration process, proceed to the [ICX6610 Licensing](6610.md) section to fully license and unlock your switch. If you have an FCX instead, you can use the [FCX Licensing](fcxlic.md) section.