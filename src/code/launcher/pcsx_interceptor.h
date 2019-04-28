//
// Created by screemer on 2/13/19.
//

#ifndef AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H
#define AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H

#include "ps_game.h"

#define RUNPOINT "/media/AutoBleem/run/"

class PcsxInterceptor {
public:
    bool execute(std::shared_ptr<PsGame> game, int resumepoint);
    void memcardIn(std::shared_ptr<PsGame> game);
    void memcardOut(std::shared_ptr<PsGame> game);
    void prepareResumePoint(std::shared_ptr<PsGame> game, int pointId);
    void saveResumePoint(std::shared_ptr<PsGame> game, int pointId);
};


#endif //AUTOBLEEM_GUI_PCSX_INTERCEPTOR_H
