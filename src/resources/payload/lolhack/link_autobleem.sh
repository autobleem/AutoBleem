#!/bin/bash

#clean after bleemsync

#This should replace links created by bleemsync with autobleem 

INPUT=/media/cbleemsync/autobleem.list
OLDIFS=$IFS
IFS=,
[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
while read number game_path 
do
	echo "$number -  $game_path"  >> /media/System/Logs/autobleem.log
	

ln -s $game_path/GameData /tmp/gaadatatmp/$number && mkdir -p $game_path/.pcsx && cp $game_path/GameData/pcsx.cfg $game_path/.pcsx
rm -rf /tmp/datatmp/AppData/sony/pcsx/$number && ln -s  $game_path /tmp/datatmp/AppData/sony/pcsx/$number 


done < $INPUT
IFS=$OLDIFS


sync
sync
sync
