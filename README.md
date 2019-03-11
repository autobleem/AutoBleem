

# Version 0.6.0beta released - please update your installations [HERE](https://github.com/screemerpl/cbleemsync/releases)

#### Please read this page before downloading AutoBleem [USB compatibility FAQ](https://github.com/screemerpl/cbleemsync/wiki/USB-compatibility-FAQ)

Watch on YouTube: [Installation Tutorial](https://youtu.be/QRocmHUuc20) 
### This is old tutorial for 0.5.0, but the installation is the same except you already have coverdb's in the package if you downloaded "Full version"

Join our discord server: [https://discord.gg/AHUS3RM](https://discord.gg/AHUS3RM)

#### NOTE: You do not need to uninstall BleemSync 0.4/0.7/1.0/1.0.1 from your console prior to use AutoBleem. Prepare new USB stick with AutoBleem and it will work as expected. 


# AutoBleem 
AutoBleem is a tool to make your PlayStation Classic more usefull.
It is relatively safe way to add games , change look and feel, change advanced configuration of your mini console.

     
## Why this project

This is my personal project not related to BleemSync developers on ModMyClassic group on discord . I started it at the day when lolhack was published, so probably same day when pathartl started his project. Soon BleemSync was released, but I was not fully happy how it worked at that time. I wanted some software just to dynamically add  games to the USB and sync them to PlayStation Classic's database. I decided to develop this in parallel with original BleemSync. All the features are based on my own personal perspective and ideas.

As BleemSync 0.4.0 was released during my development of first version I leveraged some implementation ideas from original C# code, but all C++ code is written from the scratch.

If you want some new feature, or you find a bug please raise the issue. As I am single developler without any community support it my take some time to sort it out, so any other help welcome. 

## Is it stable and is it safe to use ?
Version 0.6.0 is "pretty" stable, but it is BETA - so some elements are not fully finished. Should be more stable than 0.5.0. It is well tested by multiple users, but until 1.0 it is still pre-release. New features may be added over the time, some issues may happen etc.

AutoBleem is safe, it does not write or read any data from internal PSC storage.  It should not brick you console, but... if you brick it I am not responsible. Your risk.... from the other side, I use this solution for some time and nothing wrong happened. 

If you found an issue - report it using "Issues" section on this website or better contact us directly on Discord.

**NOTE: Do not uninstall BleemSync 1.0 if it was installed.. this step is not needed.**

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
1. Intelligent algorithm is trying to "fix" missing files like .cue, unpack .ecm, create folders for games if dropped directly to Games folder
1. Small footprint on USB drive as no .NET Core runtime needed
1. As written in native language for PlayStation Classic it is fast
1. Work with no issue with multitrack games (CUE/BIN) games
1. Supports **CUE/BIN** and **Not encrypted PBP (popstation created - PSN downloads will not work)** file formats
1. Can launch RetroArch sotware **no need to run anything on the PC**
1. Supports all themes of both AutoBleem and Sony menu
1. **configurable filter settings**
1. Supports multi disc games
1. Speaks your language
1. Ability to QuickBoot and change game resolution
1. Ability to scale up the screen to 16:9 modern TV screens
1. Makes your PlayStation Classic a great device

## Installation

1. Download the ZIP file from the release page [HERE](https://github.com/screemerpl/cbleemsync/releases) - you can select one of the packages 
  - autobleem-0.6.0-full - contains all covers
  - autobleem-0.6.0-ntscU/palE/ntscJ - contains only covers for selected region
  - autobleem-0.6.0-clean - no cover databases (they have to be installed manually)
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

The files are:

[DOWNLOAD](https://github.com/screemerpl/cbleemsync/releases/download/v0.5.1/coversU.db) NTSC-U coversU.db  ~70MB  - 1321 games

[DOWNLOAD](https://github.com/screemerpl/cbleemsync/releases/download/v0.5.1/coversP.db) PAL-E  coversP.db  ~88MB  - 2582 games

[DOWNLOAD](https://github.com/screemerpl/cbleemsync/releases/download/v0.5.1/coversJ.db) NTSC-J coversJ.db ~173MB  - 4785 games

So it is up to you to decide which files you inslude into your USB dongle. With all three files AutoBleem should be able to match most of the available games. If the game can not be found, it will still try to use "Generic" image, it will put the same name as the folder in Games and unknown Publisher.

**REMEBER COPY DATABASE FILES TO /Autobleem/bin/db folder on USB, not any other place**

### Custom DB Updates

You can edit cover databases using SQLite Browser software available here: [DOWNLOAD](https://sqlitebrowser.org/) that works on MAC and WINDOWS.

We also created a tool to dump and reload all png files in one shot. The tool is available here [DOWNLOAD](https://github.com/autobleem/abcoverpacker) 

## Credits and links 

This software uses some parts of code based on open licences:

[BleemSync](https://github.com/pathartl/BleemSync) - some parts of bash scripts were reused from early BleemSync code. The credits go to ModMyClassic team (DoctorDalek/pathartl, compscom, Swingflip, cyanic and others).

[SQLite](https://www.sqlite.org/index.html) (sqlite.c / sqlite.h) Public domain - no license

UNECM - Copyright (C) 2002 Neill Corlett (GPLv2) (unecm.c)

## Frequently Asked Questions

### My Console Crashes / Does not load AutoBleem / I can not play games / AutoBleem goes to black screen

READ THIS: [HERE](https://github.com/screemerpl/cbleemsync/releases)

### Why not contribute to BleemSync ?

I am not C# developer, but Java and C++... those are totally different technologies. I will try to support BleemSync as agreed with pathartl, but BleemSync is based on totally different ideas and visions. AutoBleem has to be simple tool and should reduce usage of PC computer to bare minimum.

### Can I extend this project ? 
Of course ... it is fully open source using GPL3.0, also if you know any C++, SQL, Java or even how to make youtube videos talk to us on our Discord and join our team.

### I can not see my game
AutoBleem is analysing the game folder during boot and using smart algorithm it is trying to generate all files needed to run the game, but sometimes it fails. In this case AutoBleem WILL NOT ADD THE GAME to the MENU, so if you do not see your game ... something is broken in it.

### Known Issues/Limitations
- Encrypted PBP files are not loaded by PCSX ... you can only use files created using Popstation/PSX2PSP software
- Some games may not be properly discovered or have missing covers
- To enable usage of shared Memory cards you need to run a game at least once before switching cards
- Shared cards can not be edited using SonyUI manager... The game selector shows content of Internal card all the time


## Road Map
This is just rough ideas of milestone ideas

no milestones set at the moment as we decided just look on what people use the most and implement it :)

