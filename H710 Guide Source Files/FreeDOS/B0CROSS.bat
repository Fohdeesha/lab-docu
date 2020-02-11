@echo off
echo B0 Revision Preparation
pause If your card is not B0, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0
echo Flash Cleaning Complete
pause Press any key to write IT Mode SBR
megarec -writesbr 0 B0Mod.sbr
pause If no errors above, you may press any key then reboot
