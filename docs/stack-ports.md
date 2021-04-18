
# ICX6610 Stack Ports
## Upgrading The Software
Thankfully in FastIron release 08.0.10 and up, Brocade quietly removed the "stack-only" restriction on the rear 40gbE ports.

This means the first step is to update to the latest release. This link has instructions and a ZIP of the latest images - [FastIron Upgrade & Config Guide](https://fohdeesha.com/docs/fcx/).

However you're not done yet!

## Removing Stack Configuration
There are going to be some stacking commands automatically put in your configuration, so we need to remove them. To see what you need to remove, show the config:
```
show run
```
Towards the top you should see a stacking related block similar to the below:
```
stack unit 1
  module 1 icx6610-48-port-management-module
  module 2 icx6610-qsfp-10-port-160g-module
  module 3 icx6610-8-port-10g-dual-mode-module
  stack-trunk 1/2/1 to 1/2/2
  stack-trunk 1/2/6 to 1/2/7
```

We need to remove the two ```stack-trunk``` lines. They may be slightly different on your end, so substitute whatever was in your config. Notice that they are under the ```stack unit 1``` level, so you need to enter that CLI level first:
```
enable
conf t
stack unit 1
no stack-trunk 1/2/1 to 1/2/2
no stack-trunk 1/2/6 to 1/2/7
exit
write mem
```
Now show the config:
```
show run
```
There should no longer be any ```stack-trunk``` commands, or any commands referencing stack ports. That's pretty much it! You can now use the ports on the rear as normal. However, see the next section on how they are laid out.

## Port Layout
Keep in mind two of the rear ports are 4x 10gbE breakout only, and the remaining two are 40gbE only. The two 40gbE-only ports will NOT run at 10gbE (for instance with a QSFP to SFP+ adapter), so use these two for 40gbE links only. The extremely professional diagram below shows the port config and their associated port numbers in the OS:

![Port Diagram](https://fohdeesha.com/data/other/icx-ports.png)

