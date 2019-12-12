#pragma once


#include <autobleem/inifile.h>
#include "gui_sdl_wrapper.h"

using namespace std;

class GfxTheme {
private:

    static GfxImage loadThemeTexture(string themePath, string defaultPath, string texname);
    static Inifile themeData;

public:
    static GfxImage  backgroundImg;
    static GfxImage  logo;
    static GfxImage  buttonX;
    static GfxImage  buttonO;
    static GfxImage  buttonT;
    static GfxImage  buttonS;
    static GfxImage  buttonStart;
    static GfxImage  buttonSelect;
    static GfxImage  buttonL1;
    static GfxImage  buttonR1;
    static GfxImage  buttonL2;
    static GfxImage  buttonR2;
    static GfxImage  buttonCheck;
    static GfxImage  buttonUncheck;

    static Inifile defaultData;
    static string themePath;


    static bool load(string currentThemePath);
    static string get(string key)
    {
        return themeData.values[key];
    };

};



