//
// Created by screemer on 2019-02-21.
//

#pragma once

#include "ps_obj.h"

//******************
// PsCenterLabel
//******************
class PsCenterLabel : public PsObj {
public:
    TTF_Font * font;
    SDL_Texture* texture=nullptr;
    int w,h;

    void render();

    void setText(string text, int r, int g, int b);

    PsCenterLabel(SDL_Renderer *renderer1, string name1, string texPath = "");
    ~PsCenterLabel();

    SDL_Texture *  createTextTex(string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font);
};
