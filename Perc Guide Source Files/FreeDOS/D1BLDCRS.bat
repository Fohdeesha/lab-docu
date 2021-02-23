@echo off
echo H710 D1 Mini BLADE SERVER Preparation
pause If your card is not H710 D1 Mini in a BLADE, hit ctrl+c now!! Otherwise hit enter
pause Press any key to clean adapter flash
megarec -cleanflash 0
pause Press any key to continue
echo running second cleaning to be sure
megarec -cleanflash 0
pause Press any key to write IT Mode SBR
megarec -writesbr 0 D1BLDMOD.sbr
pause If no errors above, you may press any key then reboot
