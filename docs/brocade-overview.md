![barry](store/barrysmallflip.png) Welcome to Beef Town ![barry](store/barrysmall.png)

## Master ZIP (Firmware, Manuals, Licenses)
Download the ZIP below which contains all the firmware, licenses, and documentation you'll need for the guides on this site.

[```Brocade Master ZIP```](store/brocade-06-22-2023.zip)  
```ICX6xxx SW version: 08030u```  
```ICX7xxx SW version: 08095k```  
```ZIP Updated: 06-22-2023```  
```MD5: 656ac17bc882b567ab385dc742544034```  

Inside this archive you'll see a folder titled `brocade-(date)` - extract this entire folder somewhere on your PC. Then, you need to get a TFTP server up and running temporarily, serving files out of the `TFTP-Content` subfolder. If you're already familiar with how to do this, point your TFTP server at `TFTP-Content` so it's the root folder, then navigate to your switch model under the **Brocade Setup** menu on the left. If you need some pointers getting TFTP running, continue below.

### Setting up TFTP (Windows)
For a simple temporary TFTP server under Windows, I highly recommend Tftpd64. In the brocade ZIP above, a portable copy that requires no installation is included under the `Tftpd64` folder - just launch the `tftpd64.exe` executable inside - it should already be configured with the appropriate settings, and set to serve the correct folder (`..\TFTP-Content\`).

That's it! You're now serving all the files via TFTP so your switch can grab them. When you're done with the guide, just exit the program. If you don't want to trust an executable from a random site, you can also just [download Tftpd64](https://pjo2.github.io/tftpd64/) from the official site and configure it yourself (granted, if you `diff` it with mine, you'll see they're identical).
>**Note:** On any modern version of Windows, the Windows Firewall will probably block TFTP connections by default - if you get TFTP issues and the switch can't download files, just completely disable the windows firewall temporarily 

### Setting up TFTP (Linux)
You may already have a TFTP daemon you prefer on *nix but if not, I've found `tftpd-hpa` to typically be the simplest. The following assumes you're on a Debian-ish distro or similar. If you're on something `yum` or `pkg` based, I'm sure you can figure out the equivalent commands. First, install `tftpd-hpa`:
```
apt install tftpd-hpa
```
By default it will enable it as a service so it will start every boot, let's disable that so it will only run when you manually start it:
```
systemctl disable tftpd-hpa
```
Now you need to edit the config located at `/etc/default/tftpd-hpa` to match the below exactly, but of course changing the directory path to wherever you happened to extract the brocade ZIP:
```
# /etc/default/tftpd-hpa

TFTP_USERNAME="nobody"
TFTP_DIRECTORY="/home/jon/brocade-05-24-2022/TFTP-Content"
TFTP_ADDRESS="0.0.0.0:69"
TFTP_OPTIONS="--secure -vvvv"
```
>Note: tftpd-hpa does *not* like spaces in the directory path  

Save the config file and now reboot the matt daemon:

```
systemctl restart tftpd-hpa
```
That's it, you're now serving all the guide files and your switches can grab them. If you have trouble, you should see a good amount of related info from `tail -F /var/log/syslog` - see if it's complaining about file permissions or similar. When you're finished with the guide, you can stop the daemon and it won't start again until you tell it to, or just uninstall it completely:
```
systemctl stop tftpd-hpa
##optionally remove it completely
apt remove tftpd-hpa
```

### Setting up TFTP (macOS)
Fortunately macOS has a built in TFTP server, all you have to do is enable it. Lets first temporarily disable the Firewall: **System Settings** -> **Network** -> **Firewall**.

Load the TFTP service:
```
sudo launchctl load -F /System/Library/LaunchDaemons/tftp.plist
```
Start the service:
```
sudo launchctl start com.apple.tftpd
```
By default, the TFTP directory is at `/private/tftpboot`. Be sure to grant permissions:
```
sudo chmod 777 /private/tftpboot
```
Move the contents of `TFTP-Content` to the `/private/tftpboot` directory.

Once complete, you can stop and disable the service:
```
sudo launchctl stop com.apple.tftpd
sudo launchctl unload -F /System/Library/LaunchDaemons/tftp.plist

##optionally to be double sure TFTP is no longer running, ensure theres no output from the following:
sudo lsof -i :69
```

Finally, be sure to re-enable the firewall: **System Settings** -> **Network** -> **Firewall**


## Begin
You're now serving the required files over TFTP. Just navigate to the left-hand menu, underneath the **Brocade Setup** section - click on the section for your switch model and begin.
