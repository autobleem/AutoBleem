//
// Created by screemer on 2/13/19.
//

#pragma once

#include "ps_game.h"
#include "emu_interceptor.h"


//******************
// PcsxInterceptor
//******************
class PcsxInterceptor : public EmuInterceptor{
public:
    bool execute(PsGamePtr & game, int resumepoint);
    void memcardIn(PsGamePtr & game);
    void memcardOut(PsGamePtr & game);
    void prepareResumePoint(PsGamePtr & game, int pointId);
    void saveResumePoint(PsGamePtr & game, int pointId);
    void cleanupConfig(PsGamePtr & game);
};
