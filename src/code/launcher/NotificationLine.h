#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

#define TicksPerSecond 1000
#define DefaultShowingTimeout 2 * TicksPerSecond

struct NotificationLine {
    int x = 10;
    int y = 10;
    std::string text = "";
    bool timed = true;
    long notificationTime = 0;  // the tick time when setText was called
    long timeLimit = 0; // display ends when current tick - notificationTime > timeLimit
    SDL_Color textColor =  { 255, 255, 255, 0 };  // brightWhite
    TTF_Font *font = nullptr;

    void setText(std::string _text, bool _timed, long _timeLimit, const SDL_Color & _textColor, TTF_Font *_font);
    void setText(std::string _text, bool _timed, long _timeLimit);

    void tickTock(SDL_Renderer * renderer);
};

struct NotificationLines {
    std::vector<NotificationLine> lines;

    void createAndSetDefaults(int count, int x_start, int y_start, TTF_Font * font, int fontHeight, int separationBetweenLines);
    NotificationLine & operator [] (int i) { return lines[i]; };
};

