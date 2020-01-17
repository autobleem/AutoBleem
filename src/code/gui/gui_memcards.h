#pragma once

#include "gui_menuBase.h"
#include <vector>
#include <string>
#include "../lang.h"

//********************
// GuiMemcards
//********************
class GuiMemcards : public GuiMenuBase {
public:
    GuiMemcards(SDL_Shared<SDL_Renderer> _renderer) : GuiMenuBase(_renderer,
                                                             "-=" + _("Custom Memory Cards") + "=-") {}

    void init() override;
    void render() override { GuiMenuBase::render(); }
    void loop() override { GuiMenuBase::loop(); }

    virtual std::string statusLine() override;   // returns the status line at the bottom

    void doCircle_Pressed() override;
    void doSquare_Pressed() override;
    void doTriangle_Pressed() override;
    void doCross_Pressed() override;

    void doEnter() { doCross_Pressed(); }
    void doEscape() { doCircle_Pressed(); }
    void doDelete() { doSquare_Pressed(); }
};
