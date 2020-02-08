//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <string>
#include "gui_screen.h"
#include "gui.h"

//********************
// GuiKeyboard
//********************
class GuiKeyboard : public GuiScreen {
public:
    virtual void render();
    virtual void loop();
    virtual void init();

    bool handlePowerShutdownAndQuit(SDL_Event &e);  // returns true if applicable event type and it was handled

    void doKbdRight();
    void doKbdLeft();
    void doKbdHome();
    void doKbdEnd();
    void doKbdBackspace();
    void doKbdDelete();
    void doKbdTab();
    void doKbdEscape();
    void doKbdReturn();
    void doKbdTextInput(SDL_Event& e);

    void doL1_up();
    void doL2_up();
    void doL1_down();
    void doL2_down();

    void doTriangle();
    void doSquare();
    void doCross();
    void doCircle();

    void doStart();

    void doJoyRight();
    void doJoyLeft();
    void doJoyDown();
    void doJoyUp();

    std::shared_ptr<Gui> gui;
    int selx=0;
    int sely=0;
    std::string label="";
    std::string result="";
    int cursorIndex = 0;
    bool L1_caps_shift = false;
    bool L2_cursor_shift = false;
    bool cancelled=true;
    bool usingUsbKeyboard = false;
    bool displayAsterisksInstead = false;   // this is a password.  display *****

    using GuiScreen::GuiScreen;
};
