#!/bin/sh

killall -s KILL sonyapp showLogo ui_menu

mkdir -p /media/System/Logs

export PCSX_ESC_KEY=2
sync
cd /media/cbleemsync
./run.sh
cd /data/AppData/sony/pcsx
sync

/usr/sony/bin/ui_menu --power-off-enable &> /media/System/Logs/ui_menu.log
sync
sync
reboot
