#pragma once

#include "gui_menuBase.h"
#include <SDL2/SDL_render.h>
#include "gui_sdl_wrapper.h"

//*******************************
// class GuiStringMenu
//*******************************
class GuiStringMenu : public GuiMenuBase<std::string> {
public:
    GuiStringMenu(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer) {}

    virtual std::string getTitle() override { return GuiMenuBase::getTitle(); }
    virtual std::string getStatusLine() override { return GuiMenuBase::getStatusLine(); }

    virtual void renderLineIndexOnRow(int index, int row) override {
        gui->renderTextLine(lines[index], row, offset, POS_LEFT, 0, font);
    }
};
