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
#include "../environment.h"
#include "../engine/database.h"

using namespace std;

//*******************************
// GuiManager::init
//*******************************
void GuiManager::init() {
    GuiMenu::init();    // call the base class init()
    menuType = Menu_TwoColumns;

    firstRow = 2;
    xoffset_L = 0;
    xoffset_R = 500;

    psGames.clear();
    gui->db->getGames(&psGames);    // Create list of games
    sort(psGames.begin(), psGames.end(), sortByTitle);  // sort by title
    for (int i = 0; i < psGames.size(); ++i) {
        // "title"                "path"
        lines_L.emplace_back(psGames[i]->title);
        string path = DirEntry::removeSeparatorFromEndOfPath(psGames[i]->folder);
        path = DirEntry::removeGamesPathFromFrontOfPath(path);
        lines_R.emplace_back(path);
    }
}

//*******************************
// GuiManager::render
//*******************************
void GuiManager::render()
{
    SDL_RenderClear(renderer);
    gui->renderBackground();
    gui->renderTextBar();
    offset = gui->renderLogo(true);

    gui->renderFreeSpace();     // this is why this menu's render is special instead of using the base class

    gui->renderTextLine(title, 0, offset, POS_CENTER);

    makeSelectedLineVisibleOnPage();
    renderLines();
    renderSelectionBox();

    gui->renderStatus(statusLine());
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiManager::statusLine
//*******************************
string GuiManager::statusLine() {
    return _("Game") + " " + to_string(selected + 1) + "/" + to_string(psGames.size()) +
           "    |@L1|/|@R1| " + _("Page") +
           "   |@X| " + _("Select") +
           "  |@S| " + _("Delete Game") +
           "  |@T| " + _("Flush covers") +
           " |@O| " + _("Close") + " |";
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
// GuiManager::doCircle
//*******************************
void GuiManager::doCircle() {
    Mix_PlayChannel(-1, gui->cancel, 0);
    if (changes)
    {
        gui->forceScan=true;
    }
    menuVisible = false;
}

//*******************************
// GuiManager::doSquare
//*******************************
void GuiManager::doSquare() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    auto game = psGames[selected];
    int gameId = game->gameId;
    string gameName = game->title;
    string gameSaveStateFolder = game->ssFolder;
    GuiConfirm *confirm = new GuiConfirm(renderer);
    confirm->label = _("Are you sure you want to delete") + " " + gameName + "?";
    confirm->show();
    bool delGame = confirm->result;
    delete confirm;

    if (delGame) {
        cout << "Trying to delete " << gameName << endl;
        gui->renderStatus(_("Please wait ... deleting") + " " + gameName);
        bool success = gui->db->deleteGameIdFromAllTables(gameId);
        if (success) {
            success = DirEntry::removeDirAndContents(game->folder);
            if (success) {
                PsGames currentGames;
                gui->db->getGames(&currentGames);
                int numberOfGamesRemainingWithSameSaveState = count_if(begin(currentGames), end(currentGames),
                                                                       [&](const PsGamePtr &g) {
                                                                           return g->ssFolder == gameSaveStateFolder;
                                                                       });
                if (numberOfGamesRemainingWithSameSaveState == 0) {
                    GuiConfirm *confirm = new GuiConfirm(renderer);
                    confirm->label = _("Delete !SaveState folder for game") + " " + gameName + "?";
                    confirm->show();
                    bool delSSFolder = confirm->result;
                    delete confirm;
                    if (delSSFolder)
                        DirEntry::removeDirAndContents(gameSaveStateFolder);
                }
            }
        } else {
            cout << "Failed to delete directory " << game->folder << endl;
            gui->renderStatus(_("Failed to delete") + " " + gameName);
        }
    } else {
        cout << "Failed to delete " << gameName << endl;
        gui->renderStatus(_("Failed to delete") + " " + gameName);
    }
    gui->forceScan = true;  // in order for the sub dir hierarchy to be fixed we have to do a rescan
    //menuVisible = false;
    init(); // refresh games list and menu item count
    render();
}

//*******************************
// GuiManager::doTriangle
//*******************************
void GuiManager::doTriangle() {
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

//*******************************
// GuiManager::doCross
//*******************************
void GuiManager::doCross() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    if (!psGames.empty())
    {
        string selectedGameFolder = psGames[selected]->folder;
        GuiEditor *editor = new GuiEditor(renderer);
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
        selected = 0;
        firstVisibleIndex = 0;
        lastVisibleIndex = firstVisibleIndex + maxVisible - 1;

        init();
        int pos = 0;
        for (const auto & psGame : psGames)
        {
            if (psGame->folder == selectedGameFolder)
            {
                selected = pos;
                firstVisibleIndex = pos;
                lastVisibleIndex = firstVisibleIndex + maxVisible - 1;
            }
            pos++;
        }
        render();
        delete editor;
    }
}

//*******************************
// GuiManager::loop
//*******************************
void GuiManager::loop()
{
    menuVisible = true;

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
                        arrowDown();
                    } else if (gui->mapper.isUp(&e)) {
                        arrowUp();
                    }
                    break;

                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_R1, &e)) {
                        pageDown();

                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        pageUp();

                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        doCircle();

                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {
                        doSquare();

                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
                        doTriangle();

                    } else if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                        doCross();

                    }
                    break;
            }   // end switch
        }
    }
}