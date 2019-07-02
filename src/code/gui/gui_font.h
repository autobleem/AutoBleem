#pragma once

#include "gui_font_wrapper.h"
#include <map>

enum FontSize { FONT_15=15, FONT_24=24, FONT_30=30 };

static FontSize allFontSizes[] { FONT_15, FONT_24, FONT_30 };

class Fonts {
    std::map<FontSize, TTF_Font_Shared> fonts;

public:
    Fonts() {
        for (auto size : allFontSizes)
            fonts.emplace(size, TTF_Font_Shared()); // insert an empty font* for now
    }

    void setFont(FontSize size, TTF_Font_Shared font) {
        fonts[size] = font;
    }

    TTF_Font_Shared getFont(FontSize size) {
        return fonts[size];
    }

    TTF_Font_Shared & operator [] (FontSize size) { return fonts[size]; }
};
