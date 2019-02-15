//
// Created by screemer on 2/15/19.
//

#include "ps_move_bnt.h"


void PsMoveBtn::update(long time) {
    long timeSinceLastFrame = time - lastTime;

    if (!up) {
        y = y + (timeSinceLastFrame / 20.0f);

    } else {
        y = y - (timeSinceLastFrame / 20.0f);
    }


    if (y >= originaly + maxMove) {
        up = true;
        y = originaly + maxMove;
    }
    if (y <= originaly) {
        up = false;
        y = originaly;
    }

    lastTime = time;
}