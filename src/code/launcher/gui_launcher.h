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

class GuiLauncher : public GuiScreen{
public:
    void init();
    void render();
    void loop();

    void renderMeta();

    void renderText(int x, int y, string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font);

    void loadAssets();

    void freeAssets();

    int len = 100;

    PsSettingsBack *settingsBack;
    PsObj *playButton;
    PsZoomBtn *playText;
    PsMeta *meta;

    TTF_Font *font30;
    TTF_Font *font15;
    TTF_Font *font24;

    vector<PsObj *> staticElements;
    vector<PsObj *> menuElements;
    vector<PsObj *> carousel;


    string gameName;
    string publisher;
    string year;
    string players;

    bool gameInfoVisible = true;

    using GuiScreen::GuiScreen;


};


#endif //AUTOBLEEM_GUI_GUI_LAUNCHER_H
