#!/bin/bash
# Update Database on the fly
cd /media/cbleemsync
./run.sh
sync
cp /media/System/regional.db /media/System/Databases/
sync