#!/bin/bash

# Retroboot launcher for AutoBleem

#kill sony stuffs and set powermanagement parameters
killall -s KILL showLogo sonyapp ui_menu auto_dimmer pcsx dimmer 
echo 2 > /data/power/disable

# Do we really need to sync here?
# sync

sh /media/retroarch/retroboot/bin/launch_rom.rfa.sh "$1" "$2"
rm /tmp/.abload 
