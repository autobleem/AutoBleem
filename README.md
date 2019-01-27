# Version 0.5.0 released - please update your installations [HERE](https://github.com/screemerpl/cbleemsync/releases)

Watch on YouTube: [New Features video](https://www.youtube.com/watch?v=NMf_CZsSLJ0&lc=z23dhnxwroyagriif04t1aokg0l3boqp1uvm3xlma41cbk0h00410.1548559704787881)

Watch on YouTube: [Installation Tutorial](https://youtu.be/QRocmHUuc20) 

# AutoBleem 
AutoBleem is a tool to make your PlayStation Classic more usefull.
It is relatively safe way to add games , change look and feel, change advanced configuration of your mini console.

     
## Why this project

This is my personal project not related to BleemSync developers on ModMyClassic group on discord . I started it at the day when lolhack was published, so probably same day when pathartl started his project. Soon BleemSync was released, but I was not fully happy how it worked at that time. I wanted some software just to dynamically add  games to the USB and sync them to PlayStation Classic's database. I decided to develop this in parallel with original BleemSync. All the features are based on my own personal perspective and ideas.

As BleemSync 0.4.0 was released during my development of first version I leveraged some implementation ideas from original C# code, but all C++ code is written from the scratch.

If you want some new feature, or you find a bug please raise the issue. As I am single developler without any community support it my take some time to sort it out, so any other help welcome. 

## Is it stable and is it safe to use ?
Version 0.4 is stable. It is well tested by multiple users. It is still pre-release ... I am planning to make version 0.5 a long time support version as the features seams to be complete then.

AutoBleem general as it uses similar overmounting scripts that BleemSync 0.1-0.4 used, so it is SAFE TO THE SAME LEVEL. It should not brick you console, but... if you brick it I am not responsible. Your risk.... from the other side, I use this solution for some time and nothing wrong happened.

#### NOTE: This tool is made to use only with legal licenced games. The whole project does not use any licenced source code, also it does not alter any of the Sony files inside the mini console, it just uses what is already there and not alter it in any way.

## Features 

1. Integrated simple **Game Manager** (that you can rename games, attach custom memory cards, block autobleem owerwritting game metadata and covers )
1. Integrated functionality to share memory cards between games
1. Separates games and it's save states into separate folders
1. AutoBleem does not need to run anything on the PC. Just copy games to usb dongle and plug it in. 
1. Does not use any internet or usb connection 
1. Overmounts portions of the PSC's filesystem to safely allow PCS modifications
1. Modifies the stock UI to show added games
1. Uses **GAME NAMES as folders** instead of numbered folders
1. Supports multi-disc games
1. **Includes offline metadata and cover art databases - no download needed during sync**
1. **Runs on boot of the PlayStation Classic** as it was designed to do so from the beginning
1. Intelligent algorithm is trying to "fix" missing files like .cue, unpack .ecm 
1. **Nice GUI that replaces need for BootMenu**
1. Small footprint on USB drive as no .NET Core runtime needed
1. As written in native language for PlayStation Classic it is fast
1. Work with no issue with multitrack games (I had no problem with it)
1. Supports **CUE/BIN** and **Not encrypted PBP (popstation created - PSN downloads will not work)** file formats
1. Replaces BootMenu and Bleemsync..intergates with RetroArch sotware **no need to run anything on the PC**
1. Supports themes of both AutoBleem and Sony menu
1. **configurable filter settings**
1. Supports multi disc games
1. Makes your PlayStation Classic a great device

## Installation

1. Download the ZIP file from the release page [HERE](https://github.com/screemerpl/cbleemsync/releases/download/0.5/autobleem-v0.5.0.zip)
1. Extract the contents to the root of your FAT32(recommended) or ext4 formatted USB flash drive
1. Name your flash drive SONY. This is a requirement.
1. Download Cover Repository databases (three separate files for NTSC-U, PAL and NTSC-J)
1. Copy Cover Repository databases into /{Your USB}/Autobleem/bin/db
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
AutoBleem will  find all games and create GameData folder, then move all files make it compatible with Sony UI. There are databases attached to this file with all covers and metadata. Also AutoBleem will create folders to store your save data and configuration.

## Database files

### General usage of database files

The inital release package do not contain any Cover Repository databases. You have to download them separatelly.  In case you want to make the installation small, do not copy the databases to your USB dongle. In this case AutoBleem will not be able to generate "proper" Game.ini files, or match covers, and you may need to create them manually (that... just sucks) to have nice looking menu...but still AutoBleem will import the game with generic cover and metadata and game will be playable.

Database is split into three separate files by region. If you do not for example plan to play JAP region games, you can install just US or PAL. The reason for that split is the file size. Original cover repository in 226x226 PNG was about 1GB in size. I managed to use a lot of optimisations and finally shrink this into much smaller size, but it still is bit big for a small USB dongle. 

The files are:

[DOWNLOAD](https://github.com/screemerpl/cbleemsync/releases/download/v0.2/coversU.db) NTSC-U coversU.db  ~47MB  - 1320 games

[DOWNLOAD](https://github.com/screemerpl/cbleemsync/releases/download/0.1b/coversP.db) PAL-E  coversP.db  ~88MB  - 2582 games

[DOWNLOAD](https://github.com/screemerpl/cbleemsync/releases/download/0.1b/coversJ.db) NTSC-J coversJ.db ~173MB  - 4785 games

So it is up to you to decide which files you inslude into your USB dongle. With all three files AutoBleem should be able to match most of the available games. If the game can not be found, it will still try to use "Generic" image, it will put the same name as the folder in Games and unknown Publisher.

**REMEBER COPY DATABASE FILES TO /cbleemsync/ folder on USB, not any other place**

## Credits and links 

This software uses some parts of code based on open licences:

[BleemSync](https://github.com/pathartl/BleemSync) - similar implementation in C# with some different functionality (no code reused - just idea for some solutions)

[SQLite](https://www.sqlite.org/index.html) (sqlite.c / sqlite.h) Public domain - no license

UNECM - Copyright (C) 2002 Neill Corlett (GPLv2) (unecm.c)

## Frequently Asked Questions
### Why not contribute to BleemSync ?

I am not C# developer, but Java and C++... those are totally different technologies. I will try to support BleemSync as agreed with pathartl

### Can I extend this project ? 
Of course ... it is fully open source using GPL3.0

### I can not see my game
AutoBleem is analysing the game folder during boot and using smart algorithm it is trying to generate all files needed to run the game, but sometimes it fails. In this case AutoBleem WILL NOT ADD THE GAME to the MENU, so if you do not see your game ... something is broken in it.

### Known Issues/Limitations
- Sometimes the wrong region is set by Sony's PCSX in PlaystationClassic (example WipeoutXL) - use GUI to change Bios setting 
- Encrypted PBP files are not loaded by PCSX ... you can only use files created using Popstation/PSX2PSP software
- Try to set compression level to 1 (Worst) in PSX2PSP for quick load times
- Some games may not be properly discovered or have missing covers


## Road Map
     
     
v0.5 - Future (final) release

     - config repository support
     
     - autoconfig high resolution based on repository data
     
     - backup of save games
     
     - all other ideas not yet listed


