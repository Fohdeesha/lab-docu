This page assumes you've already followed the update/config guide for your specific switch model. You should now have an updated switch configured with an IP address, and one of the **regular switch ports** (not the dedicated management port) plugged into your network to access said IP. 

Nothing here is necessary for your switch to continue operating as a "dumb" unmanaged switch, but the steps here are highly recommended nonetheless to set up remote management, configuration, and advanced features you might find useful.

## Naming & Key Generation
First, we can give the switch a name of your choice:
```
hostname beefchunk
```
Now tell it to generate an RSA keypair - this is the first step to enable SSH access:
```
crypto key zeroize
crypto key generate rsa modulus 2048
```


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

## Ditching The Serial Cable
Your switch now has a name, an IP address, and telnet or SSH access enabled. You should now be able to get rid of the serial cable and use a program like Putty to SSH or telnet to the switch IP. Then you can continue the guide from that CLI.

## Network Configuration

### Default Route & DNS
If you want your switch OS to be able to reach networks outside of your subnet, as well as resolve hostnames (for instance, to contact NTP servers, or talk to an SNMP server outside your immediate network), we need to give the switch a default route and a DNS server. Replace the IP with the IP of your gateway/router/etc. Assuming you are still at the ```configure terminal``` level:

```
ip dns server-address 192.168.1.1
ip route 0.0.0.0/0 192.168.1.1
```
### NTP
To have the switch keep its time synced via NTP (so its logs make more sense), use the following. If you live in an area that doesn't use Daylight Savings, skip the ```clock summer-time``` command. Use tab completion for the timezone command to see what's available. The IPs in the following example are Google's NTP servers and work well for most cases:
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

## Writing The Config & Tips
Whenever you make changes (like above) they take effect immediately, however they are not saved to onboard flash. So if you reboot the switch, they will be lost. To permanently save them to onboard flash, use the following command:
```
write memory
```


To exit the CLI level you are at, use `exit`. So assuming you are still at the ```configure terminal``` level, type the following to exit back to the ```enable``` level:
```
exit
```
Commands can also be shortened, as long as they are still unique. So to re-enter the configure terminal level, Instead of typing the entirety of ```configure terminal```, the following will also work:
```
conf t
```
There is also tab help and completion. To see all the commands available at the current CLI level, just hit tab. To see the options available for a certain command, just type that command (like ```ip```) then hit tab a couple times.

If you ever need to remove a configuration option you've added, put a ```no``` in front of them at the appropriate CLI level. For example, if you've set the switch name to `beefbox` and have since changed your mind:
```
no hostname beefbox
```

## Useful Commands

Show chassis information like fan and temperature status:
```
show chassis
```
Show the system log, which is handy for tracking down port flaps or other events:
```
show log
```
Clear the system log if it's cluttered and you want to empty old entries:
```
clear log
```
Give a port a friendly name:
```
interface ethernet 1/1/1
port-name freenas
show interfaces brief ethernet 1/1/1
exit
```

Show a table of all interfaces:
```
show interface brief
```
Show one interface in detail:
```
show interfaces ethernet 1/1/1
#Also works for virtual interfaces:
show interfaces ve 1
```
Show statistics like bandwidth and utilization for a port:
```
show statistics ethernet 1/1/1
```
Clear all stats in the switch to start the statistics and interface counters from 0 again:
```
clear statistics
```
Show the running configuration:
```
show run
```
## Advanced Configurations
### PoE
If you have a PoE enabled model you'll need to enable PoE power on the ports you have PoE devices plugged into. For example, let's say you've plugged a PoE camera into port 5. Assuming you're at the `configure terminal` CLI level, lets enable PoE power to turn it on:
```
interface ethernet 1/1/5
inline power
```
That's it, you should see the device power on now. You can monitor PoE status such as power level, which ports are enabled, and how much power they're using by running the following:
```
show inline power
#or show even more details:
show inline power detail
#or show details for just one port:
show inline power 1/1/5
```


## SFP/Optics Information
Brocade does not restrict the use of optics by manufacturer, they'll take anything given it's the right protocol. However optical monitoring information is disabled unless it sees Brocade or Foundry optics.

So if you want to see information like this :

```
telnet@Route2(config)#sh optic 1/3/1
 Port  Temperature   Tx Power     Rx Power       Tx Bias Current
+----+-----------+--------------+--------------+---------------+
5       32.7460 C  -002.6688 dBm -002.8091 dBm    5.472 mA
        Normal      Normal        Normal         Normal
```
You'll need to pick up some official Brocade or Foundry optics on ebay, or buy some flashed optics from FiberStore.  