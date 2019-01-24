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

class GuiRender {
private:
    GuiRender() {}


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
   // void aboutBox();
    void options();
    void redrawOptions();

    void getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
                        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

    int selOption=0;
    int menuOption=MENU_OPTION_SCAN;

    SDL_Rect texr;
    SDL_Rect logor;
    SDL_Renderer *renderer = NULL;

    SDL_Texture *img = NULL;
    SDL_Texture *logo = NULL;
    SDL_Texture *buttonX = NULL;
    SDL_Texture *buttonO = NULL;
    SDL_Texture *buttonT = NULL;
    SDL_Texture *buttonS = NULL;
    SDL_Texture *buttonStart = NULL;
    SDL_Texture *buttonSelect = NULL;
    SDL_Texture *buttonR1 = NULL;

    vector<string> themes;
    vector<string> sthemes;
    vector<string> pcsx;
    vector<string> mip;
    vector<string> nomusic;
    vector<string> autoregion;


    void loadAssets();
    string getOption(vector<string> list, string current, bool next);
    void renderMenuOption(string text, int line, SDL_Rect rect2, SDL_Rect logoRect, SDL_Rect textRec);


    Mix_Music * music;
    TTF_Font *Sans =  NULL;
    bool forceScan=false;
    GuiRender(GuiRender const &) = delete;
    GuiRender &operator=(GuiRender const &) = delete;
    static std::shared_ptr<GuiRender> getInstance() {
        static std::shared_ptr<GuiRender> s{new GuiRender};
        return s;
    }
};


#endif //CBLEEMSYNC_SPLASH_H
