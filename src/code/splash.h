//
// Created by screemer on 2018-12-19.
//

#ifndef CBLEEMSYNC_SPLASH_H
#define CBLEEMSYNC_SPLASH_H

#include "main.h"

#ifndef NO_GUI
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#endif

#include <memory>

using namespace std;

#define MENU_OPTION_SCAN  1
#define MENU_OPTION_RUN   2
#define MENU_OPTION_SONY  3
#define MENU_OPTION_RETRO 4

class Splash {
private:
    Splash() {}
#ifndef NO_GUI
    void getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
            TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
#endif
public:
    void display(bool forceScan);
    void finish();
    void drawText(string text);
    void logText(string message);
    void menuSelection();
    void saveSelection();
    int menuOption=MENU_OPTION_SCAN;
#ifndef NO_GUI
    SDL_Rect texr;
    SDL_Rect logor;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *img = NULL;
    SDL_Texture *logo = NULL;
    TTF_Font *Sans = TTF_OpenFont("Sans.ttf", 24);
#endif
    bool forceScan=false;
    Splash(Splash const &) = delete;
    Splash &operator=(Splash const &) = delete;
    static std::shared_ptr<Splash> getInstance() {
        static std::shared_ptr<Splash> s{new Splash};
        return s;
    }
};


#endif //CBLEEMSYNC_SPLASH_H
