#!/bin/sh

# It crashes here as when device is busy it can not be umounted - need to retry in a for loop
# TODO: make some prevention !!!

sync
# Unmount partitons and create tmpfs - Shut system down on failure
MOUNT_FAIL=0

#umount /data || MOUNT_FAIL=1
#umount /gaadata || MOUNT_FAIL=1 // we do not need to umunt gaadata

# Create gaadata and data folders in tmp then mount over original folders
mkdir -p /tmp/gaadatatmp /tmp/datatmp
mount -o bind /tmp/gaadatatmp /gaadata || MOUNT_FAIL=1 
mount -o bind /tmp/datatmp /data || MOUNT_FAIL=1 
mount -o bind /media/Autobleem/rc/20-joystick.rules /etc/udev/rules.d/20-joystick.rules || MOUNT_FAIL=1 
[ $MOUNT_FAIL -eq 1 ] && reboot && exit

# Create gaadata on tmpfs
mkdir -p /tmp/gaadatatmp/system/
ln -s /media/System/Databases /tmp/gaadatatmp/databases
ln -s /media/System/Region /tmp/gaadatatmp/geninfo
ln -s /media/System/Bios /tmp/gaadatatmp/system/bios
ln -s /media/System/Preferences/System /tmp/gaadatatmp/preferences

# Create data on tmpfs
mkdir -p /tmp/datatmp/sony/sgmo /tmp/datatmp/AppData/sony
ln -s /tmp/diag /tmp/datatmp/sony/sgmo/diag
ln -s /dev/shm/power /tmp/datatmp/power
ln -s /media/System/UI /tmp/datatmp/sony/ui
ln -s /media/System/Preferences/User /tmp/datatmp/AppData/sony/ui
ln -s /media/System/Preferences/AutoDimmer /tmp/datatmp/AppData/sony/auto_dimmer
cp -r /usr/sony/share/recovery/AppData/sony/pcsx /tmp/datatmp/AppData/sony/pcsx
ln -s /media/System/Bios /tmp/datatmp/AppData/sony/pcsx/bios
ln -s /usr/sony/bin/plugins /tmp/datatmp/AppData/sony/pcsx/plugins


# Fix for last selected game issue. If not in place user may experience UI issue
sed -i "s/iUiUserSettingLastSelectGameCursorPos.*/iUiUserSettingLastSelectGameCursorPos=0/" /tmp/datatmp/AppData/sony/ui/user.pre

# Reload and apply udev rules that were overmounted above
# Allows both controllers to be detected through a USB hub
udevadm control --reload-rules
udevadm trigger







