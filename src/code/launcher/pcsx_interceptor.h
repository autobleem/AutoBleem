//
// Created by screemer on 2/13/19.
//

#ifndef AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H
#define AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H

#include "ps_game.h"

#define RUNPOINT "/media/AutoBleem/run/"

class PcsxInterceptor {
public:
    bool execute(PsGame *game, int resumepoint);
    void memcardIn(PsGame *game);
    void memcardOut(PsGame *game);
};


#endif //AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H
