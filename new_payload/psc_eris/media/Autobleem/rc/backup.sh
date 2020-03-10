#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"

mount -o remount,ro /gaadata
mount -o remount,ro /data

mkdir -p "${AUTOBLEEM_PATH}/Games/!SaveStates"
mkdir -p "${AUTOBLEEM_PATH}/System/Bios"
mkdir -p "${AUTOBLEEM_PATH}/System/Preferences"
mkdir -p "${AUTOBLEEM_PATH}/System/Preferences/System"
mkdir -p "${AUTOBLEEM_PATH}/System/Preferences/User"
mkdir -p "${AUTOBLEEM_PATH}/System/Preferences/AutoDimmer"
mkdir -p "${AUTOBLEEM_PATH}/System/Databases"
mkdir -p "${AUTOBLEEM_PATH}/System/Region"
mkdir -p "${AUTOBLEEM_PATH}/System/Logs"
mkdir -p "${AUTOBLEEM_PATH}/System/UI"

cp -r "${AUTOBLEEM_PATH}/bin/autobleem/AutoBleem.rdb" "${AUTOBLEEM_PATH}/retroarch/database/rdb/"

# Copy the BIOS files to USB
[ ! -f "${AUTOBLEEM_PATH}/System/Bios/romw.bin" ] && cp -r "/gaadata/system/bios/romw.bin" "${AUTOBLEEM_PATH}/System/Bios/romw.bin"
[ ! -f "${AUTOBLEEM_PATH}/System/Bios/romJP.bin" ] && cp -r "${AUTOBLEEM_PATH}/System/Bios/romw.bin" "${AUTOBLEEM_PATH}/System/Bios/romJP.bin"

# Copy the regional.pre to USB
# This contains settings for the UI region
[ ! -f "${AUTOBLEEM_PATH}/System/Preferences/System/regional.pre" ] && cp "/gaadata/preferences/"* "${AUTOBLEEM_PATH}/System/Preferences/System/"

# Copy out the user.pre to USB
# This contains things like language setting
[ ! -f "${AUTOBLEEM_PATH}/System/Preferences/User/user.pre" ] && cp "/data/AppData/sony/ui/"* "${AUTOBLEEM_PATH}/System/Preferences/User/"

# Copy out the auto dimming config to USB
[ ! -f "${AUTOBLEEM_PATH}/System/Preferences/AutoDimmer/config.cnf" ] && cp "/data/AppData/sony/auto_dimmer/"* "${AUTOBLEEM_PATH}/System/Preferences/AutoDimmer/"

# Copy out the region info
[ ! -f "${AUTOBLEEM_PATH}/System/Region/REGION" ] && cp "/gaadata/geninfo/"* "${AUTOBLEEM_PATH}/System/Region/"

# Copy ui error log
[ ! -f "${AUTOBLEEM_PATH}/System/UI/error.log" ] && cp "/data/sony/ui/"* "${AUTOBLEEM_PATH}/System/UI/"

# Init the ui_menu.log
[ ! -f "${AUTOBLEEM_PATH}/System/Logs/ui_menu.log" ] && touch "${AUTOBLEEM_PATH}/System/Logs/ui_menu.log"

# mkdir -p "/tmp/gaadatatmp" "/tmp/datatmp"
# # Create gaadata on tmpfs
# mkdir -p "/tmp/gaadatatmp/system/"
# ln -s "${AUTOBLEEM_PATH}/System/Databases /tmp/gaadatatmp/databases"
# ln -s "${AUTOBLEEM_PATH}/System/Region /tmp/gaadatatmp/geninfo"
# ln -s "${AUTOBLEEM_PATH}/System/Bios /tmp/gaadatatmp/system/bios"
# ln -s "${AUTOBLEEM_PATH}/System/Preferences/System /tmp/gaadatatmp/preferences"

# # Create data on tmpfs
# mkdir -p "/tmp/datatmp/sony/sgmo" "/tmp/datatmp/AppData/sony"
# ln -s "/tmp/diag" "/tmp/datatmp/sony/sgmo/diag"
# ln -s "/dev/shm/power" "/tmp/datatmp/power"
# ln -s "${AUTOBLEEM_PATH}/System/UI" "/tmp/datatmp/sony/ui"
# ln -s "${AUTOBLEEM_PATH}/System/Preferences/User" "/tmp/datatmp/AppData/sony/ui"
# ln -s "${AUTOBLEEM_PATH}/System/Preferences/AutoDimmer" "/tmp/datatmp/AppData/sony/auto_dimmer"
# cp -r "/usr/sony/share/recovery/AppData/sony/pcsx" "/tmp/datatmp/AppData/sony/pcsx"
# ln -s "${AUTOBLEEM_PATH}/System/Bios" "/tmp/datatmp/AppData/sony/pcsx/bios"
# ln -s "/usr/sony/bin/plugins" "/tmp/datatmp/AppData/sony/pcsx/plugins"

# #disable power button event
# echo 2 > "/tmp/datatmp/power/disable"
