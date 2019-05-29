//
// Created by screemer on 2018-12-19.
//
#pragma once

#include <string>
#include "../engine/config.h"
#include "../engine/coverdb.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define PCS_DEADZONE     32000
#define PCS_BTN_L2       4
#define PCS_BTN_R2       5
#define PCS_BTN_L1       6
#define PCS_BTN_R1       7
#define PCS_BTN_START    9
#define PCS_BTN_SQUARE   3
#define PCS_BTN_TRIANGLE 0
#define PCS_BTN_CROSS    2
#define PCS_BTN_CIRCLE   1
#define PCS_BTN_SELECT   8

#define MENU_OPTION_SCAN  1
#define MENU_OPTION_RUN   2
#define MENU_OPTION_SONY  3
#define MENU_OPTION_RETRO 4
#define MENU_OPTION_START 5
#define MENU_OPTION_POWER 6

//********************
// GuiBase
//********************
struct GuiBase {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    TTF_Font *font30 = nullptr;
    TTF_Font *font15 = nullptr;
    TTF_Font *font24 = nullptr;
    Config cfg;

    GuiBase();
    ~GuiBase();

    std::string getSonyImagePath();
    std::string getSonyFontPath();
    std::string getSonySoundPath();
    std::string getSonyRootPath();

};

//********************
// Gui
//********************
class Gui : public GuiBase {
private:
    Gui() {}
    std::string themePath;

public:
    std::vector<SDL_Joystick*> joysticks;

    Coverdb *coverdb;
    Database *db;
    Inifile themeData;
    Inifile defaultData;

    void watchJoystickPort();

    void loadAssets();
    void display(bool forceScan, std::string path, Database *db, bool resume);
    void waitForGamepad();
    void finish();
    void drawText(std::string text);
    void getEmojiTextTexture(SDL_Renderer *renderer, std::string text,
                         TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
    void logText(std::string message);
    void menuSelection();
    void saveSelection();
    bool quickBoot();
    void renderBackground();
    int renderLogo(bool small);
    void renderStatus(std::string text);
    void renderTextBar();
    int renderTextLine(std::string text, int line, int offset, bool center, int xoffset);
    int renderTextLine(std::string text, int line, int offset, bool center);
    int renderTextLine(std::string text, int line, int offset);
    int renderTextLineOptions(std::string text, int line, int offset, bool center);
    int renderTextLineOptions(std::string text, int line, int offset, bool center, int xoffset);
    void renderSelectionBox(int line, int offset);
    void renderSelectionBox(int line, int offset, int xoffset);
    void renderLabelBox(int line, int offset);
    void renderTextChar(std::string text, int line, int offset, int posx);
    void renderFreeSpace();
    void getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
                        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

    Uint8 getR(std::string val);
    Uint8 getG(std::string val);
    Uint8 getB(std::string val);

    void criticalException(std::string text);
    SDL_Texture *  loadThemeTexture(SDL_Renderer * renderer, std::string themePath, std::string defaultPath, std::string texname);

    int menuOption=MENU_OPTION_SCAN;
    int lastSet = 0;

    SDL_Rect backgroundRect;
    SDL_Rect logoRect;

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
    SDL_Texture *buttonL2 = NULL;
    SDL_Texture *buttonR2 = NULL;
    SDL_Texture *buttonCheck = NULL;
    SDL_Texture *buttonUncheck = NULL;
    SDL_Texture *cdJewel = NULL;

    bool overrideQuickBoot = false;

    std::string path;

    Mix_Music * music = NULL;
    TTF_Font *font =  NULL;
    bool forceScan=false;

    Mix_Chunk *cancel = NULL;
    Mix_Chunk *cursor = NULL;
    Mix_Chunk *home_down = NULL;
    Mix_Chunk *home_up = NULL;
    Mix_Chunk *resume = NULL;

    bool startingGame = false;
    bool resumingGui = false;
    int lastSelIndex = 0;
    PsGamePtr runningGame;
    int resumepoint=-1;

    Gui(Gui const &) = delete;
    Gui &operator=(Gui const &) = delete;
    static std::shared_ptr<Gui> getInstance() {
        static std::shared_ptr<Gui> s{new Gui};
        return s;
    }
};
