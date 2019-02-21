//
// Created by screemer on 2019-02-21.
//

#ifndef AUTOBLEEM_GUI_PC_CENTERLABEL_H
#define AUTOBLEEM_GUI_PC_CENTERLABEL_H

#include "ps_obj.h"

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


#endif //AUTOBLEEM_GUI_PC_CENTERLABEL_H
