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
#include "../environment.h"

using namespace std;

// for PsMenu->selOption
#define SEL_OPTION_AB_SETTINGS 0
#define SEL_OPTION_EDIT_GAME_SETTINGS 1
#define SEL_OPTION_EDIT_MEMCARD_INFO 2
#define SEL_OPTION_RESUME_FROM_SAVESTATE 3


//*******************************
// GuiLauncher::loop
// event loop
//*******************************
void GuiLauncher::loop() {
    cout << "Main Loop" << endl;
    powerOffShift = false;  // L2 shift used for power off and selecting game sub dir or RA playlist

    menuVisible = true;
    motionStart = 0;
    timespeed = 0;
    motionDir = 0;

    while (menuVisible) {
        // get the current translated string values
        headers = {_("SETTINGS"), _("GAME"), _("MEMORY CARD"), _("RESUME")};
        texts = {_("Customize AutoBleem settings"), _("Edit game parameters"),
                 _("Edit Memory Card information"), _("Resume game from saved state point")};
        gui->watchJoystickPort();
        time = SDL_GetTicks();
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
                            nextCarouselGame(60);
                        }
                    } else {
                        if (!scrolling) {
                            prevCarouselGame(60);
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
                    if (powerOffShift)
                        continue;
                    if (gui->mapper.isCenter(&e)) {
                        if (state == STATE_GAMES) {
                            if (carouselGames.empty()) {
                                continue;
                            }
                        }
                        motionStart = 0;
                    }
                    if (gui->mapper.isLeft(&e)) {
                        loop_joyMoveLeft();
                    }
                    else if (gui->mapper.isRight(&e)) {
                        loop_joyMoveRight();
                    }
                    else if (gui->mapper.isUp(&e)) {
                        loop_joyMoveUp();
                    }
                    else if (gui->mapper.isDown(&e)) {
                        loop_joyMoveDown();
                    }
                    else {
                        ;
                    }
                    break;

                case SDL_JOYBUTTONDOWN:
                    loop_joyButtonPressed();    // button pressed
                    break;
                case SDL_JOYBUTTONUP:
                    loop_joyButtonReleased();   // button released
                    break;

            }
        }
    }

    freeAssets();
}

//*******************************
// GuiLauncher::loop_joyMoveLeft
//*******************************
void GuiLauncher::loop_joyMoveLeft() {
    if (state == STATE_GAMES) {
        if (carouselGames.empty()) {
            return;
        }
        if (!scrolling) {
            motionStart = time;
            motionDir = 1;
            scrolling = true;
            prevCarouselGame(110);
        }
    } else if (state == STATE_SET) {

        if (!menu->foreign) {
            if (menu->selOption != SEL_OPTION_AB_SETTINGS) {
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

//*******************************
// GuiLauncher::loop_joyMoveRight
//*******************************
void GuiLauncher::loop_joyMoveRight() {
    if (state == STATE_GAMES) {
        if (carouselGames.empty()) {
            return;
        }
        if (!scrolling) {
            motionStart = time;
            motionDir = 0;
            scrolling = true;
            nextCarouselGame(110);
        }
    } else if (state == STATE_SET) {

        if (!menu->foreign) {
            if (menu->selOption != SEL_OPTION_RESUME_FROM_SAVESTATE) {
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

//*******************************
// GuiLauncher::loop_joyMoveUp
//*******************************
void GuiLauncher::loop_joyMoveUp() {
    if (scrolling) {
        return;
    }
    if (state == STATE_SET) {
        if (menu->animationStarted == 0) {
            menu->transition = TR_MENUON;
            switchState(STATE_GAMES, time);
            motionStart = 0;
        }
    }
}

//*******************************
// GuiLauncher::loop_joyMoveDown
//*******************************
void GuiLauncher::loop_joyMoveDown() {
    if (scrolling) {
        return;
    }
    if (state == STATE_GAMES) {
        if (menu->animationStarted == 0) {
            menu->transition = TR_MENUON;
            switchState(STATE_SET, time);
            motionStart = 0;
        }
    }
}

//*******************************
// GuiLauncher::loop_joyButtonPressed
// button pressed
//*******************************
void GuiLauncher::loop_joyButtonPressed() {
    if (e.jbutton.button == gui->_cb(PCS_BTN_L2, &e)) {
        Mix_PlayChannel(-1, gui->cursor, 0);
        powerOffShift = true;
    }

    if (powerOffShift) {
        if (e.jbutton.button == gui->_cb(PCS_BTN_R2, &e)) {
            Mix_PlayChannel(-1, gui->cursor, 0);
            gui->drawText(_("POWERING OFF... PLEASE WAIT"));
            Util::powerOff();
            return;
        }
    }


    if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT, &e)) {
        loop_selectButtonPressed();
    };

    if (powerOffShift)
        return; // none of the following buttons should work if L2 is pressed

    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
        Mix_PlayChannel(-1, gui->cursor, 0);
        loop_prevGameFirstLetter();
    }
    if (e.jbutton.button == gui->_cb(PCS_BTN_R1, &e)) {
        Mix_PlayChannel(-1, gui->cursor, 0);
        loop_nextGameFirstLetter();
    }
    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
        loop_circleButtonPressed();
    };

    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
        loop_triangleButtonPressed();
    };

    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {
        loop_squareButtonPressed();
    }

    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
        loop_crossButtonPressed();
    };
}

//*******************************
// GuiLauncher::loop_chooseGameDir
//*******************************
void GuiLauncher::loop_chooseGameDir() {
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
    int nextSel = currentUSBGameDirIndex;
    guiGameDirMenu->selected = nextSel;
    guiGameDirMenu->firstVisible = nextSel;
    guiGameDirMenu->lastVisible = nextSel + guiGameDirMenu->maxVisible;

    guiGameDirMenu->show();
    bool cancelled = guiGameDirMenu->cancelled;
    currentUSBGameDirIndex = guiGameDirMenu->selected;
    currentUSBGameDirName = gameRowInfos[currentUSBGameDirIndex].rowName;
    delete guiGameDirMenu;

    if (cancelled)
        return;
    switchSet(currentSet,true);
    menuHead->setText(headers[0], fgR, fgG, fgB);
    menuText->setText(texts[0], fgR, fgG, fgB);
    showSetName();
    if (selGameIndex != -1) {
        updateMeta();
        menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());
    } else {
        updateMeta();
    }
}

//*******************************
// GuiLauncher::loop_chooseRAPlaylist
//*******************************
void GuiLauncher::loop_chooseRAPlaylist() {
    if (!DirEntry::exists(Env::getPathToRetroarchDir())) {
        return;
    }
    if (raPlaylists.empty()) {
        return;
    }
    powerOffShift = false;
    auto playlists = new GuiPlaylists(renderer);
    playlists->playlists = raPlaylists;
    playlists->backgroundImg = background->tex;
    playlists->integrator = &raIntegrator;
    int nextSel = 0;
    int i = 0;
    for (string plist:playlists->playlists) {
        if (plist == currentRAPlaylistName) {
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

    if (cancelled)
        return;

    currentRAPlaylistIndex = selected;
    currentRAPlaylistName = raPlaylists[selected];
    currentSet = SET_RETROARCH;
    switchSet(currentSet,false);
    menuHead->setText(headers[0], fgR, fgG, fgB);
    menuText->setText(texts[0], fgR, fgG, fgB);
    showSetName();
    if (selGameIndex != -1) {
        updateMeta();
        menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());
    } else {
        updateMeta();
    }
}

//*******************************
// GuiLauncher::loop_selectButtonPressed
//*******************************
void GuiLauncher::loop_selectButtonPressed() {
    if (state == STATE_GAMES) {
        if (powerOffShift) {
            if (currentSet == SET_EXTERNAL)
                loop_chooseGameDir();
            else if (currentSet == SET_RETROARCH)
                loop_chooseRAPlaylist();
            else
                return; // if L2 is pressed then Select should only work if current_set is SET_EXTERNAL or SET_RETROARCH
        }
        else {
            // switch to next Select Mode
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
            if (selGameIndex != -1) {
                updateMeta();
                menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());
            } else {
                updateMeta();
            }
        }
    }
}

//*******************************
// GuiLauncher::loop_circleButtonPressed
//*******************************
void GuiLauncher::loop_circleButtonPressed() {
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
        menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());

        if (sselector->operation == OP_LOAD) {
            state = STATE_SET;
        } else {
            state = STATE_GAMES;
        }
    }
}

//*******************************
// GuiLauncher::loop_triangleButtonPressed
//*******************************
void GuiLauncher::loop_triangleButtonPressed() {
    if (state != STATE_RESUME) {
        Mix_PlayChannel(-1, gui->cursor, 0);
        GuiBtnGuide *guide = new GuiBtnGuide(renderer);
        guide->backgroundImg = background->tex;
        guide->show();
        delete guide;
    } else {
        if (sselector->operation == OP_LOAD) {
            auto game = carouselGames[selGameIndex];
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
                sselector->loadSaveStateImages(carouselGames[selGameIndex], false);
                state = STATE_RESUME;
                sselector->selSlot = 0;
                sselector->operation = OP_LOAD;
                delete (confirm);

            } else {
                Mix_PlayChannel(-1, gui->cancel, 0);
            }
        }
    }
}

//*******************************
// GuiLauncher::loop_squareButtonPressed
//*******************************
void GuiLauncher::loop_squareButtonPressed() {
    gui->padMapping = gui->mapper.getMappingString(e.jbutton.which);

    if (DirEntry::exists(Env::getPathToRetroarchDir() + sep + "retroarch")) { // retroarch is a file!!

        if (state == STATE_GAMES) {
            if (carouselGames.empty()) {
                return;
            }
            if (carouselGames[selGameIndex]->foreign)
            {
                return;
            }
            gui->startingGame = true;
            gui->runningGame = carouselGames[selGameIndex];
            gui->lastSelIndex = selGameIndex;
            gui->resumepoint = -1;
            gui->lastSet = currentSet;
            gui->lastUSBGameDirIndex = currentUSBGameDirIndex;
            gui->lastRAPlaylistIndex = currentRAPlaylistIndex;
            menuVisible = false;

            gui->emuMode = EMU_RETROARCH;
        }
    }
}

//*******************************
// GuiLauncher::loop_crossButtonPressed
//*******************************
void GuiLauncher::loop_crossButtonPressed() {
    gui->padMapping = gui->mapper.getMappingString(e.jbutton.which);

    if (state == STATE_GAMES) {
        loop_crossButtonPressed_STATE_GAMES();

    } else if (state == STATE_SET) {
        loop_crossButtonPressed_STATE_SET();

    } else if (state == STATE_RESUME) {
        loop_crossButtonPressed_STATE_RESUME();

    }
}

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_GAMES
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_GAMES() {
    if (carouselGames.empty()) {
        return;
    }

    gui->startingGame = true;
    gui->runningGame = carouselGames[selGameIndex];
    gui->lastSelIndex = selGameIndex;
    gui->resumepoint = -1;
    gui->lastSet = currentSet;
    gui->lastUSBGameDirIndex = currentUSBGameDirIndex;
    gui->lastRAPlaylistIndex = currentRAPlaylistIndex;
    menuVisible = false;

    gui->emuMode = EMU_PCSX;
    if (gui->runningGame->foreign)
    {
        gui->emuMode = EMU_RETROARCH;
        gui->lastRAPlaylistIndex = currentRAPlaylistIndex;
        gui->lastRAPlaylistName = currentRAPlaylistName;
    }
}

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_SET
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_SET() {
    gui->resumingGui = false;
    if (menu->selOption == SEL_OPTION_RESUME_FROM_SAVESTATE) {
        loop_crossButtonPressed_STATE_SET__OPT_RESUME_FROM_SAVESTATE();
    }
    else if (menu->selOption == SEL_OPTION_EDIT_MEMCARD_INFO) {
        loop_crossButtonPressed_STATE_SET__OPT_EDIT_MEMCARD();
    }
    else if (menu->selOption == SEL_OPTION_EDIT_GAME_SETTINGS) {
        loop_crossButtonPressed_STATE_SET__OPT_EDIT_GAME_SETTINGS();
    }
    else if (menu->selOption == SEL_OPTION_AB_SETTINGS) {
        loop_crossButtonPressed_STATE_SET__OPT_AB_SETTINGS();
    }
}

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_AB_SETTINGS
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_AB_SETTINGS() {
    Mix_PlayChannel(-1, gui->cursor, 0);
    int lastSet = currentSet;
    int lastUSBGameDirIndex = currentUSBGameDirIndex;
    int lastRAPlaylistIndex = currentRAPlaylistIndex;
    int lastGame = selGameIndex;
    GuiOptions *option = new GuiOptions(renderer);
    option->show();
    bool exitCode = option->exitCode;
    delete option;

    if (exitCode == 0) {
        freeAssets();
        loadAssets();
        gui->resumingGui = false;
        currentSet = lastSet;
        currentUSBGameDirIndex = lastUSBGameDirIndex;
        currentRAPlaylistIndex = lastRAPlaylistIndex;
        selGameIndex = lastGame;
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
                selGameIndex = -1;
                updateMeta();
            } else {
                selGameIndex = 0;
                setInitialPositions(0);
                updateMeta();
            }
        } else {
            if (selGameIndex != -1) {
                setInitialPositions(selGameIndex);
                updateMeta();
                menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());
            }
        }

        if (!carouselGames.empty()) {
            gui->loadAssets();
            for (auto &game : carouselGames) {
                game.freeTex();
            }
            setInitialPositions(selGameIndex);
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

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_EDIT_GAME_SETTINGS
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_EDIT_GAME_SETTINGS() {
    if (carouselGames.empty()) {
        return;
    }

    Mix_PlayChannel(-1, gui->cursor, 0);
    GuiEditor *editor = new GuiEditor(renderer);
    editor->internal = carouselGames[selGameIndex]->internal;
    Inifile gameIni;
    if (!editor->internal) {
        editor->gameFolder = carouselGames[selGameIndex]->folder;
        editor->gameData = carouselGames[selGameIndex];
        gameIni.load(carouselGames[selGameIndex]->folder + sep + GAME_INI);
        string folderNoLast = DirEntry::removeSeparatorFromEndOfPath(carouselGames[selGameIndex]->folder);
        // change "/media/Games/Racing/Driver 2" to "Driver 2"
        gameIni.entry = DirEntry::getFileNameFromPath(folderNoLast);
        editor->gameIni = gameIni;
    } else {
        editor->gameData = carouselGames[selGameIndex];
    }

    editor->show();
    if (!editor->internal) {
        if (editor->changes) {
            gameIni.load(carouselGames[selGameIndex]->folder + sep + GAME_INI);
            gui->db->updateTitle(carouselGames[selGameIndex]->gameId, gameIni.values["title"]);
        }
        gui->db->refreshGame(carouselGames[selGameIndex]);
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
            internalDB->updateTitle(carouselGames[selGameIndex]->gameId, editor->lastName);
        }
        internalDB->refreshGameInternal(carouselGames[selGameIndex]);
        internalDB->disconnect();
        delete internalDB;
    }

    // if the current set is favorites and the user removes the last favorite selGameIndex will be -1
    if (selGameIndex != -1) {
        setInitialPositions(selGameIndex);
        updateMeta();
        menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());

        PsScreenpoint point2;
        point2.x = 640 - 113;
        point2.y = 90;
        point2.scale = 1;
        point2.shade = 220;

        carouselGames[selGameIndex].destination = point2;
        carouselGames[selGameIndex].actual = point2;
        carouselGames[selGameIndex].current = point2;
    }
    // fix to put back cover on top position
}

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_EDIT_MEMCARD
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_EDIT_MEMCARD() {
    if (carouselGames.empty()) {
        return;
    }
    if (carouselGames[selGameIndex]->foreign)
    {
        return;
    }

    string leftCardName = "[1]"+  _("INTERNAL");
    string rightCardName = "[2]"+  _("INTERNAL");
    string cardPath1 = carouselGames[selGameIndex]->ssFolder  +"memcards/card1.mcd";
    string cardPath2 = carouselGames[selGameIndex]->ssFolder  +"memcards/card2.mcd";
    // Mapped card
    string memcard = "SONY";
    if (!carouselGames[selGameIndex]->internal) {
        Inifile gameini;
        gameini.load(carouselGames[selGameIndex]->folder + sep + GAME_INI);
        memcard = gameini.values["memcard"];

    }
    if (memcard!="SONY")
    {
        cardPath1 =  Env::getPathToMemCardsDir() + sep + memcard  +"card1.mcd";
        cardPath1 =  Env::getPathToMemCardsDir() + sep + memcard  +"card2.mcd";
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

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_RESUME_FROM_SAVESTATE
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_SET__OPT_RESUME_FROM_SAVESTATE() {
    if (carouselGames.empty()) {
        return;
    }
    bool resumeAvailable = false;
    for (int i = 0; i < 4; i++) {
        if (carouselGames[selGameIndex]->isResumeSlotActive(i)) {
            resumeAvailable = true;
        }
    }

    if (resumeAvailable) {
        Mix_PlayChannel(-1, gui->cursor, 0);
        sselector->visible = true;
        sselector->loadSaveStateImages(carouselGames[selGameIndex], false);
        state = STATE_RESUME;
        sselector->selSlot = 0;
        sselector->operation = OP_LOAD;
    } else {
        Mix_PlayChannel(-1, gui->cancel, 0);
    }
}

//*******************************
// GuiLauncher::loop_crossButtonPressed_STATE_RESUME
//*******************************
void GuiLauncher::loop_crossButtonPressed_STATE_RESUME() {
    auto game = carouselGames[selGameIndex];
    int slot = sselector->selSlot;

    if (sselector->operation == OP_LOAD) {
        if (game->isResumeSlotActive(slot)) {
            Mix_PlayChannel(-1, gui->cursor, 0);
            gui->startingGame = true;
            gui->runningGame = carouselGames[selGameIndex];
            gui->lastSelIndex = selGameIndex;
            gui->resumepoint = slot;
            gui->lastSet = currentSet;
            gui->lastUSBGameDirIndex = currentUSBGameDirIndex;
            gui->lastRAPlaylistIndex = currentRAPlaylistIndex;
            sselector->cleanSaveStateImages();
            gui->emuMode = EMU_PCSX;
            menuVisible = false;
        } else {
            Mix_PlayChannel(-1, gui->cancel, 0);
        }
    } else {
        //Mix_PlayChannel(-1, gui->cursor, 0);
        PcsxInterceptor *interceptor = new PcsxInterceptor();
        interceptor->saveResumePoint(carouselGames[selGameIndex], sselector->selSlot);
        delete interceptor;
        carouselGames[selGameIndex]->storeResumePicture(sselector->selSlot);
        sselector->visible = false;
        arrow->visible = true;
        Mix_PlayChannel(-1, gui->resume, 0);
        notificationLines[1].setText(
                _("Resume point saved to slot") + " " + to_string(sselector->selSlot + 1),
                DefaultShowingTimeout);

        menu->setResumePic(carouselGames[selGameIndex]->findResumePicture(sselector->selSlot));

        if (sselector->operation == OP_LOAD) {
            state = STATE_SET;
        } else {
            state = STATE_GAMES;
        }
    }
}

//*******************************
// GuiLauncher::loop_joyButtonReleased
// button released
//*******************************
void GuiLauncher::loop_joyButtonReleased() {
    if (e.jbutton.button == gui->_cb(PCS_BTN_L2, &e)) {
        Mix_PlayChannel(-1, gui->cursor, 0);
        powerOffShift = false;
    }
}
//*******************************
// GuiLauncher::loop_prevGameFirstLetter
//*******************************
// event loop
void GuiLauncher::loop_prevGameFirstLetter() {
    if (state == STATE_GAMES) {
        if (carouselGames.empty()) {
            return;
        }
        // find prev game
        int nextGame = selGameIndex;
        string currentFirst = carouselGames[selGameIndex]->title.substr(0, 1);
        string futureFirst = carouselGames[selGameIndex]->title.substr(0, 1);
        for (int i = selGameIndex; i >= 0; i--) {
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
        if (nextGame != selGameIndex) {
            // we have next game;
            Mix_PlayChannel(-1, gui->cursor, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
            selGameIndex = nextGame;
            setInitialPositions(selGameIndex);
            updateMeta();
            menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());
        } else {
            Mix_PlayChannel(-1, gui->cancel, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
        }
    }
}

//*******************************
// GuiLauncher::loop_nextGameFirstLetter
//*******************************
// event loop
void GuiLauncher::loop_nextGameFirstLetter() {
    if (state == STATE_GAMES) {
        if (carouselGames.empty()) {
            return;
        }
        // find next game
        int nextGame = selGameIndex;
        string currentFirst = carouselGames[selGameIndex]->title.substr(0, 1);
        string futureFirst = carouselGames[selGameIndex]->title.substr(0, 1);
        for (int i = selGameIndex; i < carouselGames.size(); i++) {
            futureFirst = carouselGames[i]->title.substr(0, 1);
            if (currentFirst != futureFirst) {
                nextGame = i;
                break;
            }
        }
        if (nextGame != selGameIndex) {
            // we have next game;
            Mix_PlayChannel(-1, gui->cursor, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
            selGameIndex = nextGame;
            setInitialPositions(selGameIndex);
            updateMeta();
            menu->setResumePic(carouselGames[selGameIndex]->findResumePicture());
        } else {
            Mix_PlayChannel(-1, gui->cancel, 0);
            notificationLines[1].setText(futureFirst, DefaultShowingTimeout, brightWhite, FONT_24);
        }
    }
}

