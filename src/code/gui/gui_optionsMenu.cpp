#include "gui_optionsMenu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <cassert>
#include "../lang.h"

using namespace std;

//*******************************
// void GuiOptionsMenu::init()
//*******************************
void GuiOptionsMenu::init()
{
    gui = Gui::getInstance();
    font = gui->themeFont;
}

#if 0
//*******************************
// GuiOptionsMenu::render
//*******************************
void GuiOptionsMenu::render()
{
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);
    gui->renderTextLine(title, 0, offset, POS_CENTER);

    gui->renderStatus(statusLine());
    SDL_RenderPresent(renderer);
}
#endif

//*******************************
// GuiOptionsMenu::statusLine
//*******************************
// the default status line for menus.  override if needed.
string GuiOptionsMenu::statusLine() {
    return "|@X| " + _("OK") + "     " + "|@O| " + _("Cancel") + "|";
}

//*******************************
// GuiOptionsMenu::handlePowerShutdownAndQuit
//*******************************
// returns true if applicable event type and it was handled
bool GuiOptionsMenu::handlePowerShutdownAndQuit(SDL_Event &e) {
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

//*******************************
// GuiOptionsMenu::doArrowDown
//*******************************
void GuiOptionsMenu::doArrowDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected++;
    if (selected > getVerticalSize() - 1) {
        selected = 0;
    }
}

//*******************************
// GuiOptionsMenu::doArrowUp
//*******************************
void GuiOptionsMenu::doArrowUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected--;
    if (selected < 0) {
        selected = getVerticalSize() - 1;
    }
}

//*******************************
// GuiMenu::doPageDown
//*******************************
void GuiOptionsMenu::doPageDown() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected = getVerticalSize() - 1;
}

//*******************************
// GuiMenu::doPageUp
//*******************************
void GuiOptionsMenu::doPageUp() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    selected = 0;
}

//*******************************
// GuiOptionsMenu::loop
//*******************************
void GuiOptionsMenu::loop()
{
    menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;

        if (SDL_PollEvent(&e)) {
            if (handlePowerShutdownAndQuit(e))
                continue;

            switch (e.type) {
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_DOWN)
                        doArrowDown();
                    if (e.key.keysym.sym == SDLK_UP)
                        doArrowUp();
                    if (e.key.keysym.sym == SDLK_RIGHT)
                        doArrowRight();
                    if (e.key.keysym.sym == SDLK_LEFT)
                        doArrowLeft();
                    if (e.key.keysym.sym == SDLK_PAGEDOWN)
                        doPageDown();
                    if (e.key.keysym.sym == SDLK_PAGEUP)
                        doPageUp();
                    if (e.key.keysym.sym == SDLK_HOME)
                        doHome();
                    if (e.key.keysym.sym == SDLK_END)
                        doEnd();

                    if (e.key.keysym.sym == SDLK_RETURN)
                        doEnter();
                    if (e.key.keysym.sym == SDLK_DELETE)
                        doDelete();
                    if (e.key.keysym.sym == SDLK_TAB)
                        doTab();
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                        doEscape();

                    break;

                case SDL_JOYAXISMOTION:
                case SDL_JOYHATMOTION:

                    if (gui->mapper.isDown(&e)) {
                        doArrowDown();
                    } else if (gui->mapper.isUp(&e)) {
                        doArrowUp();
                    } else if (gui->mapper.isRight(&e)) {
                        doArrowRight();
                    } else if (gui->mapper.isLeft(&e)) {
                        doArrowLeft();
                    }

                    break;
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_R1,&e)) {
                        doPageDown();
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e)) {
                        doPageUp();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        doCircle();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e)) {
                        doCross();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e)) {
                        doTriangle();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE,&e)) {
                        doSquare();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_START,&e)) {
                        doStart();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT,&e)) {
                        doSelect();
                    };
            }
        }
        render();
    }
}
