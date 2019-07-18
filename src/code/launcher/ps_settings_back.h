//
// Created by screemer on 2/11/19.
//

#pragma once

#include "ps_obj.h"

//******************
// PsSettingsBack
//******************
class PsSettingsBack : public PsObj {
public:
    void setCurLen(int len);
    void update(long time);

    int nextLen = 0;
    int prevLen = 0;
    long animEndTime = 0;
    long animStarted = 0;

    using PsObj::PsObj;
};
