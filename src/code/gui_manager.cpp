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
#include "main.h"

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
    for (DirEntry entry: Util::dir(path)) {
        if (entry.name[0] == '.') continue;
        if (!entry.dir) continue;
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
    gui->renderTextLine("-=Game manager - Select game=-",0,offset,true);
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


    gui->renderStatus("Game " + to_string(selected + 1) + "/" + to_string(games.size()) +"    |@X| Select  |@O| Close |");
    SDL_RenderPresent(renderer);
}

void GuiManager::loop()
{
    shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    while (menuVisible) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYAXISMOTION:
                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > 3200) {
                            selected++;
                            if (selected >= games.size()) {
                                selected = games.size() - 1;
                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            selected--;
                            if (selected < 0) {
                                selected = 0;
                            }
                            render();
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {

                        if (changes)
                        {
                            gui->forceScan=true;
                        }
                        menuVisible = false;

                    };


                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        if (!games.empty())
                        {
                            GuiEditor *editor = new GuiEditor(renderer);
                            editor->game = games[selected];
                            editor->show();
                            if (editor->changes)
                            {
                                changes = true;
                                int selectedNow=selected;
                                init();
                                selected = selectedNow;
                            }
                            render();
                            delete editor;
                        }



                    };


            }

        }
    }
}