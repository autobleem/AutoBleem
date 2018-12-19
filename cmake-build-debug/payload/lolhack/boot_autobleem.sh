#!/bin/bash
# Update Database on the fly
cd /media/cbleemsync
mkdir -p /media/System/
mkdir -p /media/System/Databases/
./run.sh
sync
cp /media/System/regional.db /media/System/Databases/
sync