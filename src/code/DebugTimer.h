#pragma once

#include <string>

#ifndef NDEBUG  // if debug build
//******************
// DebugTimer
//******************
// to use create a DebugTimer variable passing it the name of the function or other text.
// When the object goes out of scope it will output the time delay that has passed to cout.
struct DebugTimer {
    std::string description = "";
    uint32_t ticks_start = 0;
    uint32_t ticks_end = 0;
	
    DebugTimer(const std::string & _description);
    ~DebugTimer();
};
#else // release build
struct DebugTimer {
    DebugTimer(const std::string & _description = "") {};
};
#endif
