//
// Created by screemer on 2/11/19.
//
#pragma once

#include "ps_obj.h"

//******************
// PsZoomBtn
//******************
class PsZoomBtn : public PsObj {

    float zoom = 1.0f;
    float maxZoom = 1.20f;
    float zoomStep = 0.0002f;
    bool up = true;

    void update(long time);

    using PsObj::PsObj;
};
