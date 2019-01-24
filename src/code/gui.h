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
#include "config.h"

using namespace std;

#define MENU_OPTION_SCAN  1
#define MENU_OPTION_RUN   2
#define MENU_OPTION_SONY  3
#define MENU_OPTION_RETRO 4

#define OCD_ALPHA 170

class Gui {
private:
    Gui() {}


    string themePath;


public:
    Config cfg;

    Inifile themeData;
    void display(bool forceScan);
    void finish();
    void drawText(string text);
    void getEmojiTextTexture(SDL_Renderer *renderer, string text,
                         TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
    void logText(string message);
    void menuSelection();
    void saveSelection();

    void renderBackground();
    int renderLogo(bool small);
    void renderStatus(string text);
    void renderTextBar();
    void renderTextLine(string text, int line, int offset);
    void renderSelectionBox(int line, int offset);


    void getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
                        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);


    int menuOption=MENU_OPTION_SCAN;

    SDL_Rect backgroundRect;
    SDL_Rect logoRect;
    SDL_Renderer *renderer = NULL;

    SDL_Texture *backgroundImg = NULL;
    SDL_Texture *logo = NULL;
    SDL_Texture *buttonX = NULL;
    SDL_Texture *buttonO = NULL;
    SDL_Texture *buttonT = NULL;
    SDL_Texture *buttonS = NULL;
    SDL_Texture *buttonStart = NULL;
    SDL_Texture *buttonSelect = NULL;
    SDL_Texture *buttonR1 = NULL;



    void loadAssets();


    Mix_Music * music;
    TTF_Font *font =  NULL;
    bool forceScan=false;

    Gui(Gui const &) = delete;
    Gui &operator=(Gui const &) = delete;
    static std::shared_ptr<Gui> getInstance() {
        static std::shared_ptr<Gui> s{new Gui};
        return s;
    }
};


#endif //CBLEEMSYNC_SPLASH_H
