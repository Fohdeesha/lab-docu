@echo off
echo Revert H310 Full Size To Dell Firmware
pause If your card is not an H310 Full hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 H310FLFW.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 H310FLSK.sbr
pause If no errors above, you may press any key then reboot
