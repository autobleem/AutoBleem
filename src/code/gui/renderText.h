#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

static const SDL_Color brightWhite = { 255, 255, 255, 0 };

//********************
// global ::renderText
//********************
void renderText(SDL_Renderer * renderer, int x, int y, const std::string & text, const SDL_Color & textColor, TTF_Font *font, bool center, bool background);
