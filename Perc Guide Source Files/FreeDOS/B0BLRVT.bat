@echo off
echo Revert B0 H710 Mini For Blades To Dell Firmware
pause If your card is not B0 Mini for Blades, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 710BLDSK.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 B0BLSTK.sbr
pause If no errors above, you may press any key then reboot
