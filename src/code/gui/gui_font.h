#pragma once

#include "gui_font_wrapper.h"
#include <map>

enum FontEnum {
    FONT_15_BOLD,
    FONT_20_BOLD,
    FONT_22_MED,
    FONT_28_BOLD,
};
enum FontType { FONT_MED, FONT_BOLD };

class Fonts {
    std::string rootPath;
    std::string medPath;
    std::string boldPath;

    struct FontInfo {
        FontEnum    fontEnum;
        int size;
        FontType fontType;
    };

    static FontInfo allFontInfos[];

    std::map<FontEnum, TTF_Font_Shared> fonts;
    std::map<FontEnum, FontInfo> fontInfos;

public:
    Fonts();

    // use operator [] to get or set the shared font
    TTF_Font_Shared & operator [] (FontEnum size) { return fonts[size]; }

    static TTF_Font_Shared openNewSharedFont(const std::string &filename, int fontSize);
    // in gui_launcher.cpp this call is used to change all the fonts to use the fonts in the current theme
    void openAllFonts(const std::string &_rootPath);
};
