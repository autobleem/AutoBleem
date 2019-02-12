//
// Created by screemer on 2/11/19.
//

#ifndef AUTOBLEEM_GUI_PS_ZOOM_BTN_H
#define AUTOBLEEM_GUI_PS_ZOOM_BTN_H

#include "ps_obj.h"

class PsZoomBtn : public PsObj {

    float zoom = 1.0f;
    float maxZoom = 1.20f;
    float zoomStep = 0.0002f;
    bool up = true;

    void update(long time);

    using PsObj::PsObj;

};


#endif //AUTOBLEEM_GUI_PS_ZOOM_BTN_H
