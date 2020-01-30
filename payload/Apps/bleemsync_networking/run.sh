#!/bin/sh

#set up path
export bleemsync_path="/media/bleemsync"
mkdir -p "/media/logs"

#fix line terminators
if [ -f "$bleemsync_path/etc/bleemsync/CFG/ssid.cfg" ]; then
    cp "$bleemsync_path/etc/bleemsync/CFG/ssid.cfg" "/tmp/ssid.cfg"
    tr -d '\r' < "/tmp/ssid.cfg" > "$bleemsync_path/etc/bleemsync/CFG/ssid.cfg"
fi


for mod in "$bleemsync_path/etc/bleemsync/SUP/modules/"*.ko; do insmod "$mod"; done
source "$bleemsync_path/etc/bleemsync/FUNC/0000_shared.funcs"
source "$bleemsync_path/etc/bleemsync/FUNC/0010_execute.funcs"
source "$bleemsync_path/etc/bleemsync/FUNC/0030_retroarch.funcs"
source "$bleemsync_path/etc/bleemsync/FUNC/0040_theme.funcs"
source "$bleemsync_path/etc/bleemsync/FUNC/0050_bleemsync.funcs"
source "$bleemsync_path/etc/bleemsync/FUNC/0060_network.funcs"
init_networking &> "/media/logs/network.log"