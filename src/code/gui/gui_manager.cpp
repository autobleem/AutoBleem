//
// Created by screemer on 2019-01-24.
//

#include "gui_manager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include "gui.h"
#include "gui_editor.h"
#include "gui_confirm.h"
#include "../main.h"
#include "../lang.h"
#include <ftw.h>
#include "../engine/scanner.h"
#include "../Environment.h"

using namespace std;

//*******************************
// GuiManager::init
//*******************************
void GuiManager::init()
{
    gameInis.clear();
    // Create list of games

    shared_ptr<Gui> gui(Gui::getInstance());
    string pathToGamesDir = Env::getPathToGamesDir();
    for (const DirEntry &entry : DirEntry::diru(pathToGamesDir)) {
        if (!DirEntry::isDirectory(pathToGamesDir + sep + entry.name)) continue;
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;

        string gameDataPath = pathToGamesDir + sep + entry.name + sep;
        string saveStateDir = Env::getPathToSaveStatesDir() + sep + entry.name;
        Inifile ini;
        ini.load(Env::getPathToGamesDir() + sep + entry.name + sep + GAME_INI);
        if (ini.section.empty())
        {
            continue;
        }
        ini.entry=entry.name;
        gameInis.push_back(ini);
    }
    // sort them
    sort(gameInis.begin(), gameInis.end(), sortByTitle);
    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible;
}

//*******************************
// GuiManager::render
//*******************************
void GuiManager::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderFreeSpace();
    gui->renderTextLine("-=" + _("Game manager - Select game") + "=-",0,offset,POS_CENTER);
    if (selected >= gameInis.size()) {
        selected = gameInis.size() - 1;
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
        if (i >= gameInis.size()) {
            break;
        }
        gui->renderTextLine(gameInis[i].values["title"], pos, offset);
        pos++;
    }

    if (!gameInis.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + 1, offset);
    }

    gui->renderStatus(_("Game")+" " + to_string(selected + 1) + "/" + to_string(gameInis.size()) +"    |@L1|/|@R1| "+_("Page")+"   |@X| "+_("Select")+"  |@T| "+_("Flush covers")+" |@O| "+_("Close")+" |");
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiManager::flushCovers
//*******************************
int flushCovers(const char *file, const struct stat *sb,
            int flag, struct FTW *s)
{
    int retval = 0;

    if (DirEntry::getFileExtension(file)=="png")
    {
        remove(file);
    }

    return retval;
}

//*******************************
// GuiManager::loop
//*******************************
void GuiManager::loop()
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
                            if (selected >= gameInis.size()) {
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
                                selected = gameInis.size()-1;
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
                        if (selected >= gameInis.size()) {
                            selected = gameInis.size() - 1;
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
                        if (changes)
                        {
                            gui->forceScan=true;
                        }
                        menuVisible = false;
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        GuiConfirm * confirm = new GuiConfirm(renderer);
                        confirm->label = _("Are you sure you want to flush all covers?");
                        confirm->show();
                        bool delCovers = confirm->result;
                        delete confirm;

                        if (delCovers)
                        {
                            cout << "Trying to delete covers" << endl;
                            gui->renderStatus(_("Please wait ... deleting covers..."));

                            int errors = 0;
                            int flags = FTW_DEPTH | FTW_PHYS | FTW_CHDIR;
                            //cout << gui->pathToGamesDir << endl;
                            if (nftw(DirEntry::fixPath(gui->pathToGamesDir).c_str(), flushCovers, 1, flags) != 0) {
                                errors++;
                            }

                            gui->forceScan = true;
                            menuVisible = false;
                        } else {
                            render();
                        }
                    }


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (!gameInis.empty())
                        {
                            string selectedEntry = gameInis[selected].entry;
                            GuiEditor *editor = new GuiEditor(renderer);
                            editor->gameIni = gameInis[selected];
                            editor->show();
                            if (editor->changes)
                            {
                                changes = true;
                            }
                            selected=0;
                            firstVisible=0;
                            lastVisible=firstVisible+maxVisible;

                            init();
                            int pos=0;
                            for (const Inifile & gameIni:gameInis)
                            {
                                if (gameIni.entry==selectedEntry)
                                {
                                    selected=pos;
                                    firstVisible=pos;
                                    lastVisible=firstVisible+maxVisible;
                                }
                                pos++;
                            }
                            render();
                            delete editor;
                        }
                    };
            }
        }
    }
}