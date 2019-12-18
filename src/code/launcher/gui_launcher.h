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
#include "../gui/gui.h"

#define STATE_GAMES    0
#define STATE_SET      1
#define STATE_RESUME   2
#define STATE_INFO     3

extern const SDL_Color brightWhite;

//******************
// GuiLauncher
//******************
class GuiLauncher : public GuiScreen {
public:
    void init();
    ~GuiLauncher();
    void render();

    // these variables are used by the loop routines
    bool menuVisible = true;
    long motionStart = 0;
    long timespeed = 0;
    int motionDir = 0;
    vector<string> headers;
    vector<string> texts;
    long time = 0;
    SDL_Event e;

    void loop();

    void loop_joyMoveLeft();
    void loop_joyMoveRight();
    void loop_joyMoveUp();
    void loop_joyMoveDown();

    // a button is pressed
    void loop_joyButtonPressed();
    void loop_chooseGameDir();
    void loop_chooseRAPlaylist();
    void loop_selectButtonPressed();
    void loop_circleButtonPressed();
    void loop_triangleButtonPressed();
    void loop_squareButtonPressed();
    void loop_crossButtonPressed();
    void loop_crossButtonPressed_STATE_GAMES();
    void loop_crossButtonPressed_STATE_SET();
    void loop_crossButtonPressed_STATE_SET__OPT_AB_SETTINGS();
    void loop_crossButtonPressed_STATE_SET__OPT_EDIT_GAME_SETTINGS();
    void loop_crossButtonPressed_STATE_SET__OPT_EDIT_MEMCARD();
    void loop_crossButtonPressed_STATE_SET__OPT_RESUME_FROM_SAVESTATE();
    void loop_crossButtonPressed_STATE_RESUME();

    // a button is released
    void loop_joyButtonReleased();
    void loop_prevGameFirstLetter();
    void loop_nextGameFirstLetter();

    void nextCarouselGame(int speed);
    void prevCarouselGame(int speed);
    void updateMeta();
    void loadAssets();
    void freeAssets();
    void moveMainCover(int state);

    shared_ptr<Gui> gui;

    int currentSet = SET_PS1;
    int currentPS1_SelectState = SET_PS1_All_Games;    // SET_PS1_All_Games, SET_PS1_Internal_Only, SET_PS1_Favorites, SET_PS1_Games_Subdir
    void switchSet(int newSet, bool noForce);
    void showSetName();

    void getGames_SET_FAVORITE(PsGames* gamesList);
    void getGames_SET_SUBDIR(int rowIndex, PsGames* gamesList);
    void getGames_SET_RETROARCH(const std::string& playlistName, PsGames *gamesList);
    void getGames_SET_APPS(PsGames* gamesList);
    void appendGames_SET_INTERNAL(PsGames* gamesList);

    // current USB Game Dir
    int currentUSBGameDirIndex = 0;
    std::string currentUSBGameDirName = "";
    
    // current RetroArch Playlist
    int currentRAPlaylistIndex = 0;
    std::string currentRAPlaylistName = "";

    NotificationLines notificationLines; // top two lines of the screen
    int numberOfNonDuplicatedGamesInCarousel = 0;

    static void renderText(int x, int y, const std::string & text, const SDL_Color & textColor,
                           TTF_Font_Shared font, int position, bool background);

    bool powerOffShift=false;

    PsCarousel carouselPositions;

    int len = 100;

    PsSettingsBack *settingsBack;
    PsObj *playButton;
    PsZoomBtn *playText;
    PsMeta *meta;

    PsObj *background;
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

    bool staticMeta=false;
    bool gameInfoVisible = true;
    bool scrolling = false;
    using GuiScreen::GuiScreen;

    // the carousel of games.
    // Note that if there are less than 13 games in the gamesList the games are duplicated to fill out the carousel.
    // So the same PsGamePtr could be in more than one PsCarouselGame.
    std::vector<PsCarouselGame> carouselGames;
    std::vector<std::string> raPlaylists;

    std::shared_ptr<RAIntegrator> raIntegrator;

    int selGameIndex = 0;
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
