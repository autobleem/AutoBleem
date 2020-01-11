#pragma once

#include "gui_menu.h"
#include <vector>
#include <string>
#include "../lang.h"

//********************
// GuiMemcards
//********************
class GuiMemcards : public GuiMenu {
public:
    GuiMemcards(SDL_Shared<SDL_Renderer> _renderer) : GuiMenu(_renderer,
                                                             "-=" + _("Custom Memory Cards") + "=-") {}

    void init() override;
    void render() override { GuiMenu::render(); };
    void loop() override;

    virtual std::string statusLine() override;   // returns the status line at the bottom

    void doCircle();
    void doSquare();
    void doTriangle();
    void doCross();
};
