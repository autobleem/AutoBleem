//
// Created by screemer on 2019-07-19.
//

#pragma once

#include "ps_game.h"

class EmuInterceptor {
public:
    virtual ~EmuInterceptor() {}

    virtual bool execute(PsGamePtr & game, int resumepoint){return false;};
    virtual void memcardIn(PsGamePtr & game){};
    virtual void memcardOut(PsGamePtr & game){};
    virtual void prepareResumePoint(PsGamePtr & game, int pointId){};
    virtual void saveResumePoint(PsGamePtr & game, int pointId){};
};


