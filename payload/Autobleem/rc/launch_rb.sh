#!/bin/bash

# Retroboot launcher for AutoBleem

RBCORE=/media/retroarch/cores/km_pcsx_rearmed_neon_libretro.so

#kill sony stuffs and set powermanagement parameters
killall -s KILL showLogo sonyapp ui_menu auto_dimmer pcsx dimmer autobleem-gui
echo 2 > /data/power/disable
sync

sh /media/retroarch/retroboot/bin/launch_rom.sh $RBCORE "$1"



