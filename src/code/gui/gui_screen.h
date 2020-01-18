//
// Created by screemer on 2019-01-24.
//
#pragma once

#include <SDL2/SDL_render.h>
#include "gui_sdl_wrapper.h"
#include "gui.h"

//********************
// GuiScreen
//********************
class GuiScreen {
public:
    GuiScreen(SDL_Shared<SDL_Renderer> _renderer) : renderer(_renderer), gui(Gui::getInstance()) {}

    SDL_Shared<SDL_Renderer> renderer;
    std::shared_ptr<Gui> gui;
    bool menuVisible = true;    // set this to false to exit the loop() here or your inherited loop()

    virtual void init() {};
    virtual void render()=0;
    virtual void loop();

    void show()
    {
        init();
        render();
        loop();
    }

    // controller dpad/joystick pressed
    virtual void doJoyUp() {}
    virtual void doJoyDown() {}
    virtual void doJoyRight() {}
    virtual void doJoyLeft() {}
    virtual void doJoyCenter() {}

    // controller button pressed
    virtual void doCross_Pressed() {}
    virtual void doCircle_Pressed() {}
    virtual void doTriangle_Pressed() {}
    virtual void doSquare_Pressed() {}
    virtual void doStart_Pressed() {}
    virtual void doSelect_Pressed() {}

    virtual void doL1_Pressed() {}
    virtual void doR1_Pressed() {}
    virtual void doL2_Pressed() {}
    virtual void doR2_Pressed() {}

    // controller button released
    virtual void doCircle_Released() {}
    virtual void doCross_Released() {}
    virtual void doTriangle_Released() {}
    virtual void doSquare_Released() {}
    virtual void doStart_Released() {}
    virtual void doSelect_Released() {}

    virtual void doL1_Released() {}
    virtual void doR1_Released() {}
    virtual void doL2_Released() {}
    virtual void doR2_Released() {}

    // keyboard
    virtual void doKeyUp() {}
    virtual void doKeyDown() {}
    virtual void doKeyRight() {}
    virtual void doKeyLeft() {}
    virtual void doPageDown() {}
    virtual void doPageUp() {}
    virtual void doHome() {}
    virtual void doEnd() {}
    virtual void doEnter() {}
    virtual void doDelete() {}
    virtual void doBackspace() {}
    virtual void doTab() {}
    virtual void doEscape() {}

    bool handlePowerShutdownAndQuit(SDL_Event &e);  // returns true if applicable event type and it was handled
};
