## placeholder

ICX7xxx series honor info



## License The Switch ICX7150
Run the following to unlock all the features of the switch (10gbE ports, advanced routing, etc). If you are using this switch in a commercial setting or to make money, please purchase a license entitlement from Ruckus.

```
exit
license install perpetual 1 2x10gr
conf t
int e 1/3/1 to 1/3/2
no speed-duplex 1000-full
exit
write mem
exit
reload
```
The switch will reload, and the 10gbE ports and everything else will now be functional.



## License The Switch (ICX7250)
Run the following to unlock all the features of the ICX7250 (10gbE ports, advanced routing, etc). If you are using this switch in a commercial setting or to make money, please purchase a license entitlement from Ruckus.

```
exit
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
The switch will reload, then the 10gbE ports and everything else will now be fully functional.

## License The Switch (ICX7450)
Run the following to unlock all the features of the ICX7450 (advanced routing & macsec encryption). If you are using this switch in a commercial setting or to make money, please purchase a license entitlement from Ruckus.

```
exit
license install perpetual 1 l3-prem
license install perpetual 1 macsec
write mem
reload
```
The switch will reload, then the new features will now be fully functional.