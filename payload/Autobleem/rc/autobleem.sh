#!/bin/bash
# Update Database on the fly
# Extract system files to avoid crashing
mkdir -p /media/System/Bios
mkdir -p /media/System/Preferences
mkdir -p /media/System/Preferences/System
mkdir -p /media/System/Preferences/User
mkdir -p /media/System/Preferences/AutoDimmer
mkdir -p /media/System/Databases
mkdir -p /media/System/Region
mkdir -p /media/System/Logs
mkdir -p /media/System/UI
sync
cd /media/Autobleem/bin/autobleem
./run.sh
sync


