#include "gui_Menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
//#include <iostream>
#include "../lang.h"

using namespace std;

//*******************************
// void GuiMenu::init()
//*******************************
void GuiMenu::init()
{
    gui = Gui::getInstance();

    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible - 1;
}

//*******************************
// GuiMenu::render
//*******************************
void GuiMenu::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    // use evoUI background

    SDL_RenderClear(renderer);
    gui->renderBackground();

    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine(title, 0, offset, POS_CENTER);
    if (selected >= lines.size()) {
        selected = lines.size() - 1;
    }

    if (selected < firstVisible) {
        int moveBy = firstVisible - selected;
        firstVisible -= moveBy;
        lastVisible -= moveBy;
    }
    if (selected >= lastVisible) {
        int moveBy = selected - lastVisible;
        firstVisible += moveBy;
        lastVisible += moveBy;
    }

    int pos = firstLine;
    for (int i = firstVisible; i <= lastVisible; i++) {
        if (i >= lines.size()) {
            break;
        }
        gui->renderTextLine(lines[i], pos, offset,false);
        pos++;
    }

    if (!lines.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + firstLine, offset);
    }

    gui->renderStatus(statusLine());
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiMenu::statusLine
//*******************************
// the default status line for menus.  override if needed.
string GuiMenu::statusLine() {
    return _("Entry")+" " + to_string(selected + 1) + "/" + to_string(lines.size()) +
             "    |@L1|/|@R1| " + _("Page") +
             "   |@X| " + _("Select") +
             "   |@O| " + _("Close") + " |";
}

//*******************************
// GuiMenu::loop
//*******************************
void GuiMenu::loop()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    while (menuVisible) {
        gui->watchJoystickPort();
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();
                }
            }
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYAXISMOTION:
                case SDL_JOYHATMOTION:

                    if (gui->mapper.isDown(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selected++;
                        if (selected >= lines.size()) {
                            selected = 0;
                            firstVisible = selected;
                            lastVisible = firstVisible+maxVisible;
                        }
                        render();
                    }
                    if (gui->mapper.isUp(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selected--;
                        if (selected < 0) {
                            selected = lines.size()-1;
                            firstVisible = selected;
                            lastVisible = firstVisible+maxVisible;
                        }
                        render();
                    }

                    break;
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_R1,&e)) {
                        Mix_PlayChannel(-1, gui->home_up, 0);
                        selected+=maxVisible;
                        if (selected >= lines.size()) {
                            selected = lines.size() - 1;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible+maxVisible;
                        render();
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1,&e)) {
                        Mix_PlayChannel(-1, gui->home_down, 0);
                        selected-=maxVisible;
                        if (selected < 0) {
                            selected = 0;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible+maxVisible;
                        render();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        cancelled = true;
                        menuVisible = false;
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        cancelled = false;
                        if (!lines.empty())
                        {
                            menuVisible = false;
                        }
                    };
            }
        }
    }
}
