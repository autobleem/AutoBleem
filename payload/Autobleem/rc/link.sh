#!/bin/bash

echo "Linking games"  >> /media/System/Logs/autobleem.log
INPUT=/media/Autobleem/bin/autobleem/autobleem.list
OLDIFS=$IFS
IFS=,
[ ! -f $INPUT ] && { echo "$INPUT file not found" >> /media/System/Logs/autobleem.log; exit 99; }
while read number game_path pcsx_path
do
	echo "$number -  $game_path  -  $pcsx_path"  >> /media/System/Logs/autobleem.log
    echo "Linking gaadata $number"  >> /media/System/Logs/autobleem.log

    # link back $game_path/GameData folder
    ln -s $game_path/ /tmp/gaadatatmp/$number

    echo "Cleaning PcsxState $number"  >> /media/System/Logs/autobleem.log
    # check if old pcsx folder exists
    if [ -f $game_path/PcsxState/pcsx.cfg ]
    then
       echo "Moving $number"  >> /media/System/Logs/autobleem.log
       mv -f $game_path/PcsxState/* $pcsx_path/

       echo "Deleting $number"  >> /media/System/Logs/autobleem.log
       rm -rf $game_path/PcsxState
    fi


    #overwrite configuration
    echo "Overwrite CFG $number"  >> /media/System/Logs/autobleem.log
    cp $game_path/pcsx.cfg $pcsx_path/

    echo "Cleanup $number"  >> /media/System/Logs/autobleem.log
    # remove data from savestates ??

    rm -rf /tmp/datatmp/AppData/sony/pcsx/$number
    #create nice folder to link

    mkdir -p /tmp/datatmp/AppData/sony/pcsx/$number
    # link back save states to folder

    echo "Linking data $number"  >> /media/System/Logs/autobleem.log
    ln -s  $pcsx_path/ /tmp/datatmp/AppData/sony/pcsx/$number/.pcsx

    echo "Done  $number"  >> /media/System/Logs/autobleem.log
    done < $INPUT

IFS=$OLDIFS
sync

