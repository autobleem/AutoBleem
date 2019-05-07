//
// Created by screemer on 2/13/19.
//

#ifndef AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H
#define AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H

#include "ps_game.h"

#define RUNPOINT "/media/AutoBleem/run/"

class PcsxInterceptor {
public:
    bool execute(PsGamePtr & game, int resumepoint);
    void memcardIn(PsGamePtr & game);
    void memcardOut(PsGamePtr & game);
    void prepareResumePoint(PsGamePtr & game, int pointId);
    void saveResumePoint(PsGamePtr & game, int pointId);
};


#endif //AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H
