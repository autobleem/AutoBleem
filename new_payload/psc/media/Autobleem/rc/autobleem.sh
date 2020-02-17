#!/bin/sh

# TODO: Feb 2020 All these scripts are in desperate need of refactoring... So only doing a brief refactor for
# time being. If I have time in the future I will redo alot of this

AUTOBLEEM_PATH="/media/Autobleem"

# Update Database on the fly
# Extract system files to avoid crashing
mkdir -p "/tmp/lib"
cp "${AUTOBLEEM_PATH}/lib/libs.tar.gz" "/tmp/lib"
cd "/tmp/lib"
tar xvzf "libs.tar.gz"

cd "${AUTOBLEEM_PATH}/bin/autobleem"
./run.sh
sync