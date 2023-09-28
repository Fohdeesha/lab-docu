@echo off
echo Revert D1 H710P Mini For Blades To Dell Firmware
pause If your card is not D1 Mini for Blades, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 710PBLSK.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 D1PBLSTK.sbr
pause If no errors above, you may press any key then reboot
