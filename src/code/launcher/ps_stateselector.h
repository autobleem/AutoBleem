//
// Created by screemer on 2019-02-22.
//

#pragma once

#include "ps_obj.h"
#include "ps_game.h"

#define OP_LOAD 0
#define OP_SAVE 1

//******************
// PsStateSelector
//******************
class PsStateSelector : public PsObj {
public:
    int operation=0;
    void render();
    void renderText(int x, int y, std::string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font, bool center);

    void loadSaveStateImages(PsGamePtr & game, bool saving);
    void cleanSaveStateImages();

    void freeImages();

    SDL_Texture * frame;

    SDL_Texture *slotImg[4];
    bool slotActive[4];

    TTF_Font * font30;
    TTF_Font * font24;

    int selSlot = 0;

    using PsObj::PsObj;
};
