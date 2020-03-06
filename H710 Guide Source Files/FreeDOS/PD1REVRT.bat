@echo off
echo Revert D1 H710P mini To Dell Firmware
pause If your card is not D1 P mini, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0 710PSTK.rom
echo Flashing Dell Image Complete
pause Press any key to write Dell SBR (required)
megarec -writesbr 0 PD1STOCK.sbr
pause If no errors above, you may press any key then reboot
