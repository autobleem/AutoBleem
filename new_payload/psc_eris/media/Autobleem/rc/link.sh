#!/bin/sh

AUTOBLEEM_PATH="/media/Autobleem"
AUTOBLEEM_LOG="/System/Logs/autobleem.log"
# This is kind of deprecated - the overmount is not needed with EvoUI
### Probably can do with removing...

echo "Linking games"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
INPUT=${AUTOBLEEM_PATH}/bin/autobleem/autobleem.list
OLDIFS=$IFS
IFS=,
[ ! -f $INPUT ] && { echo "$INPUT file not found" >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"; exit 99; }
while read number game_path pcsx_path
do
	echo "$number -  $game_path  -  $pcsx_path"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    echo "Linking gaadata $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    # link back $game_path/GameData folder
    ln -s $game_path/ /tmp/gaadatatmp/$number
    echo "Cleaning PcsxState $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    # check if old pcsx folder exists
    if [ -f $game_path/PcsxState/pcsx.cfg ]
    then
       echo "Moving $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
       mv -f $game_path/PcsxState/* $pcsx_path/

       echo "Deleting $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
       rm -rf $game_path/PcsxState
    fi
    #overwrite configuration
    echo "Overwrite CFG $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    cp $game_path/pcsx.cfg $pcsx_path/
    echo "Cleanup $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    # remove data from savestates ??
    rm -rf /tmp/datatmp/AppData/sony/pcsx/$number
    #create nice folder to link
    mkdir -p /tmp/datatmp/AppData/sony/pcsx/$number
    # link back save states to folder
    echo "Linking data $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    ln -s  $pcsx_path/ /tmp/datatmp/AppData/sony/pcsx/$number/.pcsx
    echo "Done  $number"  >> "${AUTOBLEEM_PATH}${AUTOBLEEM_LOG}"
    done < $INPUT
IFS=$OLDIFS
sync