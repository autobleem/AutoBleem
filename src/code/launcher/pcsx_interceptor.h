//
// Created by screemer on 2/13/19.
//

#pragma once

#include "ps_game.h"

#define RUNPOINT "/media/AutoBleem/run/"

//******************
// PcsxInterceptor
//******************
class PcsxInterceptor {
public:
    bool execute(PsGamePtr & game, int resumepoint);
    void memcardIn(PsGamePtr & game);
    void memcardOut(PsGamePtr & game);
    void prepareResumePoint(PsGamePtr & game, int pointId);
    void saveResumePoint(PsGamePtr & game, int pointId);
};
