#!/bin/bash

source ./autobleem_cfg.sh
echo MipMap: $AB_MIP
echo PCSX: $AB_PCSX


function select_pcsx
{
      echo "Custom PCSX ONLY in EvolutionUI !!!"
      cp -f /media/Autobleem/bin/emu/pcsx-ab /tmp/pcsx
      [ -f /tmp/pcsx ] && chmod +x /tmp/pcsx
}

# copy configuration to it's place
cp "$5/pcsx.cfg" "$1/pcsx.cfg"
# select pcsx to use
select_pcsx
# apply filter
# mipmap
# run the game

echo AUTOBLEEM: Starting PCSX
echo Cmd $@

mkdir -p /tmp/runpcsx
rm -rf /tmp/runpcsx/*

cd /tmp/runpcsx
ln -s "$1" /tmp/runpcsx/.pcsx

ln -s /media/System/Bios /tmp/runpcsx/bios
ln -s /data/AppData/sony/pcsx /tmp/runpcsx/plugins

FILTER=0;

if [ "$AB_MIP" = "true" ]
then
  FILTER=1
else
  FILTER=0
fi

/tmp/pcsx -filter $FILTER -ratio 1 -lang 2 -region 4 -enter 1 -cdfile "$2"


echo FINISHED

