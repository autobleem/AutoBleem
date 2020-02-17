#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"

mkdir -p "${AUTOBLEEM_PATH}/Games" "${AUTOBLEEM_PATH}/roms" "${AUTOBLEEM_PATH}/System/Databases" "${AUTOBLEEM_PATH}/System/Logs"

LD_LIBRARY_PATH="/tmp/lib" ./autobleem-gui "${AUTOBLEEM_PATH}/System/Databases/regional.db" "${AUTOBLEEM_PATH}/Games/" > "/media//Autobleem/System/Logs/AB_out.txt" 2> "${AUTOBLEEM_PATH}/System/Logs/AB_err.txt"

