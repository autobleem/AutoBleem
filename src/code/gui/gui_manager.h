//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_menu.h"
#include "../lang.h"
#include "../launcher/ps_game.h"

//********************
// GuiManager
//********************
class GuiManager : public GuiMenu {
public:
    GuiManager(SDL_Shared<SDL_Renderer> _renderer) : GuiMenu(_renderer,
                                                             "-=" + _("Game manager - Select game") + "=-") {}

    void init() override;
    void render() override;
    void loop() override { GuiMenu::loop(); }

    std::string statusLine() override;

    void doCircle();
    void doSquare();
    void doTriangle();
    void doCross();

    void doEnter() { doCross(); }
    void doEscape() { doCircle(); }
    void doDelete() { doSquare(); }

    PsGames psGames;
    static int flushCovers(const char *file, const struct stat *sb, int flag, struct FTW *s);
    static bool sortByTitle(PsGamePtr i, PsGamePtr j) { return SortByCaseInsensitive(i->title, j->title); }
};
