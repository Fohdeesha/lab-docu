@echo off
echo H810 Full Size D1 Revision Preparation
pause If your card is not a full size H810 D1, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0
pause Press any key to continue
echo running second cleaning to be sure
megarec -cleanflash 0
pause Press any key to write IT Mode SBR
megarec -writesbr 0 810D1MD.sbr
pause If no errors above, you may press any key then reboot
