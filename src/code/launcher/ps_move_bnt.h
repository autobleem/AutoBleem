//
// Created by screemer on 2/15/19.
//

#pragma once

#include "ps_obj.h"

//******************
// PsMoveBtn
//******************
class PsMoveBtn : public PsObj {
public:
    float moveStep = 4;
    int maxMove = 15;
    int originaly = 0;
    bool up = false;
    int animationStarted = 0;

    void update(long time);

    using PsObj::PsObj;
};
