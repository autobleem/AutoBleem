//
// Created by screemer on 2019-02-22.
//

#ifndef AUTOBLEEM_GUI_PS_STATESELECTOR_H
#define AUTOBLEEM_GUI_PS_STATESELECTOR_H

#include "ps_obj.h"
using namespace std;

class PsStateSelector : public PsObj {
public:
    void render();

    SDL_Texture * frame;

    using PsObj::PsObj;
};


#endif //AUTOBLEEM_GUI_PS_STATESELECTOR_H
