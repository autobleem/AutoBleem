#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"

find "${AUTOBLEEM_PATH}/Games" -name *".png"  -exec rm -rf {} \;
