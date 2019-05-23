#include "DebugTimer.h"
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include<iostream>

using namespace std;

#ifndef NDEBUG  // if debug build
//*******************************
// DebugTimer::DebugTimer
//*******************************
// to use create a DebugTimer variable passing it the name of the function or other text.
// When the object goes out of scope it will output the time delay that has passed to cout.
//
DebugTimer::DebugTimer(const string & _description) : description(_description) {
    ticks_start = SDL_GetTicks();
    cout << description << ": start timer" << endl;
}

//*******************************
// DebugTimer::~DebugTimer
//*******************************
DebugTimer::~DebugTimer() {
    ticks_end = SDL_GetTicks();
	float time = float(ticks_end - ticks_start) / 1000.0;
    cout << description << ": " << time << " seconds" << endl;
    };
#endif
