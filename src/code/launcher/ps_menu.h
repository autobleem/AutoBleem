//
// Created by screemer on 2/16/19.
//

#ifndef AUTOBLEEM_GUI_PS_MENU_H
#define AUTOBLEEM_GUI_PS_MENU_H


#include <SDL2/SDL_system.h>
#include "ps_obj.h"

#define TR_MENUON 0
#define TR_OPTION 1

class PsMenu : public PsObj {
public:
    SDL_Texture *settings;
    SDL_Texture *guide;
    SDL_Texture *memcard;
    SDL_Texture *savestate;

    SDL_Texture *resume= nullptr;



    int x,y,oy,ox;
    int xoff[4] = {0,0,0,0};
    int yoff[4] = {0,0,0,0};

    float optionscales[4] = {1.0f,1.0f,1.0f,1.0f};


    float maxZoom = 1.5;

    int selOption=0;
    int animationStarted=0;
    int targety;
    int duration;
    bool active = false;

    int direction = 0;

    string path;

    void loadAssets();
    void freeAssets();
    void update(long time);
    void render();

    void setResumePic(string picturePath);

    int transition =0;

    PsMenu(SDL_Renderer *renderer1, string name1, string texPath = "");
};


#endif //AUTOBLEEM_GUI_PS_MENU_H
