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

#define STATE_GAMES    0
#define STATE_SET      1
#define STATE_RESUME   2
#define STATE_INFO     3

#define SET_ALL      0
#define SET_INTERNAL 1
#define SET_EXTERNAL 2


class GuiLauncher : public GuiScreen {
public:
    void init();
    void render();
    void loop();

    void nextGame(int speed);
    void prevGame(int speed);
    void updateMeta();
    void renderText(int x, int y, string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font, bool background, bool center);
    void loadAssets();
    void freeAssets();
    void moveMainCover(int state);

    int currentSet=SET_ALL;
    void switchSet(int newSet);
    void showSetNotification();



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

    vector<PsObj *> staticElements;
    vector<PsObj *> frontElemets;
    vector<PsObj *> menuElements;
    vector<PsObj *> carousel;

    PsCenterLabel * menuHead;
    PsCenterLabel * menuText;

    string gameName;
    string publisher;
    string year;
    string players;

    string notificationText;
    long notificationTime=0;

    void showNotification(string text);

    bool gameInfoVisible = true;
    bool scrolling = false;
    using GuiScreen::GuiScreen;
    vector<PsGame *> gamesList;
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
