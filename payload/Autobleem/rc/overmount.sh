#!/bin/sh
# This is kind of deprecated - the overmount is not needed with EvoUI
sync
# Overmount partitions on tmpfs

MOUNT_FAIL=0
# Create gaadata and data folders in tmp then mount over original folders

mount -o bind /tmp/gaadatatmp /gaadata || MOUNT_FAIL=1 
mount -o bind /tmp/datatmp /data || MOUNT_FAIL=1 
mount -o bind /media/Autobleem/rc/20-joystick.rules /etc/udev/rules.d/20-joystick.rules || MOUNT_FAIL=1 
[ $MOUNT_FAIL -eq 1 ] && reboot && exit



# Fix for last selected game issue. If not in place user may experience UI issue
sed -i "s/iUiUserSettingLastSelectGameCursorPos.*/iUiUserSettingLastSelectGameCursorPos=0/" /tmp/datatmp/AppData/sony/ui/user.pre

# Reload and apply udev rules that were overmounted above
# Allows both controllers to be detected through a USB hub
udevadm control --reload-rules
udevadm trigger







