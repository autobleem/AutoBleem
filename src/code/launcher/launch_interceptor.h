//
// Created by screemer on 2019-12-01.
//

#ifndef AUTOBLEEM_GUI_LAUNCH_INTERCEPTOR_H
#define AUTOBLEEM_GUI_LAUNCH_INTERCEPTOR_H

#include "ps_game.h"
#include "emu_interceptor.h"

#include <string>

using namespace std;

class LaunchInterceptor : public EmuInterceptor {
private:

public:
    bool execute(PsGamePtr & game, int resumepoint);


};


#endif //AUTOBLEEM_GUI_LAUNCH_INTERCEPTOR_H
