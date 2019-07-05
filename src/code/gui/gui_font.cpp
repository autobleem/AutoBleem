#include "gui_font.h"
#include <iostream>
#include "../util.h"
#include <assert.h>

using namespace std;

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
        fonts[size] = openFont(dirname + Util::separator() + TTF_FileNameToUseForFontSize[size], size);
}
