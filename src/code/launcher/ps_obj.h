//
// Created by screemer on 2/11/19.
//

#ifndef AUTOBLEEM_GUI_PS_OBJ_H
#define AUTOBLEEM_GUI_PS_OBJ_H

#include "../main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

class PsObj {
public:
    SDL_Renderer *renderer;
    int x, y, w, h;
    int ox, oy, ow, oh;

    string name;


    SDL_Texture *tex = nullptr;


    bool visible = false;
    long lastTime = 0;

    PsObj(SDL_Renderer *renderer1, string name1, string texPath = "");

    virtual void load(string imagePath);

    virtual void destroy();

    virtual void update(long time) {};

    virtual void render();

};


#endif //AUTOBLEEM_GUI_PS_OBJ_H
