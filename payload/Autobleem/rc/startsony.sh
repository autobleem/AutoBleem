#!/bin/sh
cd /data/AppData/sony/pcsx
export PCSX_ESC_KEY=2
/usr/sony/bin/ui_menu --power-off-enable &> /media/System/Logs/ui_menu.log
sync
reboot
