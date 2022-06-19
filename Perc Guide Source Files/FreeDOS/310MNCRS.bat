@echo off
echo H310 Mini Preparation
pause If your card is not an H310 MINI, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0
pause Press any key to continue
echo running second cleaning to be sure
megarec -cleanflash 0
pause Press any key to write IT Mode SBR
megarec -writesbr 0 H310MNMD.sbr
pause If no errors above, you may press any key then reboot
