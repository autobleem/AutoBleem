#!/bin/sh

killall -s KILL showLogo sonyapp ui_menu auto_dimmer pcsx
echo 2 > /data/power/disable
sync
MOUNT_FAIL=0
umount /data || MOUNT_FAIL=1 
umount /gaadata || MOUNT_FAIL=1 
[ $MOUNT_FAIL -eq 1 ] && reboot && exit

mkdir -p "/tmp/ra_cache"
chmod +x /media/RetroArch/retroarch
HOME=/media/RetroArch /media/RetroArch/retroarch -v &> /media/RetroArch/retroarch.log
rm -rf "/tmp/ra_cache"
sync
reboot
