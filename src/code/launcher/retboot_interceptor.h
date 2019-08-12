//
// Created by screemer on 2019-07-18.
//

#pragma once

#include "ps_game.h"
#include "emu_interceptor.h"

#include <string>

using namespace std;

class RetroArchInterceptor : public EmuInterceptor {
private:
    void backupCoreConfig();
    void restoreCoreConfig();
    void transferConfig(PsGamePtr & game);
public:
    bool execute(PsGamePtr & game, int resumepoint);
    void memcardIn(PsGamePtr & game);
    void memcardOut(PsGamePtr & game);
};



