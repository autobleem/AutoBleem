#pragma once

#include "gui_menuBase.h"
#include <vector>
#include <string>
#include "../lang.h"

class GuiGameDirMenu : public GuiMenuBase {
public:
    GuiGameDirMenu(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer,
                                                         "-=" + _("Select PS1 Game Category") + "=-") {}

    void init() override { GuiMenuBase::init(); };
    void render() override { GuiMenuBase::render(); };
    void loop() override { GuiMenuBase::loop(); };

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }
};
