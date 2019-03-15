#!/bin/sh

# PSC_RetroBoot 0.5.5 - monitor.sh 
# Monitor RetroArch logs for Wayland errors

rm /media/retroarch/monitor.log
sleep 20

while : ; do
    sleep 5

    tail -n 25 /media/RetroArch/retroarch.log | grep -q "wl_display@1: error"
    RETVAL=$?

    if [ $RETVAL -eq 0 ]; then
        touch /tmp/retroboot/.monitor_killed_ra
        killall retroarch

        sleep 20
    fi
done