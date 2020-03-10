#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"

# Copy original save data to USB
for STATE in {1..20}
do
  mkdir -p "${AUTOBLEEM_PATH}/Games/!SaveStates/${STATE}"
  [ ! -f "${AUTOBLEEM_PATH}/Games/!SaveStates/${STATE}/pcsx.cfg" ] && cp -R "/data/AppData/sony/pcsx/${STATE}/.pcsx/"* "${AUTOBLEEM_PATH}/Games/!SaveStates/${STATE}"
  [ ! -f "${AUTOBLEEM_PATH}/Games/!SaveStates/${STATE}/pcsx.cfg" ] && cp "/gaadata/${STATE}/pcsx.cfg ${AUTOBLEEM_PATH}/Games/!SaveStates/${STATE}/pcsx.cfg"
done

[ ! -f "${AUTOBLEEM_PATH}/System/Databases/internal.db" ] && cp "/gaadata/databases/regional.db" "${AUTOBLEEM_PATH}/System/Databases/internal.db"

sync
