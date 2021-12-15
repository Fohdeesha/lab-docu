
This page assumes you've already followed the update/config guide for your specific switch model. You should now have an updated switch configured with an IP address, and one of the **regular switch ports** (not the dedicated management port) plugged into your network to access said IP. It's also assumed you're at the `configure terminal` CLI level.

Nothing here is necessary for your switch to continue operating as a "dumb" unmanaged switch, but the steps here are highly recommended nonetheless to set up basic security, management, and advanced features you might find useful.

>**Note:** This page is for the ICX7xxx series, anything running FastIron v8080 and above

## Key Generation, Security, & Web UI

Before we can do things like SSH to the switch or access the web UI, we need to do a couple things. First, tell it to generate an RSA keypair - this is the first step to enabling SSH access:
```
crypto key zeroize
crypto key generate rsa modulus 2048
```
By default, the switch has the `super` user that you set a password for previously. We need to configure the switch to use this account to authenticate logins and web UI access:
```
aaa authentication login default local
aaa authentication web default local
```
We should also disable telnet as it's quite insecure:

```
no telnet server
```
If you wanted to use the web UI (I don't recommend using it, really), you can now log into it using the credentials you created above. By default, the web server is http only. If you want to disable the web server, just run:
```
no web-management http
```
If you would like to then turn on the web server over  `https`  instead, run the following (skip this if you just want the webserver off totally):
```
crypto-ssl certificate generate
web-management https
```
Once you get an  `ssl-certificate creation is successful`  message in the console, you'll be able to access the web UI via  `https`.

### Optional: Changing the default user account
If you don't like the default  `super`  username that comes with the switch, you can remove it and create your own user account instead. Just replace  `customname`  and  `yourpasshere`  with your own values:

```
username customname password yourpasshere
no username super
```
For example, to create a user named  `root`  instead:

```
username root password secret123
no username super
```
You should now use this new user for all login duties.

### Optional: Key Based SSH Access
If you have followed the above to set up authentication, and also wish to disable password-based SSH login and set up a key pair instead, follow this section. If not, skip it. Enable key login, and disable password login:
```
ip ssh key-authentication yes
ip ssh password-authentication no
ip ssh interactive-authentication no
```
Now we have to generate our key pair with [puttygen](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) on windows or ```ssh-keygen -t rsa``` on linux. The default settings of RSA @ 2048 bits works without issue. Generate the pair and save out both the public and private key.  

Copy the public key file to your TFTP server. Then use the following command to import it into your switch:
```
ip ssh pub-key-file tftp 192.168.1.8 public.key
```
You shouldn't need to be told basic key management if you're following this section, but just in case - copy your private key to the proper location on the *nix machine you'll be SSH'ing from, or if you're on windows, load it using [pageant](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html). Now when you SSH to the switch, it will authenticate using your private key.

## Name & Ditching The Serial Cable
Now that you have access and authentication configured, it only makes sense to christen it with a name of your choice:
```
hostname beefchunk
```
Your switch now has a name, an IP address, and telnet or SSH access enabled. You should now be able to get rid of the serial cable and use a program like Putty to SSH or telnet to the switch IP. Then you can continue the guide from that CLI.

## Network Configuration

### Disable SmartZone
By default, v8090 and above has SmartZone turned on by default, which phones home to Ruckus among other things for the centralized management features. It's typically a good idea to turn this off completely, unless you have a SmartZone setup you'll be using with the switch:
```
no sz registrar
sz disable
```

### Default Route & DNS
If you want your switch to be able to contact NTP servers for time synchronization, remote SNMP servers, etc, we need to give the switch a default route and a DNS server. Replace the IP with the IP of your gateway/router/etc. Assuming you are still at the ```configure terminal``` level:

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
Now you can monitor the NTP sync status with the following commands:
```
show ntp associations
show ntp status
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

## Advanced Features
This section will outline some of the more advanced configurations you may want to explore. If you use any of them, don't forget to `write mem` when done to actually save your changes. 

### PoE
If you have a PoE enabled model, note that v8080 and above has PoE enabled on all ports by default. This will not harm any non-PoE equipment as no power is delivered until PoE negotiation is performed. If you plug in a PoE device, it should just work. You can monitor PoE status such as power level, which ports are enabled, and how much power they're using by running the following:
```
show inline power
#or show even more details:
show inline power detail
#or show details for just one port:
show inline power 1/1/5
```
If you need to disable PoE on a specific port for some reason, such as to power cycle a remote PoE device, just run the following:
```
interface ethernet 1/1/5
no inline power
exit
```
To turn PoE power back on:
```
interface ethernet 1/1/5
inline power
exit
```

### Link Aggregation (802.3ad LACP)
If you'd like to configure an LACP bond on the switch to aggregate 2 or more ports to a server for example, it's been simplified in v8080 and above. First you need to meet some basic criteria before creating the bond:  

* all switch ports in the bond must be the same port type / speed
* all switch ports being added to the bond cannot have an existing configuration on them (no IPs set etc)
* all switch ports must have the same VLAN membership

Now create the dynamic (802.3ad) lag, giving it a name of your choice:
```
lag FreeNAS dynamic id auto
```
Now you should be at the LAG configuration CLI level for this new LAG. Here's the important part, tell it what ports you want to be in the bond. In this example I'll add two of the 10gbE ports on an ICX7250. If you want more than two ports in the bond, add them to the `ports` command after the first two:
```
ports ethernet 1/2/1 ethernet 1/2/2
exit
```
That's it, it will create a virtual lag interface representing this LACP bond and its member ports, and begin negotiating. Assuming the other end (server, other switch, etc) has been configured for LACP correctly, you should see them negotiate and enable all links. You can view the status of bonds using the following:
```
show lag
```
This is what a correctly negotiated LACP link will look like, the most important column typically being the last, with `Ope` meaning operational. If you have something else here like inactive, you have a configuration issue somewhere:
```
Port       Link    State   Dupl Speed Trunk Tag Pvid Pri MAC             Name
1/3/7      Up      Forward Full 10G   2     Yes N/A  0   cc4e.24b8.d9d0  XEN-01-1
2/3/7      Up      Forward Full 10G   2     Yes N/A  0   cc4e.24b8.d9d0  XEN-01-2

Port       [Sys P] [Port P] [ Key ] [Act][Tio][Agg][Syn][Col][Dis][Def][Exp][Ope]
1/3/7           1        1   20002   Yes   L   Agg  Syn  Col  Dis  No   No   Ope
2/3/7           1        1   20002   Yes   L   Agg  Syn  Col  Dis  No   No   Ope
```
If you ever need to go back and edit the LAG properties, run the `lag` command again with the correct name:
```
lag freeNAS
```
Then if you want to add more ports to an existing bond for example, re-run the `ports` command, appending the new ports (don't forget to keep the existing ports in the command, or they will be removed). So, to add two more ports to our example bond:
```
ports ethernet 1/3/1 ethernet 1/3/2 ethernet 1/3/3 ethernet 1/3/4
exit
write mem
```
Whatever ports you add must have the same VLAN membership as the existing LAG, or you'll get an error about `tag type` not matching.

To manage the lag, such as add it to VLANs, apply an ACL to it, or any other port configuration options, you use the virtual lag interface. You need to know the ID of the lag you want to alter, an easy way to find it is to run: (mind the capital D)
```
show lag | inc Deployed
```
For example I get:
```
SSH@CHUNK#show lag | inc Deployed
=== LAG "6650-RACK6" ID 23 (dynamic Deployed) ===
=== LAG "COMPUTE-01" ID 5 (dynamic Deployed) ===
=== LAG "COMPUTE-02" ID 6 (dynamic Deployed) ===
```
Let's say I want to add the 6650-RACK6 link to VLAN 10, I can see its lag ID is 23. So I would do:
```
vlan 10
tagged lag 23
```
Or if I want to go into the interface config CLI level for the lag interface, and shut it down:
```
interface lag 23
disable
exit
```

### VLANs
Coming Soon

### LLDP
Coming Soon

### Stacking
Coming Soon

### ACLs
Coming Soon

### Inter-VLAN Routing
Coming Soon

### Config Management
Coming Soon

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


## SFP/Optics Information

Brocade does not restrict the use of optics or DACs by manufacturer, they'll take anything given it's the right protocol. However by default, optical monitoring information is disabled unless it sees Brocade or Foundry optics.

So if you want to see information like this:

```
telnet@Route2(config)#show optic 1/3/1
 Port  Temperature   Tx Power     Rx Power       Tx Bias Current
+----+-----------+--------------+--------------+---------------+
5       32.7460 C  -002.6688 dBm -002.8091 dBm    5.472 mA
        Normal      Normal        Normal         Normal
```
You'll need to pick up some official Brocade or Foundry optics. Regardless of what optics you have, you'll first need to enable optic monitoring in general by running  `optical-monitor`  at the  `configure terminal`  level.

### Third Party Optics Unlock

In v8080 FastIron and above, Ruckus added a command that enables optical monitoring details (like the above example) for non-brocade branded optics, so you should no longer need to hunt down Brocade branded optics for detailed SFP+ reporting. It's not guaranteed to work with all optics, but should with most that conform to SFF-8472. Run the following:
```
optical-monitor
optical-monitor non-ruckus-optic-enable
write mem
```