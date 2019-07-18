//
// Created by screemer on 2019-02-22.
//

#pragma once

#include "ps_obj.h"
#include "ps_game.h"
#include <SDL2/SDL_ttf.h>

#define OP_LOAD 0
#define OP_SAVE 1

//******************
// PsStateSelector
//******************
class PsStateSelector : public PsObj {
public:
    int operation=0;
    void render();

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
