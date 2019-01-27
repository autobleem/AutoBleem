#!/bin/sh
echo "$@" >> /media/System/Logs/autobleem.log
echo "sony/title" >> /media/System/Logs/autobleem.log
ls -aR /data/AppData/sony/title/  >> /media/System/Logs/autobleem.log
echo "current" >> /media/System/Logs/autobleem.log
ls -alhR .  >> /media/System/Logs/autobleem.log
echo "pcsx" >> /media/System/Logs/autobleem.log
ls -alhR ./.pcsx/  >> /media/System/Logs/autobleem.log
/media/Autobleem/bin/starter $@

