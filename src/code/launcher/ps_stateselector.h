//
// Created by screemer on 2019-02-22.
//

#ifndef AUTOBLEEM_GUI_PS_STATESELECTOR_H
#define AUTOBLEEM_GUI_PS_STATESELECTOR_H

#include "ps_obj.h"
#include "ps_game.h"
using namespace std;

#define OP_LOAD 0
#define OP_SAVE 1

class PsStateSelector : public PsObj {
public:
    int operation=0;
    void render();
    void renderText(int x, int y, string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font, bool center);

    void loadSaveStateImages(PsGame * game);
    void freeImages();

    SDL_Texture * frame;

    SDL_Texture *slot1img= nullptr;
    SDL_Texture *slot2img= nullptr;
    SDL_Texture *slot3img= nullptr;
    SDL_Texture *slot4img= nullptr;

    TTF_Font * font30;
    TTF_Font * font24;

    int selSlot = 0;

    using PsObj::PsObj;
};


#endif //AUTOBLEEM_GUI_PS_STATESELECTOR_H
