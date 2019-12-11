//
// Created by screemer on 2019-12-10.
//

#pragma once

#define DefaultShowingTimeoutText "2"

enum ImageType { IMAGE_NO_GAME_FOUND = -1,
    IMAGE_BIN = 0, // must be 0 to match existing game.ini
    IMAGE_PBP = 1, // must be 1 to match existing game.ini
    IMAGE_IMG
//                 IMAGE_ISO // not supported yet
};

const char GAME_DATA[] = "GameData";
const char GAME_INI[] = "Game.ini";
const char PCSX_CFG[] = "pcsx.cfg";
const char EXT_PNG[] = ".png";
const char EXT_PBP[] = ".pbp";
const char EXT_ECM[] = ".ecm";
const char EXT_BIN[] = ".bin";
const char EXT_IMG[] = ".img";
//const char EXT_ISO[] = ".iso";
const char EXT_CUE[] = ".cue";
const char EXT_LIC[] = ".lic";