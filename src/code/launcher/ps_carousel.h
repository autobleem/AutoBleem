//
// Created by screemer on 2/13/19.
//

#ifndef AUTOBLEEM_GUI_PS_CAROUSEL_H
#define AUTOBLEEM_GUI_PS_CAROUSEL_H


#include <vector>

using namespace std;

class PsScreenpoint {
public:
    int x = 0;
    int y = 0;
    float scale = 0.0;
    int shade = 0;
};

class PsCarousel {
public:
    int activeItem = 0;
    int cycleMax = 0;
    int cycleDuration = 0;
    int previousActiveElement = 0;
    vector<PsScreenpoint> coverPositions;

    void initCoverPositions();

    PsScreenpoint createCoverPoint(int x, int shade, int side);


};


#endif //AUTOBLEEM_GUI_PS_CAROUSEL_H
