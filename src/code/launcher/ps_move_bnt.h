//
// Created by screemer on 2/15/19.
//

#ifndef AUTOBLEEM_GUI_PS_MOVE_BNT_H
#define AUTOBLEEM_GUI_PS_MOVE_BNT_H

#include "ps_obj.h"

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


#endif //AUTOBLEEM_GUI_PS_MOVE_BNT_H
