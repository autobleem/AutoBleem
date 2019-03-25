//
// Created by screemer on 2019-01-24.
//

#include "gui_manager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "gui_editor.h"
#include "gui_confirm.h"
#include "../main.h"
#include "../lang.h"
#include <ftw.h>

bool wayToSort(Inifile i, Inifile j) {
    string title1 = i.values["title"];
    string title2 = j.values["title"];

    title1=lcase(title1);
    title2=lcase(title2);
    return title1 < title2;
}

void GuiManager::init()
{
    games.clear();
    // Create list of games

    shared_ptr<Gui> gui(Gui::getInstance());
    string path = gui->path;
    for (DirEntry entry: Util::diru(path)) {
        if (!Util::isDirectory(gui->path+Util::separator()+entry.name)) continue;
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;

        string gameDataPath = path + entry.name + Util::separator();
        string saveStateDir = path + Util::separator() + "!SaveStates" + Util::separator() + entry.name;
        Inifile ini;
        ini.load(path+entry.name+Util::separator()+GAME_INI);
        if (ini.section.empty())
        {
            continue;
        }
        ini.entry=entry.name;
        games.push_back(ini);

    }
    // sort them
    sort(games.begin(), games.end(), wayToSort);
    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible;
}
void GuiManager::render()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderFreeSpace();
    gui->renderTextLine(_("-=Game manager - Select game=-"),0,offset,true);
    if (selected >= games.size()) {
        selected = games.size() - 1;
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
        if (i >= games.size()) {
            break;
        }
        gui->renderTextLine(games[i].values["title"], pos, offset);
        pos++;
    }

    if (!games.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + 1, offset);
    }


    gui->renderStatus(_("Game")+" " + to_string(selected + 1) + "/" + to_string(games.size()) +"    |@L1|/|@R1| "+_("Page")+"   |@X| "+_("Select")+"  |@T| "+_("Flush covers")+" |@O| "+_("Close")+" |");
    SDL_RenderPresent(renderer);
}


int process(const char *file, const struct stat *sb,
            int flag, struct FTW *s)
{
    int retval = 0;


    if (Util::getFileExtension(file)=="png")
    {

        remove(file);
    }


    return retval;
}

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
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selected++;
                            if (selected >= games.size()) {
                                selected = 0;
                                firstVisible = selected;
                                lastVisible = firstVisible+maxVisible;
                            }
                            render();
                        }
                        if (e.jaxis.value < -PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selected--;
                            if (selected < 0) {
                                selected = games.size()-1;
                                firstVisible = selected;
                                lastVisible = firstVisible+maxVisible;
                            }
                            render();
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == PCS_BTN_R1) {
                        Mix_PlayChannel(-1, gui->home_up, 0);
                        selected+=maxVisible;
                        if (selected >= games.size()) {
                            selected = games.size() - 1;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible+maxVisible;
                        render();
                    };
                    if (e.jbutton.button == PCS_BTN_L1) {
                        Mix_PlayChannel(-1, gui->home_down, 0);
                        selected-=maxVisible;
                        if (selected < 0) {
                            selected = 0;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible+maxVisible;
                        render();
                    };


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        if (changes)
                        {
                            gui->forceScan=true;
                        }
                        menuVisible = false;

                    };


                    if (e.jbutton.button == PCS_BTN_TRIANGLE) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        GuiConfirm * confirm = new GuiConfirm(renderer);
                        confirm->label = _("Are you sure you want to flush all covers ?");
                        confirm->show();
                        bool delCovers = confirm->result;
                        delete confirm;

                        if (delCovers)
                        {
                            cout << "Trying to delete covers" << endl;
                            gui->renderStatus(_("Please wait ... deleting covers..."));


                            int errors = 0;
                            int flags = FTW_DEPTH | FTW_PHYS | FTW_CHDIR;


                            if (nftw("/media/Games", process, 1, flags) != 0) {

                                errors++;
                            }



                            gui->forceScan = true;
                            menuVisible = false;
                        } else {
                            render();
                        }
                    }


                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (!games.empty())
                        {
                            string selectedEntry = games[selected].entry;
                            GuiEditor *editor = new GuiEditor(renderer);
                            editor->game = games[selected];
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
                            for (Inifile game:games)
                            {
                                if (game.entry==selectedEntry)
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