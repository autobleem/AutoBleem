#!/bin/bash


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
sync
