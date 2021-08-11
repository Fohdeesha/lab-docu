# ICX7xxx Licensing

For the ICX7xxx series, no actual licenses are required. Around the release of FastIron v8060, they enabled honor-based licensing. This means you can simply install and unlock a valid license yourself with the `license install` command. That's it! Congratulations Ruckus, you did it. Just be sure to be running the recent firmware from this guide first.  

>If you are using these switches in a commercial environment, or to otherwise turn a profit, you should *really* purchase a license entitlement from Ruckus - it's only fair

## License The ICX7150-C12P
Run the following to unlock all the features of the ICX7150-C12P:

```
enable
license install perpetual 1 2x10gr
conf t
int e 1/3/1 to 1/3/2
no speed-duplex 1000-full
exit
write mem
exit
reload
```
The switch will reload, and the new features will be fully functional.

## License The ICX7250
Run the following to unlock all the features of the ICX7250:

```
enable
license install perpetual 1 8x10g
license install perpetual 1 l3-prem
conf t
interface ethernet 1/2/1 to 1/2/8
no speed-duplex 1000-full
exit
write mem
exit
reload
```
The switch will reload, and the new features will be fully functional.

## License The ICX7450
Run the following to unlock all the features of the ICX7450:

```
enable
license install perpetual 1 l3-prem
license install perpetual 1 macsec
write mem
reload
```
The switch will reload, and the new features will be fully functional.

## License The ICX7750
Run the following to unlock all the features of the ICX7750:

```
enable
license install perpetual 1 l3-prem
write mem
reload
```
The switch will reload, and the new features will be fully functional.

## Other ICX7xxx Models
If you have a 7-series switch not listed here and would like to license it, ensure you're at the `enable` CLI level. Then just type in `license install perpetual 1` and hit tab a few times, it will show all the auto-complete options (license options). Just run the license install command above for every one of them, same as for the switches above. Then just `write mem` and `reload`
