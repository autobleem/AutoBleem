//
// Created by screemer on 2019-07-30.
//

#pragma once

#include "../gui/gui_screen.h"
#include "../engine/cardedit.h"
#include "../gui/gui_font_wrapper.h"
#include <string>

using namespace std;

class GuiMcManager : public GuiScreen {
public:
    SDL_Shared<SDL_Texture> backgroundImg;
    string card1path;
    string card2path;
    void render();
    void loop();
    void init();

    void loadAssets();
    using GuiScreen::GuiScreen;

private:
    TTF_Font_Shared fontJIS;
    void renderStatic();
    void renderPencil(int memcard, int row, int column);
    void renderMemCardIcons(int memcard);
    void renderMetaInfo();
    CardEdit * memcard1, *memcard2;
    const int mc1XStart = 150;
    const int mc2XStart = 1010;
    const int mcYStart = 150;

    int pencilColumn;
    int pencilRow;
    int pencilMemcard;
    int oldcol;
    int oldline;
    int oldmc;
    SDL_Rect pencilPos;
    SDL_Shared<SDL_Texture> mcGrid;
    SDL_Shared<SDL_Texture> mcPencil;
};
