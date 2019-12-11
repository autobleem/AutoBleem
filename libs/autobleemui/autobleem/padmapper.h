//
// Created by screemer on 2019-03-21.
//
#pragma once

#include "enums.h"
#include "inifile.h"
#include <SDL2/SDL_events.h>
#include <map>


using namespace std;

#define PCS_BTN_L2       4
#define PCS_BTN_R2       5
#define PCS_BTN_L1       6
#define PCS_BTN_R1       7
#define PCS_BTN_START    9
#define PCS_BTN_SQUARE   3
#define PCS_BTN_TRIANGLE 0
#define PCS_BTN_CROSS    2
#define PCS_BTN_CIRCLE   1
#define PCS_BTN_SELECT   8

#define DIR_UP    1
#define DIR_DOWN  2
#define DIR_LEFT  3
#define DIR_RIGHT 4

//******************
// PadMapper
//******************
class PadMapper {
public:
    // loads all mapping files
    void init();

    map<string,Inifile*> configs;
    map<int, string> buttonNames;
    int translateButton(int button, SDL_Event* event);
    bool isUp(SDL_Event* event);
    bool isDown(SDL_Event* event);
    bool isLeft(SDL_Event* event);
    bool isRight(SDL_Event* event);
    bool isCenter(SDL_Event* event);
    bool isDirection(SDL_Event* e,  int dir);
    bool isKnownPad(int id);
    string getMappingString(int id);
    void reload();
    Inifile * defaultConfig;

};
