@echo off
echo D1 Revision Preparation
pause If your card is not D1, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0
echo Flash Cleaning Complete
pause Press any key to write IT Mode SBR
megarec -writesbr 0 D1Mod.sbr
pause If no errors above, you may press any key then reboot
