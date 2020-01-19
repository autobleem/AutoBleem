#pragma once

#include "gui_stringMenu.h"
#include <vector>
#include <string>
#include "../lang.h"

//********************
// GuiMemcards
//********************
class GuiMemcards : public GuiStringMenu {
public:
    GuiMemcards(SDL_Shared<SDL_Renderer> _renderer) : GuiStringMenu(_renderer) {}

    void init() override;

    virtual std::string getTitle() override { return "-=" + _("-=" + _("Custom Memory Cards") + "=-") + "=-"; }
    virtual std::string getStatusLine() override;   // returns the status line at the bottom

    void doCircle_Pressed() override;
    void doSquare_Pressed() override;
    void doTriangle_Pressed() override;
    void doCross_Pressed() override;

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }
    void doDelete() { doSquare_Pressed(); }
};
