#include "gui_font.h"
#include <iostream>
#include "../util.h"
#include <assert.h>
#include "../DirEntry.h"

using namespace std;

FontSize allFontSizes[] { FONT_15_BOLD, FONT_22_MED, FONT_28_BOLD };
map<FontSize, string> TTF_FileNameToUseForFontSize  {
        {FONT_15_BOLD, "SST-Bold.ttf"},
        {FONT_22_MED, "SST-Medium.ttf"},
        {FONT_28_BOLD, "SST-Bold.ttf"}
};

//********************
// Fonts::Fonts
//********************
Fonts::Fonts() {
    for (auto size : allFontSizes)
        fonts.emplace(size, TTF_Font_Shared()); // insert an empty TTF_Font_Shared pointing to nullptr for now
}

//********************
// Fonts::openNewSharedFont
//********************
TTF_Font_Shared Fonts::openNewSharedFont(const string &filename, int fontSize) {
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
        fonts[size] = openNewSharedFont(dirname + sep + TTF_FileNameToUseForFontSize[size], size);
}
