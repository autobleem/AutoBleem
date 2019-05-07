//
// Created by screemer on 2/8/19.
//

#ifndef AUTOBLEEM_GUI_GUI_LAUNCHER_H
#define AUTOBLEEM_GUI_GUI_LAUNCHER_H

#include "../gui/gui_screen.h"
#include "ps_obj.h"
#include <vector>
#include "ps_settings_back.h"
#include "ps_zoom_btn.h"
#include "ps_meta.h"
#include "ps_game.h"
#include "ps_carousel.h"
#include "ps_move_bnt.h"
#include "ps_menu.h"
#include "ps_centerlabel.h"
#include "ps_stateselector.h"
#include <memory>

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

// Note that if there are less than 13 games in the gamesList the games are duplicated to fill out the carousel.
// So more than one PsCarouselGame could be using the same PsGamePtr.
struct PsCarouselGame : public PsGamePtr {
    PsCarouselGame() = delete;
    PsCarouselGame(PsGamePtr & game) : PsGamePtr(game) { };

    PsScreenpoint current;
    PsScreenpoint destination;
    PsScreenpoint actual;
    int screenPointIndex = -1;
    int nextPointIndex = -1;
    long animationStart = 0;
    long animationDuration = 0;
    bool visible = false;
};

class GuiLauncher : public GuiScreen {
public:
    void init();
    void render();
    void loop();

    void nextGame(int speed);
    void prevGame(int speed);
    void updateMeta();
    void renderText(int x, int y, std::string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font, bool background, bool center);
    void loadAssets();
    void freeAssets();
    void moveMainCover(int state);

    int currentSet=SET_ALL;
    void switchSet(int newSet);
    void showSetNotification();
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

    TTF_Font *font30;
    TTF_Font *font15;
    TTF_Font *font24;

    int fgR=255, fgG=255, fgB=255;
    int secR=100, secG=100, secB=100;

    std::vector<PsObj *> staticElements;
    std::vector<PsObj *> frontElemets;
    std::vector<PsObj *> menuElements;
    std::vector<PsObj *> carousel;

    PsCenterLabel * menuHead;
    PsCenterLabel * menuText;

    std::string gameName;
    std::string publisher;
    std::string year;
    std::string players;

    std::string notificationText;
    long notificationTime=0;
    int numberOfNonDuplicatedGamesInCarousel = 0;
    bool staticMeta=false;

    void showNotification(std::string text);

    bool gameInfoVisible = true;
    bool scrolling = false;
    using GuiScreen::GuiScreen;

    // the carousel of games.
    // Note that if there are less than 13 games in the gamesList the games are duplicated to fill out the carousel.
    // So the same PsGamePtr could be in more than one PsCarouselGame.
    std::vector<PsCarouselGame> carouselGames;

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
};


#endif //AUTOBLEEM_GUI_GUI_LAUNCHER_H
