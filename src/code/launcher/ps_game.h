//
// Created by screemer on 2/12/19.
//

#ifndef AUTOBLEEM_GUI_PS_GAME_H
#define AUTOBLEEM_GUI_PS_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <string>

using namespace std;

class PsGame {
public:
    string title;
    string publisher;
    int year;
    int players;

    string folder;
    string ssFolder;

    string base;
    SDL_Texture *coverPng = nullptr;

    void loadTex(SDL_Renderer *renderer);

    void freeTex();

};


#endif //AUTOBLEEM_GUI_PS_GAME_H
