#!/bin/bash
mount -o remount,ro /gaadata
mount -o remount,ro /data


mkdir -p /media/Games/!SaveStates
mkdir -p /media/System/Bios
mkdir -p /media/System/Preferences
mkdir -p /media/System/Preferences/System
mkdir -p /media/System/Preferences/User
mkdir -p /media/System/Preferences/AutoDimmer
mkdir -p /media/System/Databases
mkdir -p /media/System/Region
mkdir -p /media/System/Logs
mkdir -p /media/System/UI

if [ -f /media/retroarch/retroboot ]
then
  [ ! -f /media/retroarch/retroboot/bin/launch_rom.sh  ] && cp -r /media/Autobleem/bin/autobleem/launch_rom.sh /media/retroarch/retroboot/bin/
fi

# Copy the BIOS files to USB
[ ! -f /media/System/Bios/romw.bin ] && cp -r /gaadata/system/bios/romw.bin /media/System/Bios/romw.bin
[ ! -f /media/System/Bios/romJP.bin ] && cp -r /media/System/Bios/romw.bin /media/System/Bios/romJP.bin

# Copy the regional.pre to USB
# This contains settings for the UI region
[ ! -f /media/System/Preferences/System/regional.pre ] && cp /gaadata/preferences/* /media/System/Preferences/System

# Copy out the user.pre to USB
# This contains things like language setting
[ ! -f /media/System/Preferences/User/user.pre ] && cp /data/AppData/sony/ui/* /media/System/Preferences/User

# Copy out the auto dimming config to USB
[ ! -f /media/System/Preferences/AutoDimmer/config.cnf ] && cp /data/AppData/sony/auto_dimmer/* /media/System/Preferences/AutoDimmer

# Copy out the region info
[ ! -f /media/System/Region/REGION ] && cp /gaadata/geninfo/* /media/System/Region

# Copy ui error log
[ ! -f /media/System/UI/error.log ] && cp /data/sony/ui/* /media/System/UI

# Init the ui_menu.log
[ ! -f /media/System/Logs/ui_menu.log ] && touch /media/System/Logs/ui_menu.log

mkdir -p /tmp/gaadatatmp /tmp/datatmp
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

#disable power button event
echo 2 > /tmp/datatmp/power/disable