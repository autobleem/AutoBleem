//
// Created by screemer on 2/11/19.
//

#ifndef AUTOBLEEM_GUI_PS_FOOTER_H
#define AUTOBLEEM_GUI_PS_FOOTER_H

#include "ps_obj.h"

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


#endif //AUTOBLEEM_GUI_PS_FOOTER_H
