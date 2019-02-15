//
// Created by screemer on 2/13/19.
//

#ifndef AUTOBLEEM_GUI_PS_CAROUSEL_H
#define AUTOBLEEM_GUI_PS_CAROUSEL_H


#include <vector>

using namespace std;

class PsScreenpoint {
public:
    int x;
    int y;
    float scale;
    int shade;
};

class PsCarousel {
public:
    int activeItem;
    int cycleMax;
    int cycleDuration;
    int previousActiveElement;
    vector<PsScreenpoint> coverPositions;

    void initCoverPositions();

    PsScreenpoint createCoverPoint(int x, int shade, int side);


};


#endif //AUTOBLEEM_GUI_PS_CAROUSEL_H
