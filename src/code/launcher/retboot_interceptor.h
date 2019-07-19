//
// Created by screemer on 2019-07-18.
//

#pragma once

#include "ps_game.h"
#include "emu_interceptor.h"

class RetroArchInterceptor : public EmuInterceptor {
    bool execute(PsGamePtr & game, int resumepoint);
    void memcardIn(PsGamePtr & game);
    void memcardOut(PsGamePtr & game);
};



