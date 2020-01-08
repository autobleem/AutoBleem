#include "gui_gameDirMenu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include "gui.h"
#include "../main.h"
#include "../lang.h"
#include <ftw.h>

using namespace std;

//*******************************
// void GuiGameDirMenu::init()
//*******************************
void GuiGameDirMenu::init()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible - 1;
}

//*******************************
// GuiGameDirMenu::render
//*******************************
void GuiGameDirMenu::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    // use evoUI background

    SDL_RenderClear(renderer);
    gui->renderBackground();

    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + _("Select PS1 Game Category") + "=-",0,offset,true);
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

    int pos = 1;
    for (int i = firstVisible; i <= lastVisible; i++) {
        if (i >= lines.size()) {
            break;
        }
        gui->renderTextLine(lines[i], pos+1, offset,false);
        pos++;
    }

    if (!lines.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + 2, offset);
    }

    gui->renderStatus(_("Entry")+" " + to_string(selected + 1) + "/" + to_string(lines.size()) +"    |@L1|/|@R1| "+_("Page")+"   |@X| "+_("Select")+"   |@O| "+_("Close")+" |");
    SDL_RenderPresent(renderer);
}


//*******************************
// GuiGameDirMenu::loop
//*******************************
void GuiGameDirMenu::loop()
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