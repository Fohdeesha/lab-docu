# Accessing Linux
This only applies to the newer architecture switches - the ICX6450, ICX6650, and the ICX7xxx series. The older models (GS, FCX, ICX6610, etc) run a proprietary bootloader and OS, so there is no Linux to access.

Some time after the ICX6610 design, they moved to using the popular u-boot bootloader, which then boots good old linux. The linux system then launches a monolithic FastIron binary. This is of course all transparent, all you will see is the normal FastIron interface - except now it's an application being ran on top of linux.

After some poking around in a dump of a running switches RAM, I stumbled upon the INIT script used to start the linux system. Of interest were these statements (trimmed for brevity):

```
#If noautostart is set in uboot, stop at linux prompt
cat /proc/cmdline |grep "noautostart" 1>/dev/null 2>&1 if [ $? -eq 0 ] then START_FI_AUTO=0

#If enabletelnet is set in uboot, then start telnet daemon
cat /proc/cmdline |grep "enabletelnet" 1>/dev/null 2>&1 if [ $? -eq 0 ] then
/usr/sbin/telnetd -l /bin/sh -f /etc/welcome
```

It's looking at the boot arguments passed by the bootloader for a `noautostart` or `enabletelnet` string. If it sees the `noautostart` string, it does not start the Fastiron app and drops you right into linux. If it sees the `enabletelnet` string, it starts a hidden telnet server with raw access to linux. These are our two ways in.

## Method 1: Disable FastIron AutoStart

This method tells the switch not to boot the Fastiron app, and drops you right into linux. You will only have serial access to the switch, the usual networking software will NOT be loaded.

Get into the bootloader like usual (hit `b` during boot), then run the following:
```
setenv extra_bootargs noautostart
boot
```
>note: this only sets this argument once and does not save it. So on subsequent reboots or power-ons, the string will be gone and it will boot normally.


 It will begin booting like normal, but eventually you'll land in a shell:
```
BusyBox v1.18.3 (2013-11-22 14:33:50 IST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

/ #
```
**Note:** If your ICX7xxx series switch asks for a root password, it's `wYbRaMWrYIJgg`

 Press tab twice and it'll show you all possible commands (just typing `help` will not show you all commands), there's a couple hundred, most of the utilities you'd expect including `wget`, `curl`, `vi`, and `ftpput`.

Check out the filesystem:

```
/ # ls
FastIron.xz  etc          home         mnt          sbin         tmp
bin          fast_iron    init         proc         sys          usr
dev          ftp          lib          root         tftpboot     var
/ #
```
In the `fast_iron` directory, you'll find the startup configs you save from the normal switch CLI, as well as some logs. To reboot back to normal FastIron, just issue the `reboot` command.

## Method 2: Enable Hidden Telnet Server

This method tells the switch to start a hidden telnet server attached to the underlying Linux OS. It's only accessible via the management port (this is the only networking port Linux can see as it's off the management CPU, not the switching ASIC). The IP for Linux (where the telnet server will be) is set by a bootloader variable.

The upside of this method is that FastIron will still start, so you'll still have a normal functioning switch. There will just be an (unsecure) telnet daemon running on the management port with raw access to the underlying Linux OS - great for live debugging.

Get into the bootloader like usual (hit `b` during boot), then run the following. For the IP, choose an IP that is not in use anywhere else, especially on the switch. If you have a management IP assigned to the switch, this is separate from that and needs to be a different IP:
```
setenv extra_bootargs enabletelnet
setenv ipaddr 192.168.1.57
boot
```
>note: this only sets this argument once and does not save it. So on subsequent reboots or power-ons, the string will be gone and it will boot normally.

The switch will boot as normal, except now the underlying Linux OS has launched a telnet server at the specified IP, and is accessible via the management port only. When you telnet to it (default telnet port), you will have a full Linux CLI:

```
Welcome to FastIron Board's Telnet session
           _  _
          | ||_|
          | | _ ____  _   _  _  _
          | || |  _ \| | | |\ \/ /
          | || | | | | |_| |/    \
          |_||_|_| |_|\____|\_/\_/

          On Brocade's FastIron board


BusyBox v1.18.3 (2013-11-22 14:33:50 IST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

/ # ls
FastIron   etc        home       mnt        sbin       tmp
bin        fast_iron  init       proc       sys        usr
dev        ftp        lib        root       tftpboot   var
/ #
```

**Note:** If your ICX7xxx series switch asks for a root linux password, try `fibranne` or `wYbRaMWrYIJgg` - if neither of those work for some reason, you can append `single` to your bootargs and it will boot linux in single-user mode which will not ask for a password, eg `setenv extra_bootargs noautostart single`