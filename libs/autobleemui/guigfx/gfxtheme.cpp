//
// Created by screemer on 2019-12-12.
//

#include <autobleem/environment.h>
#include <autobleem/direntry.h>
#include "gfxtheme.h"
#include "gfx.h"

string GfxTheme::themePath;
Inifile GfxTheme::themeData;
Inifile GfxTheme::defaultData;

GfxImage  GfxTheme::backgroundImg;
GfxImage  GfxTheme::logo;
GfxImage  GfxTheme::buttonX;
GfxImage  GfxTheme::buttonO;
GfxImage  GfxTheme::buttonT;
GfxImage  GfxTheme::buttonS;
GfxImage  GfxTheme::buttonStart;
GfxImage  GfxTheme::buttonSelect;
GfxImage  GfxTheme::buttonL1;
GfxImage  GfxTheme::buttonR1;
GfxImage  GfxTheme::buttonL2;
GfxImage  GfxTheme::buttonR2;
GfxImage  GfxTheme::buttonCheck;
GfxImage  GfxTheme::buttonUncheck;

GfxImage
GfxTheme::loadThemeTexture(string themePath, string defaultPath, string texname) {
    GfxImage  tex = nullptr;
    if (DirEntry::exists(themePath + themeData.values[texname])) {
        tex = Gfx::loadImage( (themePath + themeData.values[texname]).c_str());
    } else {
        tex = Gfx::loadImage( (defaultPath + defaultData.values[texname]).c_str());
    }
    return tex;
}

bool GfxTheme::load(string currentThemePath)
{
    bool result = true;
    themePath = currentThemePath;
    string defaultPath = Env::getPathToThemesDir() + sep + "default" + sep;
    themePath = currentThemePath + sep;

    if (!DirEntry::exists(themePath + "theme.ini"))
    {
        themePath=defaultPath;
        result = false;
    }
    themeData.load(defaultPath + "theme.ini");
    defaultData.load(defaultPath + "theme.ini");
    themeData.load(themePath + "theme.ini");

    backgroundImg = loadThemeTexture(themePath, defaultPath, "background");
    logo = loadThemeTexture(themePath, defaultPath, "logo");

    buttonO = loadThemeTexture( themePath, defaultPath, "circle");
    buttonX = loadThemeTexture( themePath, defaultPath, "cross");
    buttonT = loadThemeTexture( themePath, defaultPath, "triangle");
    buttonS = loadThemeTexture( themePath, defaultPath, "square");
    buttonSelect = loadThemeTexture( themePath, defaultPath, "select");
    buttonStart = loadThemeTexture( themePath, defaultPath, "start");
    buttonL1 = loadThemeTexture( themePath, defaultPath, "l1");
    buttonR1 = loadThemeTexture( themePath, defaultPath, "r1");
    buttonL2 = loadThemeTexture( themePath, defaultPath, "l2");
    buttonR2 = loadThemeTexture( themePath, defaultPath, "r2");
    buttonCheck = loadThemeTexture(themePath, defaultPath, "check");
    buttonUncheck = loadThemeTexture( themePath, defaultPath, "uncheck");
    return result;
}