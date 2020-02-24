@echo off
megacli.exe -adpallinfo -a0 -NoLog | grep -E "(Chip|Product|SAS Address)"