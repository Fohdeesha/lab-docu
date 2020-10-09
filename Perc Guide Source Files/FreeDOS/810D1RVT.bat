@echo off
echo Revert Full Size D1 H810 To Dell Firmware
pause If your card is not an H810 D1, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 H810.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 810D1STK.sbr
pause If no errors above, you may press any key then reboot