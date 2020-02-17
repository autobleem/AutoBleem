#!/bin/sh

#kill sony stuffs and set powermanagement parameters
killall -s KILL showLogo sonyapp ui_menu auto_dimmer pcsx dimmer

sh /media/retroarch/Retroarch/bin/launch_rfa.sh
rm /tmp/.abload 
