@echo off
echo h710 D1 mini Revision Preparation
pause If your card is not h710 D1 mini, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0
pause Press any key to continue
echo running second cleaning to be sure
megarec -cleanflash 0
pause Press any key to write IT Mode SBR
megarec -writesbr 0 D1Mod.sbr
pause If no errors above, you may press any key then reboot
