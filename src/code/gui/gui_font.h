#pragma once

#include "gui_font_wrapper.h"
#include <map>

enum FontSize { FONT_15=15, FONT_24=24, FONT_30=30 };

static FontSize allFontSizes[] { FONT_15, FONT_24, FONT_30 };
static std::map<FontSize, std::string> TTF_FileNameToUseForFontSize  {
    {FONT_15, "SST-Bold.ttf"},
    {FONT_24, "SST-Medium.ttf"},
    {FONT_30, "SST-Bold.ttf"}
};

class Fonts {
    std::map<FontSize, TTF_Font_Shared> fonts;

public:
    Fonts() {
        for (auto size : allFontSizes)
            fonts.emplace(size, TTF_Font_Shared()); // insert an empty TTF_Font_Shared pointing to nullptr for now
    }

    // use operator [] to get or set the shared font
    TTF_Font_Shared & operator [] (FontSize size) { return fonts[size]; }

    static TTF_Font_Shared openFont(const std::string &filename, int fontSize);
    void openAllFonts(const std::string &dirname);
};
