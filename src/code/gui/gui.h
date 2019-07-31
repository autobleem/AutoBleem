//
// Created by screemer on 2018-12-19.
//
#pragma once

#include "../main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>
#include "../engine/database.h"
#include "../engine/config.h"
#include "../engine/coverdb.h"
#include "../engine/scanner.h"
#include "../engine/padmapper.h"
#include "gui_sdl_wrapper.h"
//#include "gui_font_wrapper.h"
#include "gui_font.h"

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


#define EMU_PCSX          0
#define EMU_RETROARCH     1

#define POS_LEFT 0
#define POS_CENTER 1
#define POS_RIGHT 2
//********************
// GuiBase
//********************
class GuiBase {
public:
    SDL_Shared<SDL_Window> window;
    SDL_Shared<SDL_Renderer> renderer;

    Fonts fonts;

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

    Gui() { mapper.init(); }

    string themePath;

public:
    std::vector<SDL_Joystick *> joysticks;

    int _cb(int button, SDL_Event *e);

    vector<string> joynames;
    PadMapper mapper;

    Coverdb *coverdb = nullptr;
    Database *db = nullptr;
    Inifile themeData;
    Inifile defaultData;

    void watchJoystickPort();

    void loadAssets();

    void display(bool forceScan, std::string path, Database *db, bool resume);

    void waitForGamepad();

    void finish();

    void drawText(const std::string & text);

    void getEmojiTextTexture(SDL_Shared<SDL_Renderer> renderer, std::string text,
                             TTF_Font_Shared font, SDL_Shared<SDL_Texture> *texture, SDL_Rect *rect);

    void logText(const std::string & message);

    void menuSelection();

    void saveSelection();

    bool quickBoot();

    void renderBackground();

    int renderLogo(bool small);

    void renderStatus(const std::string & text);

    void renderTextBar();

    int renderTextLine(const std::string & text, int line, int offset,  int position, int xoffset);
    int renderTextLine(const std::string & text, int line, int offset, int position, int xoffset, TTF_Font_Shared font);

    int renderTextLine(const std::string & text, int line, int offset,  int position);

    int renderTextLine(const std::string & text, int line, int offset);

    int renderTextLineOptions(const std::string & text, int line, int offset,  int position);

    int renderTextLineOptions(const std::string & text, int line, int offset,  int position, int xoffset);

    void renderSelectionBox(int line, int offset);

    void renderSelectionBox(int line, int offset, int xoffset);

    void renderLabelBox(int line, int offset);

    void renderTextChar(const std::string & text, int line, int offset, int posx);

    void renderFreeSpace();

    void getTextAndRect(SDL_Shared<SDL_Renderer> renderer, int x, int y, const char *text,
                        TTF_Font_Shared font, SDL_Shared<SDL_Texture> *texture, SDL_Rect *rect);

    Uint8 getR(const std::string & val);

    Uint8 getG(const std::string & val);

    Uint8 getB(const std::string & val);

    void criticalException(const std::string & text);

    SDL_Shared<SDL_Texture>
    loadThemeTexture(SDL_Shared<SDL_Renderer> renderer, std::string themePath, std::string defaultPath, std::string texname);

    void exportDBToRetroarch();

    int menuOption = MENU_OPTION_SCAN;
    int lastSet = 0;

    SDL_Rect backgroundRect;
    SDL_Rect logoRect;

    SDL_Shared<SDL_Texture> backgroundImg;
    SDL_Shared<SDL_Texture> logo;
    SDL_Shared<SDL_Texture> buttonX;
    SDL_Shared<SDL_Texture> buttonO;
    SDL_Shared<SDL_Texture> buttonT;
    SDL_Shared<SDL_Texture> buttonS;
    SDL_Shared<SDL_Texture> buttonStart;
    SDL_Shared<SDL_Texture> buttonSelect;
    SDL_Shared<SDL_Texture> buttonL1;
    SDL_Shared<SDL_Texture> buttonR1;
    SDL_Shared<SDL_Texture> buttonL2;
    SDL_Shared<SDL_Texture> buttonR2;
    SDL_Shared<SDL_Texture> buttonCheck;
    SDL_Shared<SDL_Texture> buttonUncheck;
    SDL_Shared<SDL_Texture> cdJewel;


    bool overrideQuickBoot = false;

    std::string path;

    Mix_Music *music = nullptr;
    TTF_Font_Shared themeFont;
    bool forceScan = false;

    Mix_Chunk *cancel = nullptr;
    Mix_Chunk *cursor = nullptr;
    Mix_Chunk *home_down = nullptr;
    Mix_Chunk *home_up = nullptr;
    Mix_Chunk *resume = nullptr;

    bool startingGame = false;
    bool resumingGui = false;
    int lastSelIndex = 0;
    string lastPlaylist = "";
    PsGamePtr runningGame;
    int emuMode = EMU_PCSX;
    int resumepoint = -1;
    string padMapping;



    Gui(Gui const &) = delete;

    Gui &operator=(Gui const &) = delete;

    static std::shared_ptr<Gui> getInstance() {
        static std::shared_ptr<Gui> s{new Gui};
        return s;
    }

    static bool sortByTitle(const PsGamePtr &i, const PsGamePtr &j) { return SortByCaseInsensitive(i->title, j->title); }
};
