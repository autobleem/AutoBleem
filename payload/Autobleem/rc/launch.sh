#!/bin/bash


# copy configuration to it's place
cp "$5/pcsx.cfg" "$1/pcsx.cfg"
# select pcsx to use

echo "Custom PCSX ONLY in EvolutionUI !!!"
cp -f /media/Autobleem/bin/emu/pcsx-ab /tmp/pcsx
[ -f /tmp/pcsx ] && chmod +x /tmp/pcsx


echo AUTOBLEEM: Starting PCSX
echo Cmd $@

rm -rf /tmp/runpcsx
mkdir -p /tmp/runpcsx

cd /tmp/runpcsx
ln -s "$1" /tmp/runpcsx/.pcsx

ln -s /media/System/Bios /tmp/runpcsx/bios
ln -s /media/Autobleem/bin/emu/plugins /tmp/runpcsx/plugins

if [ "$6" == "0" ]
then
  /tmp/pcsx -filter $8 -ratio $7 -lang $3 -region 4 -enter 1 -cdfile "$2"
else
  /tmp/pcsx -filter $8 -ratio $7 -lang $3 -region 4 -enter 1 -load $6 -cdfile "$2"
fi

echo FINISHED

