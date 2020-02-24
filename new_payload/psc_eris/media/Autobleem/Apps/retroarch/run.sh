#!/bin/sh

kill_it_with_fire(){
  n=0
  while [ "${n}" -lt 50 ]; do
    n=$(( n + 1 ))
    killall autobleem-gui
    sleep 0.1
  done
}

echo "launch_retroarch_standalone_AB" > "/tmp/launchfilecommand"
# TODO: Find more graceful way to kill Autobleem
kill_it_with_fire &