#pragma once

#include "gui_menu.h"
#include <vector>
#include <string>
#include "../lang.h"

class GuiGameDirMenu : public GuiMenu {
public:
    GuiGameDirMenu(SDL_Shared<SDL_Renderer> _renderer) : GuiMenu(_renderer,
                                                         "-=" + _("Select PS1 Game Category") + "=-") {}

    void init() override { GuiMenu::init(); };
    void render() override { GuiMenu::render(); };
    void loop() override { GuiMenu::loop(); };
};
