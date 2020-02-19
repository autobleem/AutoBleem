#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"

# TODO: refactor.

#PCSX launcher for AutoBleem

# copy configuration to it's place
if [[ ${5} == *"/gaadata"* ]]; then
  echo "Internal game"
else
  cp "${5}/pcsx.cfg" "${1}/pcsx.cfg"
fi
# select pcsx to use
cp -f "${AUTOBLEEM_PATH}/bin/emu/pcsx-ab" "/tmp/pcsx" #??? is this even needed PCSX is already patched
[ -f "/tmp/pcsx" ] && chmod +x "/tmp/pcsx"
echo "AUTOBLEEM: Starting PCSX"
echo "Cmd $@"
rm -rf "/tmp/runpcsx"
mkdir -p "/tmp/runpcsx"
cd "/tmp/runpcsx"
ln -s "${1}" "/tmp/runpcsx/.pcsx"
ln -s "${AUTOBLEEM_PATH}/System/Bios" "/tmp/runpcsx/bios"
ln -s "${AUTOBLEEM_PATH}/bin/emu/plugins" "/tmp/runpcsx/plugins"
if [ "${6}" == "0" ]; then
  /tmp/pcsx -filter ${8} -ratio ${7} -lang ${3} -region 4 -enter 1 -cdfile "${2}" -pad ${9}
else
  /tmp/pcsx -filter ${8} -ratio ${7} -lang ${3} -region 4 -enter 1 -load ${6} -cdfile "${2}" -pad ${9}
fi