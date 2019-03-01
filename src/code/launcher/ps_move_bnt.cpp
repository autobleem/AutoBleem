//
// Created by screemer on 2/15/19.
//

#include "ps_move_bnt.h"


void PsMoveBtn::update(long time) {
    int duration = 500;

    float progress = time - animationStarted;

    progress = progress / (duration * 1.0f);
    if (progress > 1) progress = 1;
    if (progress < 0) progress = 0;

    if (!up) {
        y = originaly + maxMove * progress;

    } else {
        y = (originaly + maxMove) - maxMove * progress;
    }


    if (progress >= 1.0f) {
        up = !up;

        animationStarted = time;
    }

    lastTime = time;
}