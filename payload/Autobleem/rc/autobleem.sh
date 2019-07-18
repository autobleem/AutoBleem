#!/bin/bash
# Update Database on the fly
# Extract system files to avoid crashing
mkdir -p /tmp/lib
cp /media/Autobleem/lib/libs.tar.gz /tmp/lib
cd /tmp/lib
tar xvzf libs.tar.gz
cd /media/Autobleem/bin/autobleem
./run.sh
sync


