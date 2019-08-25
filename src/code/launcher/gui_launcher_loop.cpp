#include "gui_launcher.h"
#include "../gui/gui.h"
#include "../gui/gui_options.h"
#include "../gui/gui_confirm.h"
#include "../gui/gui_editor.h"
#include "../lang.h"
#include "pcsx_interceptor.h"
#include "gui_btn_guide.h"
#include <algorithm>
#include <iostream>
#include "../engine/scanner.h"
#include "../gui/gui_playlists.h"
#include "gui_mc_manager.h"
#include "../gui/gui_gameDirMenu.h"

using namespace std;

//*******************************
// GuiLauncher::loop_prevGame
//*******************************
// event loop
void GuiLauncher::loop_prevGame() {
    if (state == STATE_GAMES) {
        if (carouselGames.empty()) {
            return;
        }
        // find prev game
        int nextGame = selGame;
        string currentFirst = carouselGames[selGame]->title.substr(0, 1);
        string futureFirst = carouselGames[selGame]->title.substr(0, 1);
        for (int i = selGame; i >= 0; i--) {
            futureFirst = carouselGames[i]->title.substr(0, 1);
            if (currentFirst != futureFirst) {
                nextGame = i;
                break;
            }
        }
        // now find the same
        for (int i = nextGame; i >= 0; i--) {
            string foundFirst = carouselGames[i]->title.substr(0, 1);
            if (futureFirst == foundFirst) {
                nextGame = i;
            } else {
                break;
            }
        }
        if (nextGame != selGame) {
            // we have next game;
            Mix_PlayChannel(-1, gui->cursor, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
            selGame = nextGame;
            setInitialPositions(selGame);
            updateMeta();
            menu->setResumePic(carouselGames[selGame]->findResumePicture());
        } else {
            Mix_PlayChannel(-1, gui->cancel, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
        }
    }
}

//*******************************
// GuiLauncher::loop_nextGame
//*******************************
// event loop
void GuiLauncher::loop_nextGame() {
    if (state == STATE_GAMES) {
        if (carouselGames.empty()) {
            return;
        }
        // find next game
        int nextGame = selGame;
        string currentFirst = carouselGames[selGame]->title.substr(0, 1);
        string futureFirst = carouselGames[selGame]->title.substr(0, 1);
        for (int i = selGame; i < carouselGames.size(); i++) {
            futureFirst = carouselGames[i]->title.substr(0, 1);
            if (currentFirst != futureFirst) {
                nextGame = i;
                break;
            }
        }
        if (nextGame != selGame) {
            // we have next game;
            Mix_PlayChannel(-1, gui->cursor, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
            selGame = nextGame;
            setInitialPositions(selGame);
            updateMeta();
            menu->setResumePic(carouselGames[selGame]->findResumePicture());
        } else {
            Mix_PlayChannel(-1, gui->cancel, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
        }
    }
}

//*******************************
// GuiLauncher::loop
//*******************************
// event loop
    void GuiLauncher::loop() {
    cout << "Main Loop" << endl;
    powerOffShift = false;
    L1_shift = false;
    bool menuVisible = true;
    long motionStart = 0;
    long timespeed = 0;
    int motionDir = 0;

    while (menuVisible) {
        vector<string> headers = {_("SETTINGS"), _("GAME"), _("MEMORY CARD"), _("RESUME")};
        vector<string> texts = {_("Customize AutoBleem settings"), _("Edit game parameters"),
                                _("Edit Memory Card information"), _("Resume game from saved state point")};
        gui->watchJoystickPort();
        long time = SDL_GetTicks();
        for (auto obj:staticElements) {
            obj->update(time);
        }

        menu->update(time);
        updatePositions();
        render();

        if (motionStart != 0) {
            long timePressed = time - motionStart;
            if (timePressed > 300) {
                if (time - timespeed > 100) {
                    if (motionDir == 0) {
                        if (!scrolling) {
                            nextGame(60);
                        }
                    } else {
                        if (!scrolling) {
                            prevGame(60);
                        }
                    }
                    timespeed = time;
                }
                if (timespeed == 0) {
                    timespeed = time;
                }
            } else {
                timespeed = 0;
            }
        }
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            // this is for pc Only
            if (e.type == SDL_QUIT) {
                menuVisible = false;
            }
            switch (e.type) {
                case SDL_KEYDOWN:
                    if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                        gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                        Util::powerOff();
                    }
                    break;
                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                case SDL_JOYHATMOTION:
                    if (gui->mapper.isCenter(&e)) {
                        if (state == STATE_GAMES) {
                            if (carouselGames.empty()) {
                                continue;
                            }
                        }
                        motionStart = 0;
                    }
                    if (gui->mapper.isLeft(&e)) {
                        if (state == STATE_GAMES) {
                            if (carouselGames.empty()) {
                                continue;
                            }
                            if (!scrolling) {
                                motionStart = time;
                                motionDir = 1;
                                scrolling = true;
                                prevGame(110);
                            }
                        } else if (state == STATE_SET) {

                            if (!menu->foreign) {
                                if (menu->selOption != 0) {
                                    if (menu->animationStarted == 0) {
                                        Mix_PlayChannel(-1, gui->cursor, 0);
                                        menu->transition = TR_OPTION;
                                        menu->direction = 0;
                                        menu->duration = 100;
                                        menuHead->setText(headers[menu->selOption - 1], fgR, fgG, fgB);
                                        menuText->setText(texts[menu->selOption - 1], fgR, fgG, fgB);
                                        menu->animationStarted = time;
                                    }
                                }
                            }

                        } else if (state == STATE_RESUME) {
                            if (sselector->selSlot != 0) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                sselector->selSlot--;
                            }
                        }
                    }
                    if (gui->mapper.isRight(&e)) {
                        if (state == STATE_GAMES) {
                            if (carouselGames.empty()) {
                                continue;
                            }
                            if (!scrolling) {
                                motionStart = time;
                                motionDir = 0;
                                scrolling = true;
                                nextGame(110);
                            }
                        } else if (state == STATE_SET) {

                            if (!menu->foreign) {
                                if (menu->selOption != 3) {
                                    if (menu->animationStarted == 0) {
                                        Mix_PlayChannel(-1, gui->cursor, 0);
                                        menu->transition = TR_OPTION;
                                        menu->direction = 1;
                                        menu->duration = 100;
                                        menuHead->setText(headers[menu->selOption + 1], fgR, fgG, fgB);
                                        menuText->setText(texts[menu->selOption + 1], fgR, fgG, fgB);
                                        menu->animationStarted = time;
                                    }
                                }
                            }

                        } else if (state == STATE_RESUME) {
                            if (sselector->selSlot != 3) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                sselector->selSlot++;
                            }
                        }
                    }

                    if (gui->mapper.isDown(&e)) {
                        if (scrolling) {
                            continue;
                        }
                        if (state == STATE_GAMES) {
                            if (menu->animationStarted == 0) {
                                menu->transition = TR_MENUON;
                                switchState(STATE_SET, time);
                                motionStart = 0;
                            }
                        }

                    } else if (gui->mapper.isUp(&e)) {
                        if (scrolling) {
                            continue;
                        }
                        if (state == STATE_SET) {
                            if (menu->animationStarted == 0) {
                                menu->transition = TR_MENUON;
                                switchState(STATE_GAMES, time);
                                motionStart = 0;
                            }
                        }
                    } else {
                        ;
                    }
                    break;
                case SDL_JOYBUTTONUP:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L2, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        powerOffShift = false;
                    }
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (L1_shift) {
                            loop_prevGame();
                        }
                        L1_shift = false;
                    }
                    if (e.jbutton.button == gui->_cb(PCS_BTN_R1, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        loop_nextGame();
                        L1_shift = false;
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L2, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        powerOffShift = true;
                    }
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        L1_shift = true;
                        powerOffShift = false;
                    }

                    if (L1_shift) {
                        if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT, &e)) {
                            L1_shift = false;
                            if (state == STATE_GAMES && currentSet == SET_EXTERNAL) {
                                // pop game dir menu
                                powerOffShift = false;
                                GameRowInfos gameRowInfos;
                                gui->db->getGameRowInfos(&gameRowInfos);
                                if (gameRowInfos.size() == 0) {
                                    // abort
                                }
                                auto guiGameDirMenu = new GuiGameDirMenu(renderer);
                                for (auto &rowInfo : gameRowInfos) {
                                    string text = string(rowInfo.indentLevel * 2, ' ') +
                                            rowInfo.rowName + " (" + to_string(rowInfo.numGames) + " " + _("Games") + ")";
                                    guiGameDirMenu->textsToDisplay.emplace_back(text);
                                }
                                guiGameDirMenu->backgroundImg = background->tex;
                                int nextSel = currentGameDirIndex;
                                guiGameDirMenu->selected = nextSel;
                                guiGameDirMenu->firstVisible = nextSel;
                                guiGameDirMenu->lastVisible = nextSel + guiGameDirMenu->maxVisible;

                                guiGameDirMenu->show();
                                bool cancelled = guiGameDirMenu->cancelled;
                                currentGameDirIndex = guiGameDirMenu->selected;
                                currentGameDirName = gameRowInfos[currentGameDirIndex].rowName;
                                delete guiGameDirMenu;

                                if (cancelled)
                                    continue;
                                switchSet(currentSet,true);
                                menuHead->setText(headers[0], fgR, fgG, fgB);
                                menuText->setText(texts[0], fgR, fgG, fgB);
                                showSetName();
                                if (selGame != -1) {
                                    updateMeta();
                                    menu->setResumePic(carouselGames[selGame]->findResumePicture());
                                } else {
                                    updateMeta();
                                }
                            }
                            L1_shift = false;
                            powerOffShift = false;
                            continue;
                        }
                    }

                    if (powerOffShift) {
                        if (e.jbutton.button == gui->_cb(PCS_BTN_R2, &e)) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                            Util::powerOff();
                        }

                        if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT, &e)) {

                            if (state == STATE_GAMES) {
                                if (!DirEntry::exists(RA_FOLDER)) {
                                    continue;
                                }
                                if (raPlaylists.empty()) {
                                    continue;
                                }
                                powerOffShift = false;
                                auto playlists = new GuiPlaylists(renderer);
                                playlists->playlists = raPlaylists;
                                playlists->backgroundImg = background->tex;
                                playlists->integrator = &raIntegrator;
                                int nextSel = 0;
                                int i = 0;
                                for (string plist:playlists->playlists) {
                                    if (plist == retroarch_playlist_name) {
                                        nextSel = i;
                                        break;
                                    }
                                    i++;
                                }
                                playlists->selected = nextSel;
                                playlists->firstVisible = nextSel;
                                playlists->lastVisible = nextSel + playlists->maxVisible;

                                playlists->show();
                                bool cancelled = playlists->cancelled;
                                int selected = playlists->selected;
                                delete playlists;

                                if (cancelled) continue;
                                retroarch_playlist_name = raPlaylists[selected];
                                currentSet = SET_RETROARCH;
                                switchSet(currentSet,false);
                                menuHead->setText(headers[0], fgR, fgG, fgB);
                                menuText->setText(texts[0], fgR, fgG, fgB);
                                showSetName();
                                if (selGame != -1) {
                                    updateMeta();
                                    menu->setResumePic(carouselGames[selGame]->findResumePicture());
                                } else {
                                    updateMeta();
                                }
                            }
                        }
                        break;
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        if (state == STATE_SET) {
                            if (menu->animationStarted == 0) {
                                menu->transition = TR_MENUON;
                                switchState(STATE_GAMES, time);
                                motionStart = 0;
                            }
                        } else if (state == STATE_GAMES) {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                            menuVisible = false;
                        } else if (state == STATE_RESUME) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            sselector->visible = false;
                            arrow->visible = true;
                            sselector->cleanSaveStateImages();
                            menu->setResumePic(carouselGames[selGame]->findResumePicture());

                            if (sselector->operation == OP_LOAD) {
                                state = STATE_SET;
                            } else {
                                state = STATE_GAMES;
                            }
                        }
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {
                        gui->padMapping = gui->mapper.getMappingString(e.jbutton.which);
                        if (DirEntry::exists(string(RA_FOLDER)+"/retroarch")) {

                            if (state == STATE_GAMES) {
                                if (carouselGames.empty()) {
                                    continue;
                                }
                                if (carouselGames[selGame]->foreign)
                                {
                                    continue;
                                }
                                gui->startingGame = true;
                                gui->runningGame = carouselGames[selGame];
                                gui->lastSelIndex = selGame;
                                gui->resumepoint = -1;
                                gui->lastSet = currentSet;
                                menuVisible = false;

                                gui->emuMode = EMU_RETROARCH;
                            }
                        }
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                        gui->padMapping = gui->mapper.getMappingString(e.jbutton.which);

                        if (state == STATE_GAMES) {
                            if (carouselGames.empty()) {
                                continue;
                            }
                            gui->startingGame = true;
                            gui->runningGame = carouselGames[selGame];
                            gui->lastSelIndex = selGame;
                            gui->resumepoint = -1;
                            gui->lastSet = currentSet;
                            menuVisible = false;

                            gui->emuMode = EMU_PCSX;
                            if (gui->runningGame->foreign)
                            {
                                gui->emuMode = EMU_RETROARCH;
                                gui->lastPlaylist = retroarch_playlist_name;
                            }

                        } else if (state == STATE_SET) {
                            gui->resumingGui = false;
                            if (menu->selOption == 3) {
                                if (carouselGames.empty()) {
                                    continue;
                                }
                                bool resumeAvailable = false;
                                for (int i = 0; i < 4; i++) {
                                    if (carouselGames[selGame]->isResumeSlotActive(i)) {
                                        resumeAvailable = true;
                                    }
                                }

                                if (resumeAvailable) {
                                    Mix_PlayChannel(-1, gui->cursor, 0);
                                    sselector->visible = true;
                                    sselector->loadSaveStateImages(carouselGames[selGame], false);
                                    state = STATE_RESUME;
                                    sselector->selSlot = 0;
                                    sselector->operation = OP_LOAD;
                                } else {
                                    Mix_PlayChannel(-1, gui->cancel, 0);
                                }
                            }
                            if (menu->selOption == 2) {
                                if (carouselGames.empty()) {
                                    continue;
                                }
                                if (carouselGames[selGame]->foreign)
                                {
                                    continue;
                                }

                                string leftCardName = "[1]"+  _("INTERNAL");
                                string rightCardName = "[2]"+  _("INTERNAL");
                                string cardPath1 = carouselGames[selGame]->ssFolder  +"memcards/card1.mcd";
                                string cardPath2 = carouselGames[selGame]->ssFolder  +"memcards/card2.mcd";
                                // Mapped card
                                string memcard = "SONY";
                                if (!carouselGames[selGame]->internal) {
                                    Inifile gameini;
                                    gameini.load(carouselGames[selGame]->folder + "/Game.ini");
                                    memcard = gameini.values["memcard"];

                                }
                                if (memcard!="SONY")
                                {
                                    cardPath1 =  gui->path + sep +"!MemCards/" + memcard  +"card1.mcd";
                                    cardPath1 =  gui->path + sep +"!MemCards/" + memcard  +"card2.mcd";
                                    leftCardName = "[1]"+ memcard;
                                    rightCardName = "[2]"+ memcard;
                                }

                                Mix_PlayChannel(-1, gui->cursor, 0);
                                auto mcManager = new GuiMcManager(renderer);
                                mcManager->backgroundImg=background->tex;
                                mcManager->leftCardName = leftCardName;
                                mcManager->rightCardName = rightCardName;
                                mcManager->card1path = cardPath1;
                                mcManager->card2path = cardPath2;
                                mcManager->show();
                                delete mcManager;
                            }
                            if (menu->selOption == 1) {
                                if (carouselGames.empty()) {
                                    continue;
                                }

                                Mix_PlayChannel(-1, gui->cursor, 0);
                                GuiEditor *editor = new GuiEditor(renderer);
                                editor->internal = carouselGames[selGame]->internal;
                                Inifile gameIni;
                                if (!editor->internal) {
                                    editor->gameFolder = carouselGames[selGame]->folder;

                                    gameIni.load(carouselGames[selGame]->folder + "Game.ini");
                                    string folderNoLast =
                                            carouselGames[selGame]->folder.substr(0,
                                                                                  carouselGames[selGame]->folder.size() -
                                                                                  1);
                                    gameIni.entry = folderNoLast.substr(folderNoLast.find_last_of("//") + 1);
                                    editor->gameIni = gameIni;
                                } else {
                                    editor->gameData = carouselGames[selGame];
                                }

                                editor->show();
                                if (!editor->internal) {
                                    if (editor->changes) {
                                        gameIni.load(carouselGames[selGame]->folder + "Game.ini");
                                        gui->db->updateTitle(carouselGames[selGame]->gameId, gameIni.values["title"]);
                                    }
                                    gui->db->refreshGame(carouselGames[selGame]);
                                    if (currentSet == SET_FAVORITE && editor->gameIni.values["favorite"] == "0") {
                                        gui->lastSet = SET_FAVORITE;
                                        loadAssets();
                                    }
                                } else {
                                    Database *internalDB = new Database();
#if defined(__x86_64__) || defined(_M_X64)
                                    internalDB->connect("internal.db");
#else
                                    internalDB->connect("/media/System/Databases/internal.db");
#endif
                                    if (editor->changes) {
                                        internalDB->updateTitle(carouselGames[selGame]->gameId, editor->lastName);
                                    }
                                    internalDB->refreshGameInternal(carouselGames[selGame]);
                                    internalDB->disconnect();
                                    delete internalDB;
                                }

                                // if the current set is favorites and the user removes the last favorite selGame will be -1
                                if (selGame != -1) {
                                    setInitialPositions(selGame);
                                    updateMeta();
                                    menu->setResumePic(carouselGames[selGame]->findResumePicture());

                                    PsScreenpoint point2;
                                    point2.x = 640 - 113;
                                    point2.y = 90;
                                    point2.scale = 1;
                                    point2.shade = 220;

                                    carouselGames[selGame].destination = point2;
                                    carouselGames[selGame].actual = point2;
                                    carouselGames[selGame].current = point2;
                                }
                                // fix to put back cover on top position

                            } else if (menu->selOption == 0) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                int lastSet = currentSet;
                                int lastGame = selGame;
                                GuiOptions *option = new GuiOptions(renderer);
                                option->show();
                                bool exitCode = option->exitCode;
                                delete option;

                                if (exitCode == 0) {
                                    freeAssets();
                                    loadAssets();
                                    gui->resumingGui = false;
                                    currentSet = lastSet;
                                    selGame = lastGame;
                                    bool resetCarouselPosition = false;
                                    if (gui->cfg.inifile.values["origames"] != "true") {
                                        if (currentSet == SET_INTERNAL) {
                                            currentSet = SET_ALL;
                                            resetCarouselPosition = true;
                                        }
                                    }

                                    switchSet(currentSet,false);
                                    showSetName();

                                    if (resetCarouselPosition) {
                                        if (carouselGames.empty()) {
                                            selGame = -1;
                                            updateMeta();
                                        } else {
                                            selGame = 0;
                                            setInitialPositions(0);
                                            updateMeta();
                                        }
                                    } else {
                                        if (selGame != -1) {
                                            setInitialPositions(selGame);
                                            updateMeta();
                                            menu->setResumePic(carouselGames[selGame]->findResumePicture());
                                        }
                                    }

                                    if (!carouselGames.empty()) {
                                        gui->loadAssets();
                                        for (auto &game : carouselGames) {
                                            game.freeTex();
                                        }
                                        setInitialPositions(selGame);
                                    } else {
                                        gui->loadAssets();
                                        meta->gameName = "";
                                        menu->setResumePic("");
                                    }

                                    state = STATE_GAMES;
                                } else {
                                    render();
                                }
                            }
                        } else if (state == STATE_RESUME) {
                            auto game = carouselGames[selGame];
                            int slot = sselector->selSlot;

                            if (sselector->operation == OP_LOAD) {
                                if (game->isResumeSlotActive(slot)) {
                                    Mix_PlayChannel(-1, gui->cursor, 0);
                                    gui->startingGame = true;
                                    gui->runningGame = carouselGames[selGame];
                                    gui->lastSelIndex = selGame;
                                    gui->resumepoint = slot;
                                    gui->lastSet = currentSet;
                                    sselector->cleanSaveStateImages();
                                    gui->emuMode = EMU_PCSX;
                                    menuVisible = false;
                                } else {
                                    Mix_PlayChannel(-1, gui->cancel, 0);
                                }
                            } else {
                                //Mix_PlayChannel(-1, gui->cursor, 0);
                                PcsxInterceptor *interceptor = new PcsxInterceptor();
                                interceptor->saveResumePoint(carouselGames[selGame], sselector->selSlot);
                                delete interceptor;
                                carouselGames[selGame]->storeResumePicture(sselector->selSlot);
                                sselector->visible = false;
                                arrow->visible = true;
                                Mix_PlayChannel(-1, gui->resume, 0);
                                notificationLines[1].setText(
                                        _("Resume point saved to slot") + " " + to_string(sselector->selSlot + 1),
                                        DefaultShowingTimeout);

                                menu->setResumePic(carouselGames[selGame]->findResumePicture(sselector->selSlot));

                                if (sselector->operation == OP_LOAD) {
                                    state = STATE_SET;
                                } else {
                                    state = STATE_GAMES;
                                }
                            }
                        }
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
                        if (state != STATE_RESUME) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            GuiBtnGuide *guide = new GuiBtnGuide(renderer);
                            guide->backgroundImg = background->tex;
                            guide->show();
                            delete guide;
                        } else {
                            if (sselector->operation == OP_LOAD) {
                                auto game = carouselGames[selGame];
                                int slot = sselector->selSlot;
                                if (game->isResumeSlotActive(slot)) {
                                    Mix_PlayChannel(-1, gui->cursor, 0);

                                    GuiConfirm *confirm = new GuiConfirm(renderer);
                                    confirm->label = _("Are you sure?");
                                    confirm->show();

                                    if (confirm->result) {
                                        game->removeResumePoint(slot);
                                    }
                                    sselector->cleanSaveStateImages();
                                    sselector->loadSaveStateImages(carouselGames[selGame], false);
                                    state = STATE_RESUME;
                                    sselector->selSlot = 0;
                                    sselector->operation = OP_LOAD;
                                    delete (confirm);

                                } else {
                                    Mix_PlayChannel(-1, gui->cancel, 0);
                                }
                            }
                        }
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT, &e)) {
                        if (state == STATE_GAMES) {
                            Mix_PlayChannel(-1, gui->cursor, 0);

                            int previousSet = currentSet;
                            currentSet++;
                            if (previousSet == SET_RETROARCH) {
                                showAllOptions();
                                menuHead->setText(headers[0], fgR, fgG, fgB);
                                menuText->setText(texts[0], fgR, fgG, fgB);
                            }

                            if (gui->cfg.inifile.values["origames"] != "true") {
                                if (currentSet == SET_INTERNAL) {
                                    currentSet = SET_EXTERNAL;
                                }
                            }
                            if (currentSet > SET_LAST) currentSet = 0;
                            switchSet(currentSet,false);
                            showSetName();
                            if (selGame != -1) {
                                updateMeta();
                                menu->setResumePic(carouselGames[selGame]->findResumePicture());
                            } else {
                                updateMeta();
                            }
                        }
                    };
            }
        }
    }

    freeAssets();
}
