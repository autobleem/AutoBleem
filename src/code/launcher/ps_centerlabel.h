//
// Created by screemer on 2019-02-21.
//

#pragma once

#include "ps_obj.h"
#include <guigfx/gui.h>

//******************
// PsCenterLabel
//******************
class PsCenterLabel : public PsObj {
public:
    TTF_Font_Shared font;
    SDL_Shared<SDL_Texture> texture;
    int w = 0, h = 0;
 
    void render();

    void setText(const std::string & text, int r, int g, int b);

    PsCenterLabel(SDL_Shared<SDL_Renderer> renderer1, const std::string & name1, const std::string & texPath = "");
    ~PsCenterLabel();

    SDL_Shared<SDL_Texture> createTextTex(const std::string & text, Uint8 r, Uint8 g, Uint8 b, TTF_Font_Shared font);
};
