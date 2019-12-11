//
// Created by screemer on 2019-01-24.
//

#include <autobleem/autobleemui.h>
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
#include <ftw.h>
#include "../engine/scanner.h"
#include "../engine/database.h"

using namespace std;

//*******************************
// GuiManager::init
//*******************************
void GuiManager::init()
{
    psGames.clear();
    // Create list of games

    shared_ptr<Gui> gui(Gui::getInstance());
    gui->db->getGames(&psGames);

    // sort them by title
    sort(psGames.begin(), psGames.end(), sortByTitle);

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
    if (selected >= psGames.size()) {
        selected = psGames.size() - 1;
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
        if (i >= psGames.size()) {
            break;
        }
        string path = DirEntry::removeSeparatorFromEndOfPath(psGames[i]->folder);
        path = DirEntry::removeGamesPathFromFrontOfPath(path);
        gui->renderTextLine(string(80, ' ') + path, pos, offset);   // display game path in same column on the right
        gui->renderTextLine(psGames[i]->title, pos, offset);        // display game title on left
        pos++;
    }

    if (!psGames.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + 1, offset);
    }

    gui->renderStatus(_("Game") + " " + to_string(selected + 1) + "/" + to_string(psGames.size()) +
                      "    |@L1|/|@R1| " + _("Page") + "   |@X| " + _("Select") + "  |@S| " + _("Delete Game") + "  |@T| " + _("Flush covers") +
                      " |@O| " + _("Close") + " |");
    SDL_RenderPresent(Application::renderer);
}

//*******************************
// flushCovers
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
                            if (selected >= psGames.size()) {
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
                                selected = psGames.size()-1;
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
                        if (selected >= psGames.size()) {
                            selected = psGames.size() - 1;
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

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        auto game = psGames[selected];
                        int gameId = game->gameId;
                        string gameName = game->title;
                        string gameSaveStateFolder = game->ssFolder;
                        GuiConfirm * confirm = new GuiConfirm();
                        confirm->label = _("Are you sure you want to delete") + " " + gameName + "?";
                        confirm->show();
                        bool delGame = confirm->result;
                        delete confirm;

                        if (delGame)
                        {
                            cout << "Trying to delete " << gameName  << endl;
                            gui->renderStatus(_("Please wait ... deleting") + " " + gameName);
                            bool success = gui->db->deleteGameIdFromAllTables(gameId);
                            if (success) {
                                success = DirEntry::removeDirAndContents(game->folder);
                                if (success) {
                                    PsGames currentGames;
                                    gui->db->getGames(&currentGames);
                                    int numberOfGamesRemainingWithSameSaveState = count_if(begin(currentGames), end(currentGames),
                                            [&] (const PsGamePtr& g) { return g->ssFolder == gameSaveStateFolder; });
                                    if (numberOfGamesRemainingWithSameSaveState == 0) {
                                        GuiConfirm * confirm = new GuiConfirm();
                                        confirm->label = _("Delete !SaveState folder for game") + " " + gameName + "?";
                                        confirm->show();
                                        bool delSSFolder = confirm->result;
                                        delete confirm;
                                        if (delSSFolder)
                                            DirEntry::removeDirAndContents(gameSaveStateFolder);
                                    }
                                }
                                } else {
                                    cout << "Failed to delete directory " << game->folder  << endl;
                                    gui->renderStatus(_("Failed to delete") + " " + gameName);
                                }
                        }
                            else {
                                cout << "Failed to delete " << gameName  << endl;
                                gui->renderStatus(_("Failed to delete") + " " + gameName);
                            }
                            gui->forceScan = true;  // in order for the sub dir hierarchy to be fixed we have to do a rescan
                            //menuVisible = false;
                            init(); // refresh games list and menu item count
                            render();
                        } else {
                            render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        GuiConfirm * confirm = new GuiConfirm();
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
                        if (!psGames.empty())
                        {
                            string selectedGameFolder = psGames[selected]->folder;
                            GuiEditor *editor = new GuiEditor();
                            editor->gameData = psGames[selected];
                            editor->gameFolder = selectedGameFolder;
                            editor->gameIni.load(selectedGameFolder + sep + GAME_INI);
                            string folderNoLast = DirEntry::removeSeparatorFromEndOfPath(selectedGameFolder);
                            // change "/media/Games/Racing/Driver 2" to "Driver 2"
                            editor->gameIni.entry = DirEntry::getFileNameFromPath(folderNoLast);
                            editor->show();
                            if (editor->changes)
                            {
                                changes = true;
                            }
                            selected=0;
                            firstVisible=0;
                            lastVisible = firstVisible + maxVisible;

                            init();
                            int pos=0;
                            for (const auto & psGame : psGames)
                            {
                                if (psGame->folder == selectedGameFolder)
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