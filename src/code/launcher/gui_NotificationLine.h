#pragma once

#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include "../gui/gui_font_wrapper.h"
#include "../gui/gui_font.h"

#define TicksPerSecond 1000
#define DefaultShowingTimeout (2 * TicksPerSecond)
#define DefaultShowingTimeoutText "2"

//******************
// NotificationLine
//******************
struct NotificationLine {
    int x = 10;
    int y = 10;
    std::string text;
    bool timed = true;
    long notificationTime = 0;  // the tick time when setText was called.  this is in milliseconds.
    long timeLimit = 0; // display ends when current tick - notificationTime > timeLimit.  this is in milliseconds.
    SDL_Color textColor =  { 255, 255, 255, 0 };  // brightWhite
    FontSize fontSize = FONT_24;

    // timelimit is in milliseconds.  a timeLimit of 0 = no limit.
    void setText(std::string _text, long _timeLimit, const SDL_Color & _textColor, FontSize fontSize);
    void setText(std::string _text, long _timeLimit);

    void tickTock();
};

//******************
// NotificationLines
//******************
struct NotificationLines {
    std::vector<NotificationLine> lines;
    NotificationLine & operator [] (int i) { return lines[i]; };

    void createAndSetDefaults(int count, int x_start, int y_start, FontSize fontSize, int fontHeight, int separationBetweenLines);
    void tickTock();
};

