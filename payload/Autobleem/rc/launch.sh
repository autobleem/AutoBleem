#!/bin/bash

source ./autobleem_cfg.sh
echo Selection: $AB_SELECTION
echo MipMap: $AB_MIP
echo PCSX: $AB_PCSX

function mipmap
{
     if [ "$AB_MIP" = "true" ]
     then
        ./mipmap.sh
     fi
}
function select_pcsx
{
   if [ "$AB_PCSX" = "original" ]
   then
      echo Original PCSX
      cp -f /media/Autobleem/bin/emu/pcsx-sony /tmp/pcsx
      [ -f /tmp/pcsx ] && chmod +x /tmp/pcsx
   else
      echo Custom PCSX
      cp -f /media/Autobleem/bin/emu/pcsx /tmp/pcsx
      [ -f /tmp/pcsx ] && chmod +x /tmp/pcsx
   fi
}

# copy configuration to it's place
cp "$5/pcsx.cfg" "$1/pcsx.cfg"
# select pcsx to use
select_pcsx
# apply filter
mipmap
# run the game

echo AUTOBLEEM: Starting PCSX
echo Cmd $@

mkdir -p /tmp/runpcsx
rm -rf /tmp/runpcsx/*

cd /tmp/runpcsx
ln -s "$1" /tmp/runpcsx/.pcsx

ln -s /media/System/Bios /tmp/runpcsx/bios
ln -s /data/AppData/sony/pcsx /tmp/runpcsx/plugins

/tmp/pcsx -lang $3 -region $4  -cdfile "$2"
echo FINISHED

