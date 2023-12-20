
This page assumes you've already followed the update/config guide for your specific switch model. You should now have an updated switch configured with an IP address, and one of the **regular switch ports** (not the dedicated management port) plugged into your network to access said IP. It's also assumed you're at the `configure terminal` CLI level.

Nothing here is necessary for your switch to continue operating as a "dumb" unmanaged switch, but the steps here are highly recommended nonetheless to set up basic security, management, and advanced features you might find useful.

>**Note:** This page is for the ICX6xxx series, anything running FastIron v8030

## Key Generation, Security, & Web UI
Before we can do things like SSH to the switch or access the web UI, we need to do a couple things. First, tell it to generate an RSA keypair - this is the first step to enabling SSH access:
```
crypto key zeroize
crypto key generate rsa modulus 2048
```

To secure the switch, we need to create an account - "root" can be any username string you wish:
```
username root password yourpasshere
```
We also need to tell it to use our new local user account(s) to authorize attempts to log in or access the web UI:
```
aaa authentication login default local
aaa authentication web default local
```

We should also disable telnet as it's quite insecure. If the switch is located outside your home, password protect the serial console port as well:

```
no telnet server
#optional: password protect the serial port:
enable aaa console
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

??? info "Using custom HTTPS certificates"
    If you'd like the switch web UI to use your own custom HTTPS certificates, that's possible as well. It's important to remember that the switch [doesn't support PKCS #8](https://community.ruckuswireless.com/t5/Switches/Can-t-import-SSL-certificates-quot-Could-not-parse-the-PEM/m-p/18554/highlight/true#M686) and this isn't actually [documented anywhere](https://docs.ruckuswireless.com/fastiron/08.0.60/fastiron-08060-securityguide/GUID-E83AC70A-9F89-4209-B6C4-ED5725D4F487.html). You must create certificates using the PKCS #1 format. If you try to use PKCS #8 (the default for openssl), you'll get certificate parsing errors.

    The ICX6xxx series doesn't support keys larger than 2048 bits. This example assumes you want to create your own CA and sign with it.

    Create a new Root CA:
    ```
    pass='{{ pass }}' \
    name='{{ name }}' \
    openssl req \
        -newkey rsa:4096 \
        -sha512 \
        -passin pass:"${pass}" \
        -x509 \
        -nodes \
        -keyout "$name"Root.pem \
        -new \
        -out "$name"Root.crt \
        -subj "/CN="$name" Root CA" \
        -days 3650
    ```

    Generate a key in PKCS #1 format using the `-traditional` flag (see [openssl-genrsa](https://www.openssl.org/docs/manmaster/man1/openssl-genrsa.html) for more details):
    ```
    openssl genrsa -traditional -out keyfile 2048
    ```

    Create the certificate signing request:
    ```
    name='{{ name }}' \
    C='{{ country }}' \
    ST='{{ state/province }}' \
    openssl req \
        -new \
        -sha512 \
        -key keyfile \
        -subj "/C="$C"/ST="$ST"/O="$name" Network, Inc./CN=sw1.home.arpa" \
        -out certsignreq.csr \
        -reqexts SAN \
        -extensions SAN \
        -config <(cat /etc/ssl/openssl.cnf ; printf "[SAN]\nsubjectAltName=DNS:%s" "sw1.home.arpa")
    ```

    Sign the certificate request:
    ```
    name='{{ name }}' \
    openssl x509 \
        -req \
        -in certsignreq.csr \
        -CA "$name"Root.crt \
        -CAkey "$name"Root.pem \
        -CAcreateserial \
        -out certfile \
        -days 3650 \
        -sha512 \
        -extensions v3_ext \
        -extensions SAN \
        -extfile <(cat /etc/ssl/openssl.cnf ; printf "[SAN]\nsubjectAltName=DNS:%s" "sw1.home.arpa")
    ```

    Install your custom certificate on the switch via TFTP:
    ```
    ip ssl cert-key-size 2048
    ip ssl certificate-data-file tftp 192.168.1.51 certfile
    ip ssl private-key-file tftp 192.168.1.51 keyfile
    web-management https
    ```
    That's it! the web UI should now use your cert when loaded via HTTPS

### Optional: Key Based SSH Access
If you have followed the above to set up authentication, and also wish to disable password-based SSH login and set up a key pair instead, follow this section. If not, skip it. Enable key login, and disable password login:
```
ip ssh key-authentication yes
ip ssh password-authentication no
ip ssh interactive-authentication no
```
Now we have to generate our key pair with [puttygen](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) on windows or ```ssh-keygen -t rsa``` on linux. The default settings of RSA @ 2048 bits works without issue. Generate the pair and save out both the public and private key.
>The ICX6xxx series do *not* support 4096 bit keys - when using `ssh-keygen` or `puttygen` etc, you must create 2048 bit keys.

For the switch to accept your public key file it should start with:
```
---- BEGIN SSH2 PUBLIC KEY ----
```
and end with:
```
---- END SSH2 PUBLIC KEY ----
```
Add those manually if it doesn't.  

Copy the public key file to your TFTP server. Then use the following command to import it into your switch:
```
ip ssh pub-key-file tftp 192.168.1.8 public.key
```
You shouldn't need to be told basic key management if you're following this section, but just in case - copy your private key to the proper location on the *nix machine you'll be SSH'ing from, or if you're on windows, load it using [pageant](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html). Now when you SSH to the switch, it will authenticate using your private key.

On linux and macOS you'll want to add these lines to your ```~\.ssh\config``` file. IdentitiesOnly is to prevent ssh-agent from offering any other bigger (> 2048) keys first that will terminate the connection negotiation early. IdentityFile will use only this key for this connection. KexAlgorithms sets a key exchange algorithm that the older ICX6xxx series accepts.  The PubkeyAcceptedKeyTypes must be set as the algorithm is off by default more frequently, along with the HostKeyAlgorithm, to match what the ICX6xxx can offer:

```
Host <switch-ip>
   IdentitiesOnly yes
   IdentityFile ~/.ssh/private_key
   KexAlgorithms +diffie-hellman-group1-sha1
   PubkeyAcceptedKeyTypes=+ssh-rsa
   HostKeyAlgorithms=+ssh-rsa
```


## Name & Ditching The Serial Cable
Now that you have access and authentication configured, it only makes sense to christen it with a name of your choice:
```
hostname beefchunk
```
Your switch now has a name, an IP address, and telnet or SSH access enabled. You should now be able to get rid of the serial cable and use a program like Putty to SSH or telnet to the switch IP. Then you can continue the guide from that CLI.

## Network Configuration

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
If you have a PoE enabled model you'll need to enable power on the ports you have PoE devices plugged into. For example, let's say you've plugged a PoE camera into port 5. Lets enable PoE power to turn it on:
```
interface ethernet 1/1/5
inline power
exit
```
That's it, now you should see the device power on. You can monitor PoE status such as power level, which ports are enabled, and how much power they're using by running the following:
```
show inline power
#or show even more details:
show inline power detail
#or show details for just one port:
show inline power 1/1/5
```
ICX6xxx switches also have something called `legacy inline power` on by default, which detects and powers very old legacy PoE devices that rely on a specific resistance for detection. Devices like this are exceptionally rare and this can occasionally erroneously detect regular devices as legacy PoE units, so let's disable it:
```
stack unit 1
no legacy-inline-power
exit
```
If you have a switch that does not support stacking like the ICX6430, you'll need to run `no legacy-inline-power` at the global configure terminal level instead. If you have a switch stack built, don't forget to run the above for stack unit 2 as well (or however many units you have).

### Link Aggregation (802.3ad LACP)
If you'd like to configure an LACP bond on the switch to aggregate 2 or more ports to a server for example, it's pretty easy under FastIron. First you need to meet some basic criteria before creating the bond:

* all switch ports in the bond must be the same port type / speed
* all switch ports being added to the bond cannot have an existing configuration on them (no IPs set etc)
* all switch ports must have the same VLAN membership

Now create the dynamic (802.3ad) lag, giving it a name of your choice:
```
lag freeNAS dynamic
```
Now you should be at the LAG configuration CLI level for this new LAG. Here's the important part, tell it what ports you want to be in the bond. In this example I'll add two of the 10gbE ports on an ICX6610. If you want more than two ports in the bond, add them to the `ports` command after the first two:
```
ports ethernet 1/3/1 ethernet 1/3/2
```
Now we need to specify the primary port. This is the port member of the bond that will act as the single port that controls all bond members in your config. For instance, if your primary port is 1/3/1, then to add your LAG to a VLAN, you'd use `tag int eth 1/3/1` - the switch will then do the same for all the other bond members. Same with any port config, if you want to configure the LACP group, you use the primary port. This is almost always the first port in the LAG, but typically it doesn't matter:
```
primary-port 1/3/8
```
Lastly, we need to deploy the new LAG you've created and only needs to be ran once to enable it:
```
deploy
exit
write mem
```
Assuming the other end (server, other switch, etc) has been configured for LACP correctly, you should see them negotiate and enable all links. You can view the status of bonds using the following:
```
show lag
```
This is what a correctly negotiated LACP link will look like, the most important column typically being the last, with `Ope` meaning operational. If you have something else here like inactive, you have a configuration issue somewhere:
```
Deployment: HW Trunk ID 1
Port       Link    State   Dupl Speed Trunk Tag Pvid Pri MAC             Name
1/3/7      Up      Forward Full 10G   2     Yes N/A  0   cc4e.24b8.d9d0  XEN-01-1
2/3/7      Up      Forward Full 10G   2     Yes N/A  0   cc4e.24b8.d9d0  XEN-01-2

Port       [Sys P] [Port P] [ Key ] [Act][Tio][Agg][Syn][Col][Dis][Def][Exp][Ope]
1/3/7           1        1   20002   Yes   L   Agg  Syn  Col  Dis  No   No   Ope
2/3/7           1        1   20002   Yes   L   Agg  Syn  Col  Dis  No   No   Ope
```
If you ever need to go back and edit the LAG properties, run the `lag` command again with the correct name:
```
lag freeNAS dynamic
```
Then if you want to add more ports to an existing bond for example, re-run the `ports` command, appending the new ports (don't forget to keep the existing ports in the command, or they will be removed). So, to add two more ports to our example bond:
```
ports ethernet 1/3/1 ethernet 1/3/2 ethernet 1/3/3 ethernet 1/3/4
deploy
exit
write mem
```
Whatever ports you add must have the same VLAN membership as the existing LAG, or you'll get an error about `tag type` not matching.

### VLANs
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
