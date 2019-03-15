#
# RetroBoot 0.5.5 based code as agreed with genderbent
#

#!/bin/bash
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

# Start RA log monitor 
sh /media/Autobleem/rc/rb_monitor.sh &
MONPID=$!

# Start RetroArch.  Restart it if it crashes.
export XDG_CONFIG_HOME=/media/
while : ; do
    rm /tmp/retroboot/.monitor_killed_ra
   # /media/retroarch/retroarch --config /media/retroarch/config/retroarch.cfg --menu &> /media/retroarch/retroarch.log
    HOME=/media/RetroArch /media/RetroArch/retroarch -v --menu --config /media/RetroArch/.config/retroarch/retroarch.cfg &> /media/RetroArch/retroarch.log
    LVL=$?

    if [ $LVL -eq 0 ] && [ ! -f /tmp/retroboot/.monitor_killed_ra ]; then
        break
    fi

    echo 0 > /sys/class/leds/green/brightness
    echo 1 > /sys/class/leds/red/brightness

    mv /media/RetroArch/retroarch.log "/media/RetroArch/retroarch_crash.log"
    printf "\n--End of retroarch.log--\n\nOutput from dmesg:\n\n" >> /media/RetroArch/retroarch_crash.log
    dmesg >> /media/RetroArch/retroarch_crash.log
    printf "\n--End of Log--\n" >> /media/RetroArch/retroarch_crash.log



    # Flash leds to indicate restart
    for i in 1 2 3 4
    do
        echo 1 > /sys/class/leds/red/brightness
        usleep 125000
        echo 0 > /sys/class/leds/red/brightness
        usleep 125000
    done

    echo 1 > /sys/class/leds/green/brightness
done

# Kill the monitor
kill $MONPID


rm -rf "/tmp/ra_cache"
sync

#return to Autobleem UI
cd /media/Autobleem/
./start.sh