#include "gui_font.h"
#include <iostream>
#include <autobleem/autobleemui.h>
#include <assert.h>


using namespace std;

FontSize allFontSizes[] { FONT_15, FONT_24, FONT_30 };
map<FontSize, string> TTF_FileNameToUseForFontSize  {
        {FONT_15, "SST-Bold.ttf"},
        {FONT_24, "SST-Medium.ttf"},
        {FONT_30, "SST-Bold.ttf"}
};

//********************
// Fonts::Fonts
//********************
Fonts::Fonts() {
    for (auto size : allFontSizes)
        fonts.emplace(size, TTF_Font_Shared()); // insert an empty TTF_Font_Shared pointing to nullptr for now
}

//********************
// Fonts::openFont
//********************
TTF_Font_Shared Fonts::openFont(const string &filename, int fontSize) {
    TTF_Font_Shared font = TTF_Font_Shared(TTF_OpenFont(filename.c_str(), fontSize));
    if (font) {
        cout << "Success opening font " << filename << " of size " << fontSize << endl;
    } else {
        cout << "FAILURE opening font " << filename << " of size " << fontSize << endl;
        font = nullptr;
        assert(false);
    }

    return font;
}

//********************
// Fonts::openAllFonts
//********************
void Fonts::openAllFonts(const std::string &dirname) {
    for (auto size : allFontSizes)
        fonts[size] = openFont(dirname + sep + TTF_FileNameToUseForFontSize[size], size);
}
