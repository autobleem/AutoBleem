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

rm -f "${RUNTIME_LOG_PATH}/retroarch.log"
chmod +x "${PROJECT_ERIS_PATH}/bin/ra_watch"
chmod +x "${RETROARCH_PATH}/retroarch"
touch "/tmp/launch_ra.flag"
export XDG_CONFIG_HOME="${RETROARCH_PATH}/config"

kill_it_with_fire &

while [ -f "/tmp/launch_ra.flag" ]; do
  rm -f "/tmp/launch_ra.flag"
  PATH="${PATH}:${PROJECT_ERIS_PATH}/bin" "${RETROARCH_PATH}/retroarch" --config "${RETROARCH_PATH}/config/retroarch/retroarch.cfg" -L "${RA_CORE}" "${RA_GAME}" "${RA_ARGS}" 2>&1 | "${PROJECT_ERIS_PATH}/bin/ra_watch" >> "${RUNTIME_LOG_PATH}/retroarch.log"
done

sh /media/Autobleem/start.sh