//
// Created by screemer on 2019-01-24.
//
#pragma once

#include "gui_twoColumnStringMenu.h"
#include "../../lang.h"
#include "../../launcher/ps_game.h"

//********************
// GuiManager
//********************
class GuiManager : public GuiTwoColumnStringMenu {
public:
    GuiManager(SDL_Shared<SDL_Renderer> _renderer) : GuiTwoColumnStringMenu(_renderer) {}

    void init() override;
    void render() override;

    virtual std::string getTitle() override;
    std::string getStatusLine() override;

    virtual void doCircle_Pressed() override;
    virtual void doSquare_Pressed() override;
    virtual void doTriangle_Pressed() override;
    virtual void doCross_Pressed() override;

    virtual void doEnter() override { doCross_Pressed(); }
    virtual void doEscape() override { doCircle_Pressed(); }
    virtual void doDelete() override { doSquare_Pressed(); }

    PsGames psGames;
    static int flushCovers(const char *file, const struct stat *sb, int flag, struct FTW *s);
    static bool sortByTitle(PsGamePtr i, PsGamePtr j) { return SortByCaseInsensitive(i->title, j->title); }
};
