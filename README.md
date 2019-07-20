### Please Read: Important note about upgrading from previous versions with RetroBoot 0.9 installed manually
The non RetroBoot enabled packages (full, ntscU, palE, ntscJ) contain a startup script that is not compatible with RetroBoot 0.9. If RetroBoot 0.9 was previously installed manually on your build and you see issues running RetroArch  please update one file on your build to fix the loader:

[RetroBoot startup script](https://github.com/screemerpl/cbleemsync/releases/download/0.6.0-b3/retroarch.sh)

Replace the file in /Autobleem/rc with the version downloaded from this link

### Current version 0.6.0b3 please update your installations [HERE](https://github.com/screemerpl/cbleemsync/releases)

#### Please read this page before downloading AutoBleem [USB compatibility FAQ](https://github.com/screemerpl/cbleemsync/wiki/USB-compatibility-FAQ)

### For installation tutorial look on YouTube videos on channels: Patton Plays, Restalgia, Madlittlepixel and others.

Join our discord server for support: [https://discord.gg/AHUS3RM](https://discord.gg/AHUS3RM)

#### NOTE: You do not need to uninstall BleemSync 0.4/0.7/1.0/1.0.1 from your console prior to use AutoBleem. Prepare new USB stick with AutoBleem and it will work as expected. 

#### NOTE: If you already installed BleemSync 1.1 firmware in your PSC AutoBleem can be booted from rear USB port

# AutoBleem 
AutoBleem is a tool to make your PlayStation Classic more usefull.
It is relatively safe way to add games , change look and feel, change advanced configuration of your mini console.
     
## Why this project

This is my personal project not related to BleemSync. I started it at the day when lolhack was published. The idea was to create "something easier to use" than lolhack/gpghax. Soon BleemSync was released, but I was not fully happy how it worked at that time, so I continued to develop AutoBleem as an alternative/(initially an extension for BleemSync). As BleemSync changed over that time drastically I decided to continue working on AutoBleem as a separate alternative mod for PSC.

If you want some new feature, or you find a bug please raise the issue here, or contact us on Discord.

## Is it stable and is it safe to use ?
Version 0.6.0 is "pretty" stable, but it is beta and... will be beta until we reach all of our goals. Do not be afraid ... its fully safe to use and will not brick your PSC as it does not write anything to internal storage in the console. The beta status only states that some functionalities we would want to have are still under development.  Also its well tested by multiple users and testers. 

If you found an issue - report it using "Issues" section on this website or better contact us directly on Discord.

**NOTE: You do not need to uninstall BleemSync 1.0/1.1 if it was already installed**

#### NOTE: This tool is made to use only with legal licenced games. The whole project does not use any licenced source code, also it does not alter any of the Sony files inside the mini console, it just uses what is already there and not alter it in any way.

## Features 

1. Replaces Bleemsync, Bootmenu and SonyUI fully for better experience
1. Integrated functionality to share memory cards between games
1. Separates games and it's save states into separate folders
1. AutoBleem does not need to run anything on the PC. Just copy games to usb dongle and plug it in. 
1. Does not use any internet or usb connection 
1. Modifies or replaces (selectable in options) the stock UI to show added games
1. Uses **GAME NAMES as folders** instead of numbered folders
1. Can create game folders automatically
1. Supports multi-disc games
1. **Includes offline metadata and cover art databases - no download needed during sync**
1. **Runs on boot of the PlayStation Classic** as it was designed to do so from the beginning
1. Speaks your language
1. Makes your PlayStation Classic a great device
1. Starting with v0.6.0b3 includes prebuild distribution of RetroBoot by genderbent - RetroBoot is a PSC tailored distribution of RetroArch - an open source emulator that can emulate other consoles and makes you able to play more and more games. The package is provided by RetroBoot developers.  

## Installation

1. Download the ZIP file from the release page [HERE](https://github.com/screemerpl/cbleemsync/releases) - you can select one of the packages 
  
  - autobleem-0.6.0-full - contains all covers for any region
  - autobleem-0.6.0-ntscU/palE/ntscJ - contains only covers for selected region
  - autobleem-0.6.0-clean - no cover databases (have to be installed manually) 
  - autobleem-0.6.0-RetroBoot - a version based on full package but including RetroBoot distribution of RetroArch preinstalled
  
1. Extract the contents to the root of your FAT32(recommended) or ext4 formatted USB flash drive
1. Name your flash drive SONY. This is a requirement.
1. If you decided to use clean installer - download Cover Repository databases (three separate files for NTSC-U, PAL and NTSC-J)
1. Copy Cover Repository databases into /{Your USB}/Autobleem/bin/db not needed if you downloaded full or region packages
1. You may want to install RetroArch (the folder is empty) - find it on ModMyClassic website - I do not distribute this software.
1. You may want to install additional themes ... find them on reddit and copy to themes folder (follow folder structure)

## Automatic Game scanning

To allow AutoBleem to scan your games they need to be inserted into Game folder. Just take your game folder and drop it there. As soon there is .bin file inside this folder AutoBleem will try to generate all other needed files for you on console boot. So structure can be as follows:

```
Games/
     Resident Evil 2 Leon/
           Resident Evil.bin
     Silent Hill/
            Silent Hill.cue
            Silent Hill.bin
     Revolt/
             Revolt.PBP
```
This version can also create those folders for you if you drop them directly to Game folder, but it is based on an assumption that game image files are correct (especially CUE files) and some games may not be added this way. 

There are databases attached to this file with all covers and metadata. Also AutoBleem will create folders to store your save data and configuration.

## Database files

### General usage of database files

The inital release package "Full version" DO contain Cover Repository databases. And this is probably everything you need to start your build.  In case you want to make the installation small, doenload "Clean version" and then copy the databases to your USB dongle as on previous releases. 

Database is split into three separate files by region. If you do not for example plan to play JAP region games, you can install just US or PAL. The reason for that split is the file size. Original cover repository in 226x226 PNG was about 1GB in size. I managed to use a lot of optimisations and finally shrink this into much smaller size, but it still is bit big for a small USB dongle. 

The files are available on release page. Look for latest release for most updated databases. 


**REMEBER COPY DATABASE FILES TO /Autobleem/bin/db folder on USB, not any other place**

### Custom DB Updates


We also created a tool to dump and reload all png files in one shot. The tool is available here [DOWNLOAD](https://github.com/autobleem/abcoverpacker) 

There ia also a tool to edit cover databases available here [DOWNLOAD](https://github.com/autobleem/abCoverEdit)

## Credits and links 

This software uses some parts of code based on open licences:

[BleemSync](https://github.com/pathartl/BleemSync) - some parts of bash scripts were reused from early BleemSync code. The credits go to ModMyClassic team (DoctorDalek/pathartl, compscom, Swingflip, cyanic and others).

[SQLite](https://www.sqlite.org/index.html) (sqlite.c / sqlite.h) Public domain - no license

UNECM - Copyright (C) 2002 Neill Corlett (GPLv2) (unecm.c)

RetroBoot distribution of Retroarch - genderbent, cores by KMDFManic - All components of the distribution are licenced as described here:
http://docs.libretro.com/development/licenses/


## Frequently Asked Questions

### My Console Crashes / Does not load AutoBleem / I can not play games / AutoBleem goes to black screen

Get to our Discord for support.

### Can I extend this project ? 
Of course ... it is fully open source using GPL3.0, also if you know any C++, SQL, Java or even how to make youtube videos talk to us on our Discord and join our team.

### I can not see my game
AutoBleem does scan the game folder during boot and using smart algorithm it is trying to generate all files needed to run the game, but sometimes it fails. In this case AutoBleem WILL NOT ADD THE GAME to the MENU, so if you do not see your game ... something is broken in it.

### Known Issues/Limitations
- Encrypted PBP files are not loaded by PCSX ... you can only use files created using Popstation/PSX2PSP software
- Some games may not be properly discovered or have missing covers
- To enable usage of shared Memory cards you need to run a game at least once before switching cards
- Shared cards can not be edited using SonyUI manager... The game selector shows content of Internal card all the time


## Road Map
This is just rough ideas of milestone ideas

no milestones set at the moment as we decided just look on what people use the most and implement it :)

