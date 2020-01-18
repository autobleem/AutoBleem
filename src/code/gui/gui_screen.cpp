#include "gui_screen.h"
#include "../lang.h"

//*******************************
// GuiScreen::loop
//*******************************
void GuiScreen::loop()
{
    menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;

        if (SDL_PollEvent(&e)) {
            if (handlePowerShutdownAndQuit(e))
                continue;

            switch (e.type) {
                case SDL_JOYAXISMOTION:
                case SDL_JOYHATMOTION:

                    if (gui->mapper.isUp(&e))
                        doJoyUp();
                    else if (gui->mapper.isDown(&e))
                        doJoyDown();
                    else if (gui->mapper.isRight(&e))
                        doJoyRight();
                    else if (gui->mapper.isLeft(&e))
                        doJoyLeft();
                    else if (gui->mapper.isCenter(&e))
                        doJoyCenter();
                    break;

                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e))
                        doCross_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e))
                        doCircle_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e))
                        doTriangle_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE,&e))
                        doSquare_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_START,&e))
                        doStart_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT,&e))
                        doSelect_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e))
                        doL1_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_R1,&e))
                        doR1_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_L2,&e))
                        doL2_Pressed();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_R2,&e))
                        doR2_Pressed();

                case SDL_JOYBUTTONUP:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e))
                        doCross_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e))
                        doCircle_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e))
                        doTriangle_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE,&e))
                        doSquare_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_START,&e))
                        doStart_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT,&e))
                        doSelect_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e))
                        doL1_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_R1,&e))
                        doR1_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_L2,&e))
                        doL2_Released();
                    else if (e.jbutton.button == gui->_cb(PCS_BTN_R2,&e))
                        doR2_Released();

                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_UP)
                        doKeyUp();
                    else if (e.key.keysym.sym == SDLK_DOWN)
                        doKeyDown();
                    else if (e.key.keysym.sym == SDLK_RIGHT)
                        doKeyRight();
                    else if (e.key.keysym.sym == SDLK_LEFT)
                        doKeyLeft();
                    else if (e.key.keysym.sym == SDLK_PAGEDOWN)
                        doPageDown();
                    else if (e.key.keysym.sym == SDLK_PAGEUP)
                        doPageUp();
                    else if (e.key.keysym.sym == SDLK_HOME)
                        doHome();
                    else if (e.key.keysym.sym == SDLK_END)
                        doEnd();
                    else if (e.key.keysym.sym == SDLK_RETURN)
                        doEnter();
                    else if (e.key.keysym.sym == SDLK_DELETE)
                        doDelete();
                    else if (e.key.keysym.sym == SDLK_BACKSPACE)
                        doBackspace();
                    else if (e.key.keysym.sym == SDLK_TAB)
                        doTab();
                    else if (e.key.keysym.sym == SDLK_ESCAPE)
                        doEscape();
                    break;

            }
        }
        render();
    }
}

//*******************************
// GuiScreen::handlePowerShutdownAndQuit
//*******************************
// returns true if applicable event type and it was handled
bool GuiScreen::handlePowerShutdownAndQuit(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
            gui->drawText(_("POWERING OFF... PLEASE WAIT"));
            Util::powerOff();
            return true;    // but it will never get here
        }
    } else if (e.type == SDL_QUIT) {     // this is for pc Only
        menuVisible = false;
        return true;
    }
    return false;
}

