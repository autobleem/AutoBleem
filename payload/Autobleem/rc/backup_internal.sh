#!/bin/bash

# Copy original save data to USB
for STATE in {1..20}
do
  mkdir -p /media/Games/!SaveStates/$STATE
  [ ! -f /media/Games/!SaveStates/$STATE/pcsx.cfg ] && cp -R /data/AppData/sony/pcsx/$STATE/.pcsx/* /media/Games/!SaveStates/$STATE
  [ ! -f /media/Games/!SaveStates/$STATE/pcsx.cfg ] && cp /gaadata/$STATE/pcsx.cfg /media/Games/!SaveStates/$STATE/pcsx.cfg
done

[ ! -f /media/System/Databases/internal.db ] && cp /gaadata/databases/regional.db /media/System/Databases/internal.db

sync
