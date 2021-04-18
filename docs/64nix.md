# Assigning tons of /64s to *nix machines (telehouse-NY)

### Get a /125 transit net for the connection to the server:
`2620:57:e000:e::10/125`

### give the first IP in transit subnet to the ICX7750:
```
enable
conf t
int ve 2000
ipv6 addr 2620:57:e000:e::11/125
ipv6 nd suppress-ra
```
The next IP after this, `2620:57:e000:e::12`, will be given to the server.

### Choose a /56 to give to the server:
`2620:57:e000:400::/56`

### Tell the ICX7750 the entire /56 is accessible over previous transit link:
```
enable
conf t
ipv6 route 2620:57:e000:400::/56 2620:57:e000:e::12
```

The destination is your chosen `2620:57:e000:400::/56` subnet, the next-hop is the server end of the transit subnet, `2620:57:e000:e::12`

### Give the server its own address in the transit subnet:
**Note**: this should be one address up from what the router (ICX7750) has.

```
nano /etc/network/interfaces

auto eth1
iface eth1 inet6 static
        address 2620:57:e000:e::12
        netmask 125
        gateway 2620:57:e000:e::11
        autoconf 0
```

### Assign an entire /56 to a linux server

Run the below command to assign the entire /56 to the server's loopback interface:
**Note:** this uses Linux's [AnyIP](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=ab79ad14a2d51e95f0ac3cef7cd116a57089ba82) feature:

`ip -6 route add local 2620:57:e000:400::/56 dev lo`

Your server will now respond to any address in the entire /56 - however most applications and daemons will complain when you try to bind them to an address in this block, since they're not *technically* assigned to any real linux interface. To solve this, enable the `net.ipv6.ip_nonlocal_bind` sysctl:

`sysctl -w net.ipv6.ip_nonlocal_bind=1`

Applications can now bind to any IP address in the /56. To make this permanent so it persists reboots, you need to edit `/etc/sysctl.conf`:

```nano /etc/sysctl.conf```

Add the following line and then save it:

`net.ipv6.ip_nonlocal_bind = 1`

The `ip -6 route` command we ran earlier to assign the /56 will also disappear on reboot, to make it persist reboots, add it as a `pre-up` command to our network interfaces file, under your transit subnet interface:

```
nano /etc/network/interfaces

auto eth1
iface eth1 inet6 static
        address 2620:57:e000:e::12
        netmask 125
        gateway 2620:57:e000:e::11
        autoconf 0
        pre-up ip -6 route add local 2620:57:e000:400::/56 dev lo
```

### Finding all the /64s to bind to
If your application needs to be bound to each /64, stick our `2620:57:e000:400::/56` subnet into [this calculator](https://subnettingpractice.com/ipv6_subnetting.html), and choose /64s from the dropdown then press calculate:

![calculator](../../data/other/brocade/64nix-1.png)

Then simply bind each of your application instances to the first address in each /64 - For instance:

```
Instance 1 - 2620:57:e000:400::1/64
Instance 2 - 2620:57:e000:401::1/64
Instance 3 - 2620:57:e000:402::1/64
etc etc etc
```



