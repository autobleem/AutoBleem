//
// Created by screemer on 2019-07-25.
//

#include "gui_playlists.h"
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
// void GuiPlaylists::init()
//*******************************
void GuiPlaylists::init()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible;
}

//*******************************
// GuiPlaylists::render
//*******************************
void GuiPlaylists::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + _("Select RetroBoot Platform") + "=-",0,offset,true);
    if (selected >= playlists.size()) {
        selected = playlists.size() - 1;
    }

    if (selected < firstVisible) {
        firstVisible--;
        lastVisible--;
    }
    if (selected >= lastVisible) {
        firstVisible++;
        lastVisible++;
    }

    int pos = 1;
    for (int i = firstVisible; i < lastVisible; i++) {
        if (i >= playlists.size()) {
            break;
        }
        gui->renderTextLine(Util::getFileNameWithoutExtension(playlists[i]), pos, offset,true);
        pos++;
    }

    if (!playlists.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + 1, offset);
    }

    gui->renderStatus(_("Entry")+" " + to_string(selected + 1) + "/" + to_string(playlists.size()) +"    |@L1|/|@R1| "+_("Page")+"   |@X| "+_("Select")+"   |@O| "+_("Close")+" |");
    SDL_RenderPresent(renderer);
}


//*******************************
// GuiManager::loop
//*******************************
void GuiPlaylists::loop()
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
                        if (selected >= playlists.size()) {
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
                            selected = playlists.size()-1;
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
                        if (selected >= playlists.size()) {
                            selected = playlists.size() - 1;
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
                        if (!playlists.empty())
                        {

                            menuVisible = false;

                        }
                    };
            }
        }
    }
}