# AutoBleem - former cbleemsync
BleemSync implementation in C++ that works on PlayStation Classic 
It is relatively safe way to add games to your PlayStation Classic.

## Why this project

This is my personal project not related to BleemSync developers on ModMyClassic group in any way . I started it at the day when lolhack was published, so probably same day when pathartl started his project. Soon BleemSync was released, but I was not happy how it worked at that time. As I wanted some software just to dynamically add  games to the USB and sync them to PlayStation Classic's database I decided to develop this in parallel with original BleemSync. All the features are based on my own personal perspective and ideas.

As BleemSync 0.4.0 was released during my development of first version I leveraged some implementation ideas from original C# code, but all C++ code is written from the scratch.

If you want some new feature, or you find a bug please raise the issue. As I am single developler without any community support it my take some time to sort it out, so any other help welcome. 

## It is stable and is it safe to use

**This is getting stable now ... but ...  use it ONLY if you know what you are doing.** 
In general as it uses same overmounting scripts that BleemSync use it is SAFE TO THE SAME LEVEL. It should not brick you console, but... if you brick it I am not responsible. Your risk.... from the other side, I use this solution for some time and nothing wrong happened.

## Features similar to BleemSync or upgraded

1. Overmounts portions of the PSC's filesystem to safely allow modifications
2. Modifies the stock UI to show added games
3. Uses **GAME NAMES as folders** instead of numbered folders
4. Supports multi-disc games(not tested yet)
5. **Includes offline metadata and cover art databases - no download needed during sync**
6. **Runs on boot of the PlayStation Classic** as it was designed to do so from the beginning
7. Intelligent algorithm is trying to "fix" missing files like .cue, unpack .ecm (currently switched off)
8. Small footprint on USB drive as no .NET Core runtime needed
9. As written in native language for PlayStation Classic it is fast
10. Should work with no issue with multitrack games (I had no problem with it)

## Installation

1. Download the ZIP file from the release page (beta will available soon)
2. Extract the contents to the root of your FAT32 or ext4 formatted USB flash drive
3. Name your flash drive SONY. This is a requirement.
4. Download Cover Repository databases (three separate files for NTSC-U, PAL and NTSC-J)
5. Copy Cover Repository databases into /{Your USB}/cbleemsync

## Automatic Game scanning

To allow AutoBleem to scan your games they need to be structurized in ... wait .. this is not BleemSync. Just take your game folder and drop into Games folder. As soon there is .bin file inside this folder AutoBleem will try to generate all other needed files for you on console boot. So structure can be as follows:

```
Games/
     Resident Evil 2 Leon/
           Resident Evil.bin
     Silent Hill/
            Silent Hill.cue
            Silent Hill.bin
```
AutoBleem will  find all games and create GameData folder, then move all files make it compatible with Sony UI. There are databases attached to this file with all covers and metadata.

## Database files

### General usage of database files

The inital release package do not contain any Cover Repository databases. You have to download them separatelly.  In case you want to make the installation small, do not copy the databases to your USB dongle. In this case AutoBleem will not be able to generate "proper" Game.ini files, or match covers, and you may need to create them manually (that... just sucks) to have nice looking menu...but still AutoBleem will import the game with generic cover and metadata and game will be playable.

Database is split into three separate files by region. If you do not for example plan to play JAP region games, you can install just US or PAL. The reason for that split is the file size. Original cover repository in 226x226 PNG was about 1GB in size. I managed to use a lot of optimisations and finally shrink this into much smaller size, but it still is bit big for a small USB dongle. 

The files are:

NTSC-U coversU.db  ~47MB  - 1320 games

PAL-E  coversP.db  ~88MB  - 2582 games

NTSC-J coversJ.db ~173MB  - 4785 games

So it is up to you to decide which files you inslude into your USB dongle. With all three files AutoBleem should be able to match most of the available games. If the game can not be found, it will still try to use "Generic" image, it will put the same name as the folder in Games and unknown Publisher.

**REMEBER COPY DATABASE FILES TO /cbleemsync/ folder on USB, not any other place**

## Credits and links 

This software uses some parts of code based on open licences:

[BleemSync](https://github.com/pathartl/BleemSync) - original implementation in C# with some different functionality

[SQLite](https://www.sqlite.org/index.html) (sqlite.c / sqlite.h)

UNECM - Copyright (C) 2002 Neill Corlett (GPL) (unecm.c)

## Frequently Asked Questions
### Why not contribute to BleemSync ?

I am not C# developer, but Java and C++... those are totally different languages and I .. just do not like C#. Also BleemSync is developed by ModMyClassic group that I am not member of.

### Can I extend this project ? 
Of course ... it is fully open source using GPL3.0

### Known Issues/Limitations
- Games have to be in cue/bin same as on BleemSync


