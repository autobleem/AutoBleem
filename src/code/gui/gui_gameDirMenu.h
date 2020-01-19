#pragma once

#include "gui_stringMenu.h"
#include <vector>
#include <string>
#include "../lang.h"

//*******************************
// class GuiGameDirMenu
//*******************************
class GuiGameDirMenu : public GuiStringMenu {
public:
    GuiGameDirMenu(SDL_Shared<SDL_Renderer> _renderer) : GuiStringMenu(_renderer) {}

    virtual std::string getTitle() override { return "-=" + _("Select PS1 Game Category") + "=-"; }
    virtual std::string getStatusLine() override { return GuiStringMenu::getStatusLine(); }

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }
};
