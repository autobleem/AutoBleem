#!/bin/sh

source "/var/volatile/project_eris.cfg"

RA_ARGS=""
RA_GAME="${1}"
RA_CORE="${2}"

kill_it_with_fire(){
  n=0
  while [ "${n}" -lt 50 ]; do
    n=$(( n + 1 ))
    killall autobleem-gui
    sleep 0.1
  done
}

# This kinda sucks, AB should calculate and dictate RA core and use the RA pathing environment variable.
# PEOPS is also not able to run full speed so the likelihood is 95% of people will just use the NEON build.
# Not 100% sure if this is all necessary. But hopefuly can remove in the future.
if [ ${RA_CORE} = "PEOPS" ]; then
  RA_CORE="${RETROARCH_PATH}/config/retroarch/cores/pcsx_rearmed_libretro.so"
elif [ ${RA_CORE} = "NEON" ]; then
  RA_CORE="${RETROARCH_PATH}/config/retroarch/cores/pcsx_rearmed_libretro.so"
else
  RA_CORE=$2
fi

rm -f "${RUNTIME_LOG_PATH}/retroarch.log"
touch "/tmp/launch_ra.flag"
export XDG_CONFIG_HOME="${RETROARCH_PATH}/config"

echo "AutoBleem RetroArch Launcher" >> "${RUNTIME_LOG_PATH}/retroarch.log"
echo "GAME: ${RA_GAME}" >> "${RUNTIME_LOG_PATH}/retroarch.log"
echo "CORE: ${RA_CORE}" >> "${RUNTIME_LOG_PATH}/retroarch.log"
echo "EXTRA ARGS: ${RA_ARGS}" >> "${RUNTIME_LOG_PATH}/retroarch.log"

kill_it_with_fire &

PATH="${PATH}:${PROJECT_ERIS_PATH}/bin" "${RETROARCH_PATH}/retroarch" --config "${RETROARCH_PATH}/config/retroarch/retroarch.cfg" -L "${RA_CORE}" "${RA_GAME}" "${RA_ARGS}" 2>&1 | "${PROJECT_ERIS_PATH}/bin/ra_watch" >> "${RUNTIME_LOG_PATH}/retroarch.log"

rm -f "/tmp/launch_ra.flag"

echo "sh /media/Autobleem/start.sh"  > "/tmp/launchfilecommand"