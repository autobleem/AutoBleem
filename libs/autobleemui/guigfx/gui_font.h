#pragma once

#include "gui.h"
#include <map>

enum FontSize { FONT_15=15, FONT_24=24, FONT_30=30 };

extern FontSize allFontSizes[];
extern std::map<FontSize, std::string> TTF_FileNameToUseForFontSize;

class Fonts {
    std::map<FontSize, TTF_Font_Shared> fonts;

public:
    Fonts();

    // use operator [] to get or set the shared font
    TTF_Font_Shared & operator [] (FontSize size) { return fonts[size]; }

    static TTF_Font_Shared openFont(const std::string &filename, int fontSize);
    void openAllFonts(const std::string &dirname);
};
