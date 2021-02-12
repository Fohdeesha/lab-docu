@echo off
echo Revert Full Size B0 H810 To Dell Firmware
pause If your card is not an B0 H810, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 H810.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 810B0STK.sbr
pause If no errors above, you may press any key then reboot