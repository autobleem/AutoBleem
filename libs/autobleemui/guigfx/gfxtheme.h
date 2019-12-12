#pragma once


#include <autobleem/inifile.h>
#include "gui_sdl_wrapper.h"
#include "gfx.h"
#include "gui_font_wrapper.h"

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

    // theme drawing routines
    static void drawBackground();
    static int drawLogo(bool small);
    static Uint8 getR(const std::string & val);
    static Uint8 getG(const std::string & val);
    static Uint8 getB(const std::string & val);


    static string get(string key)
    {
        return themeData.values[key];
    };

    static void getEmojiTextTexture(string text, TTF_Font_Shared font, GfxImage *texture, SDL_Rect *rect);
    static GfxImage getTextImage( const char *text, TTF_Font_Shared font);
    static GfxSize getTextSize(const char *text, TTF_Font_Shared font);

};



