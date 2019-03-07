#!/bin/sh

#kill sony stuffs and set powermanagement parameters
killall -s KILL showLogo sonyapp ui_menu auto_dimmer pcsx dimmer
echo 2 > /data/power/disable
sync

#duplicate internal PSX bios to a bunch of bios files required by retroarch
if [ ! -f "/media/RetroArch/system/scph102b.bin" ]
    then
    cp -f "/gaadata/system/bios/romw.bin" "/media/RetroArch/system/scph102b.bin"
    fi
if [ ! -f "/media/RetroArch/system/scph5500.bin" ]
    then
    cp -f "/gaadata/system/bios/romw.bin" "/media/RetroArch/system/scph5500.bin"
    fi
if [ ! -f "/media/RetroArch/system/scph5501.bin" ]
    then
    cp -f "/gaadata/system/bios/romw.bin" "/media/RetroArch/system/scph5501.bin"
    fi
if [ ! -f "/media/RetroArch/system/scph5502.bin" ]
    then
    cp -f "/gaadata/system/bios/romw.bin" "/media/RetroArch/system/scph5502.bin"
    fi

#make default retroarch config active, if its not already active
if [ ! -f "/media/RetroArch/.config/retroarch/retroarch.cfg" ]
    then
    cp -f "/media/RetroArch/.config/retroarch/retroarch_DEFAULT.cfg" "/media/RetroArch/.config/retroarch/retroarch.cfg"
    fi
sync

#create RA cache folder
mkdir -p "/tmp/ra_cache"

#set execute permission
chmod +x /media/RetroArch/retroarch

#set Home directory and run retroarch
HOME=/media/RetroArch /media/RetroArch/retroarch -v &> /media/RetroArch/retroarch.log

#delete cache directory
rm -rf "/tmp/ra_cache"
sync

#return to Autobleem UI
cd /media/Autobleem/
./start.sh