#pragma once

#include "gui_font_wrapper.h"
#include <map>

enum FontSize { FONT_15_BOLD=15, FONT_22_MED=22, FONT_28_BOLD=28 };

extern FontSize allFontSizes[];
extern std::map<FontSize, std::string> TTF_FileNameToUseForFontSize;

class Fonts {
    std::map<FontSize, TTF_Font_Shared> fonts;

public:
    Fonts();

    // use operator [] to get or set the shared font
    TTF_Font_Shared & operator [] (FontSize size) { return fonts[size]; }

    static TTF_Font_Shared openNewSharedFont(const std::string &filename, int fontSize);
    // in gui_launcher.cpp this call is used to change all the fonts to use the fonts in the current theme
    void openAllFonts(const std::string &dirname);
};
