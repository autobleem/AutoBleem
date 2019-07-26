//
// Created by screemer on 2/8/19.
//
#pragma once

#include "gui_NotificationLine.h"
#include "../gui/gui_screen.h"
#include "ps_obj.h"
#include "ps_settings_back.h"
#include "ps_zoom_btn.h"
#include "ps_meta.h"
#include "ps_carousel.h"
#include "ps_move_bnt.h"
#include "ps_menu.h"
#include "ps_centerlabel.h"
#include "ps_stateselector.h"
#include "../main.h"
#include <vector>
#include <memory>
#include "ra_integrator.h"

#define STATE_GAMES    0
#define STATE_SET      1
#define STATE_RESUME   2
#define STATE_INFO     3

// if you add a new set also update setNames in gui_launcher.cpp
#define SET_ALL      0
#define SET_INTERNAL 1
#define SET_EXTERNAL 2
#define SET_FAVORITE 3
#define SET_LAST 3

#define SET_RETROARCH -1

extern const SDL_Color brightWhite;

//******************
// GuiLauncher
//******************
class GuiLauncher : public GuiScreen {
public:
    void init();
    void render();
    void loop();

    void nextGame(int speed);
    void prevGame(int speed);
    void updateMeta();
    void loadAssets();
    void freeAssets();
    void moveMainCover(int state);

    int currentSet=SET_ALL;
    void switchSet(int newSet, bool noForce);
    void showSetName();
    NotificationLines notificationLines; // top two lines of the screen
    int numberOfNonDuplicatedGamesInCarousel = 0;

    static void renderText(int x, int y, const std::string & text, const SDL_Color & textColor,
                           TTF_Font_Shared font, bool center, bool background);

    bool powerOffShift=false;

    PsCarousel carouselPositions;

    int len = 100;

    PsSettingsBack *settingsBack;
    PsObj *playButton;
    PsZoomBtn *playText;
    PsMeta *meta;

    PsMoveBtn *arrow;
    PsObj *xButton;
    PsObj *oButton;
    PsObj *tButton;
    PsMenu *menu;
    PsStateSelector * sselector= nullptr;

    Uint8 fgR=255, fgG=255, fgB=255;
    Uint8 secR=100, secG=100, secB=100;

    std::vector<PsObj *> staticElements;
    std::vector<PsObj *> frontElemets;
    std::vector<PsObj *> menuElements;
    std::vector<PsObj *> carousel;

    PsCenterLabel * menuHead;
    PsCenterLabel * menuText;

    std::string gameName;
    std::string publisher;
    std::string year;
    std::string serial;
    std::string region;
    std::string players;

    std::string retroarch_playlist_name;

    bool staticMeta=false;
    bool gameInfoVisible = true;
    bool scrolling = false;
    using GuiScreen::GuiScreen;

    // the carousel of games.
    // Note that if there are less than 13 games in the gamesList the games are duplicated to fill out the carousel.
    // So the same PsGamePtr could be in more than one PsCarouselGame.
    std::vector<PsCarouselGame> carouselGames;
    std::vector<std::string> raPlaylists;

    RAIntegrator raIntegrator;

    int selGame = 0;
    int state = STATE_GAMES;
    void setInitialPositions(int selected);
    int getPreviousId(int id);
    int getNextId(int id);
    void scrollLeft(int speed);
    void scrollRight(int speed);
    void updatePositions();
    void updateVisibility();
    void switchState(int state, int time);
    void forceSettingsOnly();
    void showAllOptions();

    static bool sortByTitle(const PsGamePtr &i, const PsGamePtr &j) { return SortByCaseInsensitive(i->title, j->title); }
};
