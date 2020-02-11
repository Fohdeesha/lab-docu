@echo off
echo Revert B0 H710P To Dell Firmware
pause If your card is not D1 P, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 710PSTK.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 PB0STOCK.sbr
pause If no errors above, you may press any key then reboot
