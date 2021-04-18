# Hidden Brocade Dev Stuff
This is a collection of hidden tools, modes, and commands buried inside Brocade FastIron products.

## Hidden Commands

All Fastiron platforms have hidden CLI commands, with the biggest one being `dm`. The ```dm``` command is available (but hidden) for diagnostic use. Type ```dm``` and hit tab to see available options, like the below for example:
```
FCX3(config)#dm
  HEX                           Number
  802-1w                        show 802-1w internal information
  aaa-acct-session
  acl_tcam                      command for tcam manager
  aging-loop                    Turn off/on system aging loop
  allled                        Toggle -On/Off All LED
  alt-diag                      Test only, off/on
  app_vlan_debug                App VLAN table (shadow)
  auq-resync                    Sync Auq for a device
  auq-status                    show auq status
  badaddr                       Test only, will reboot
  blink                         Show gig link changes due to PHY blink
  buffer                        DMA CPU buffer related
  cancel-flash-timeout          set flash timeout to default
  clear_boot_count              Clear crash dump
  cpld-reg-dump                 Dump the CPLD register
--More--
```
There's hundreds of options. These are low level debug commands, so if you don't know what they do you can easily break your switch.

**Note:** There's around ~700 commands that are completely hidden, they will not even show up using tab autocomplete like the above `dm` commands do. [Click here](https://fohdeesha.com/data/other/brocade/FastIron-Hidden.txt) for a full list of these commands. Many of these might delete your config, your licenses, lock the switch up, etc.

## Hidden OS Console
This only works over a serial connection. It will not work over telnet/ssh/etc. Connect to a FastIron device over serial, and once it's fully booted and running, press ```ctrl+y```, let go, then press ```m```. Then hit enter. It should drop you down to the OS console:
```
OS>
```

> Note: This is mainly for the PowerPC architecture switches (FCX, ICX6610). The ARM switches (ICX6450, ICX7xxx) still have this hidden console available, but it only has ~5 commands. For a proper low level CLI on ARM switches, use the *Accessing Linux* section.

Press the question mark on your keyboard for a list of all available options:

```
OS>
clear            Clear internal table
console          Attach console to a process
copy             Copy file
daw              Set data address watchpoint
dbg              Toggle debug mode
dq               Display memory in 64-bit quad word
dd               Display memory in 32-bit double word
del              Delete flash file
dir              List flash files
----trimmed----
```
You'll note the options are nearly identical to what's available in the bootloader, except while the switch is fully booted and running. There's a couple extra options here too that aren't available in just the bootloader. To exit this mode, just hit ```ctrl+z```.

## Hidden Bootloader Modes

All the newer switches that run linux (ICX6450, ICX6650, ICX7xxx) use u-boot as the bootloader. However it runs in a very locked down mode with only a few commands available. Thankfully after contacting Arris (who own Ruckus, who now owns Brocade), they published their u-boot [source code](https://sourceforge.net/arris/wiki/Projects/).

From analyzing their u-boot source, we are able to find some hidden routines that will force the switch to boot into fully unlocked u-boot. To do so, run the following in the bootloader:

#### ICX6430 & ICX6450
```
manufacturing_diag enable
reset
## it will reboot into fully unlocked u-boot
## be sure to stop it in the bootloader (smash b)

## set it back to normal
manufacturing_diag disable
reset
```
#### ICX6650 & ICX7150 & ICX7250 & ICX7450 & ICX7750
```
diagmode enable
reset
## it will reboot into fully unlocked u-boot
## be sure to stop it in the bootloader (smash b)

## set it back to normal
diagmode disable
reset
```

#### ICX7650 & ICX7850
```
setenv diag_mode_on 1
saveenv
reset
## it will reboot into fully unlocked u-boot
## be sure to stop it in the bootloader (smash b)

## set it back to normal
setenv diag_mode_on
saveenv
reset
```

#### ICX7550
Unknown, but most likely one of the above - just try each one until the command takes.