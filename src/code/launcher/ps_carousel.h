//
// Created by screemer on 2/13/19.
//

#ifndef AUTOBLEEM_GUI_PS_CAROUSEL_H
#define AUTOBLEEM_GUI_PS_CAROUSEL_H


#include "ps_game.h"
#include <vector>

using namespace std;

class PsCarousel {
    float ovalWidth;
    float ovalHeight;
    int activeItem;
    float offsetX;
    float offsetY;
    float angle;
    int cycleMax;
    int cycleDuration;
    int previousActiveElement;
    vector<PsGame *> games;
};


#endif //AUTOBLEEM_GUI_PS_CAROUSEL_H
