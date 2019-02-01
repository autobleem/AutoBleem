//
// Created by screemer on 2018-12-19.
//

#ifndef CBLEEMSYNC_SPLASH_H
#define CBLEEMSYNC_SPLASH_H

#include "main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>
#include "database.h"
#include "config.h"

#define PCS_BTN_L1       6
#define PCS_BTN_R1       7
#define PCS_BTN_START    9
#define PCS_BTN_SQUARE   3
#define PCS_BTN_TRIANGLE 0
#define PCS_BTN_CROSS    2
#define PCS_BTN_CIRCLE   1
#define PCS_BTN_SELECT   8


using namespace std;

#define MENU_OPTION_SCAN  1
#define MENU_OPTION_RUN   2
#define MENU_OPTION_SONY  3
#define MENU_OPTION_RETRO 4


class Gui {
private:
    Gui() {}


    string themePath;


public:
    Config cfg;

    Inifile themeData;
    Inifile defaultData;
    void display(bool forceScan, string path, Database * db);
    void waitForGamepad();
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
    void renderTextLine(string text, int line, int offset, bool center);
    void renderTextLine(string text, int line, int offset);
    void renderSelectionBox(int line, int offset);
    void renderLabelBox(int line, int offset);
    void renderTextChar(string text, int line, int offset, int posx);
    void renderFreeSpace();
    void getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
                        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

    Uint8 getR(string val);
    Uint8 getG(string val);
    Uint8 getB(string val);


    SDL_Texture *  loadThemeTexture(SDL_Renderer * renderer, string themePath, string defaultPath, string texname);

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
    SDL_Texture *buttonL1 = NULL;
    SDL_Texture *buttonR1 = NULL;
    SDL_Texture *buttonCheck = NULL;
    SDL_Texture *buttonUncheck = NULL;


    string path;

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
