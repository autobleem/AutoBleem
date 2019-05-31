//
// Created by screemer on 2019-01-25.
//

#include "gui_editor.h"
#include "gui.h"
#include "gui_keyboard.h"
#include "gui_selectmemcard.h"
#include "../engine/memcard.h"
#include "../engine/cfgprocessor.h"
#include <SDL2/SDL_image.h>
#include "../lang.h"
#include <sstream>

using namespace std;

#define OPT_FAVORITE       5
#define OPT_LOCK           6
#define OPT_HIGHRES        7
#define OPT_SPEEDHACK      8
#define OPT_SCANLINES      9
#define OPT_SCANLINELV     10
#define OPT_CLOCK_PSX      11
#define OPT_FRAMESKIP      12
#define OPT_PLUGIN         13
#define OPT_INTERPOLATION  14

//*******************************
// GuiEditor::processOptionChange
//*******************************
void GuiEditor::processOptionChange(bool direction) {
    shared_ptr<Gui> gui(Gui::getInstance());
    CfgProcessor *processor = new CfgProcessor();

    string path = gui->path;
    if (internal) {
        path = gameData->ssFolder;
    }
    stringstream ss;
    string s;

    switch (selOption) {
        case OPT_FAVORITE:
            if (!internal) {
                if (gameIni.values["favorite"] == "")
                    gameIni.values["favorite"] = "0";   // doesn't exist yet in this ini so set to 0
                if (direction == true) {
                    if (gameIni.values["favorite"] == "0") {
                        gameIni.values["favorite"] = "1";
                    }
                } else {
                    if (gameIni.values["favorite"] == "1") {
                        gameIni.values["favorite"] = "0";
                    }
                }
                gameIni.save(gameIni.path);
            }
            break;

        case OPT_LOCK:
            if (!internal) {
                if (direction == true) {
                    if (gameIni.values["automation"] == "1") {
                        gameIni.values["automation"] = "0";
                    }
                } else {
                    if (gameIni.values["automation"] == "0") {
                        gameIni.values["automation"] = "1";
                    }
                }
                gameIni.save(gameIni.path);
            }
            break;

        case OPT_HIGHRES:
            if (direction == false) {
                if (highres == 1) {
                    highres = 0;
                }
            } else {
                if (highres == 0) {
                    highres = 1;
                }
            }
            gameIni.values["highres"] = to_string(highres);

            processor->replace(gameIni.entry, path, "gpu_neon.enhancement_enable",
                               "gpu_neon.enhancement_enable = " + gameIni.values["highres"], internal);
            if (!internal) {
                gameIni.save(gameIni.path);
            }

            refreshData();
            break;

        case OPT_SPEEDHACK:
            if (direction == false) {
                if (speedhack == 1) {
                    speedhack = 0;
                }
            } else {
                if (speedhack == 0) {
                    speedhack = 1;
                }
            }

            processor->replace(gameIni.entry, path, "gpu_neon.enhancement_no_main",
                               "gpu_neon.enhancement_no_main = " + to_string(speedhack), internal);
            refreshData();
            break;

        case OPT_SCANLINES:
            if (direction == false) {
                if (scanlines == 1) {
                    scanlines = 0;
                }
            } else {
                if (scanlines == 0) {
                    scanlines = 1;
                }
            }
            processor->replace(gameIni.entry, path, "scanlines",
                               "scanlines = " + to_string(scanlines), internal);
            refreshData();
            break;

        case OPT_SCANLINELV:
            if (direction == true) {
                scanlineLevel++;
                if (scanlineLevel > 100) {
                    scanlineLevel = 100;
                }
            } else {
                scanlineLevel--;
                if (scanlineLevel < 0) {
                    scanlineLevel = 0;
                }
            }

            ss << std::hex << scanlineLevel;
            s = ss.str();

            processor->replace(gameIni.entry, path, "scanline_level",
                               "scanline_level = " + s, internal);
            refreshData();
            break;

        case OPT_CLOCK_PSX:
            if (direction == true) {
                clock++;
                if (clock > 100) {
                    clock = 100;
                }

            } else {
                clock--;
                if (clock < 0) {
                    clock = 0;
                }
            }

            ss << std::hex << clock;
            s = ss.str();

            processor->replace(gameIni.entry, path, "psx_clock",
                               "psx_clock = " + s, internal);
            refreshData();
            break;

        case OPT_FRAMESKIP:
            if (direction == true) {

                frameskip++;
                if (frameskip > 3) {
                    frameskip = 3;
                }
            } else {
                frameskip--;
                if (frameskip < 0) {
                    frameskip = 0;
                }
            }

            ss << std::hex << frameskip;
            s = ss.str();

            processor->replace(gameIni.entry, path, "frameskip3",
                               "frameskip3 = " + s, internal);
            refreshData();
            break;

        case OPT_INTERPOLATION:
            if (direction == true) {
                interpolation++;
                if (interpolation > 3) {
                    interpolation = 3;
                }
            } else {
                interpolation--;
                if (interpolation < 0) {
                    interpolation = 0;
                }
            }

            ss << std::hex << interpolation;
            s = ss.str();

            processor->replace(gameIni.entry, path, "spu_config.iUseInterpolation",
                               "spu_config.iUseInterpolation = " + s, internal);
            refreshData();
            break;

        case OPT_PLUGIN:
            if (!internal) {
                if (direction == true) {
                    gpu = "gpu_peops.so";
                } else {
                    gpu = "builtin_gpu";
                }
                processor->replace(gameIni.entry, path, "Gpu3",
                                   "Gpu3 = " + gpu, internal);
                refreshData();
            }
            break;
    }
    delete (processor);
}

//*******************************
// GuiEditor::refreshData
//*******************************
void GuiEditor::refreshData() {
    shared_ptr<Gui> gui(Gui::getInstance());
    CfgProcessor *processor = new CfgProcessor();
    string path = gui->path;
    if (internal) {
        path = gameData->ssFolder;
    }
    highres = atoi(processor->getValue(gameIni.entry, path, "gpu_neon.enhancement_enable", internal).c_str());
    speedhack = atoi(processor->getValue(gameIni.entry, path, "gpu_neon.enhancement_no_main", internal).c_str());
    clock = strtol(processor->getValue(gameIni.entry, path, "psx_clock", internal).c_str(), NULL, 16);
    gpu = processor->getValue(gameIni.entry, path, "gpu3", internal);
    frameskip = atoi(processor->getValue(gameIni.entry, path, "frameskip3", internal).c_str());
    dither = atoi(processor->getValue(gameIni.entry, path, "gpu_peops.iUseDither", internal).c_str());
    scanlines = atoi(processor->getValue(gameIni.entry, path, "scanlines", internal).c_str());
    scanlineLevel = strtol(processor->getValue(gameIni.entry, path, "scanline_level", internal).c_str(), NULL, 16);
    interpolation = strtol(processor->getValue(gameIni.entry, path, "spu_config.iUseInterpolation", internal).c_str(),
                           NULL, 16);

    delete processor;
}

//*******************************
// GuiEditor::init
//*******************************
void GuiEditor::init() {
    shared_ptr<Gui> gui(Gui::getInstance());
    if (!internal) {
        if (this->gameIni.values["memcard"] != "SONY") {
            string cardpath =
                    gui->path + Util::separator() + "!MemCards" + Util::separator() + this->gameIni.values["memcard"];
            if (!Util::exists(cardpath)) {
                this->gameIni.values["memcard"] = "SONY";
            }
        }

        bool pngLoaded = false;
        for (DirEntry entry:Util::diru(gui->path + Util::separator() + gameIni.entry)) {
            if (Util::matchExtension(entry.name, EXT_PNG)) {
                cover = IMG_LoadTexture(renderer, (gui->path + Util::separator() + gameIni.entry + Util::separator() +
                                                   entry.name).c_str());
                pngLoaded = true;
            }
        }
        if (!pngLoaded) {
            cover = IMG_LoadTexture(renderer, (Util::getWorkingPath() + Util::separator() + "default.png").c_str());
        }
    } else {
        // recover ini
        this->gameIni.values["title"] = gameData->title;
        this->gameIni.values["publisher"] = gameData->publisher;
        this->gameIni.values["year"] = to_string(gameData->year);
        this->gameIni.values["players"] = to_string(gameData->players);
        this->gameIni.values["memcard"] = gameData->memcard;

        if (this->gameIni.values["memcard"] != "SONY") {
            string cardpath =
                    gui->path + Util::separator() + "!MemCards" + Util::separator() + this->gameIni.values["memcard"];
            if (!Util::exists(cardpath)) {
                this->gameIni.values["memcard"] = "SONY";
            }
        }

        bool pngLoaded = false;
        for (DirEntry entry:Util::diru(gameData->folder)) {
            if (Util::matchExtension(entry.name, EXT_PNG)) {
                cover = IMG_LoadTexture(renderer, (gameData->folder + Util::separator() +
                                                   entry.name).c_str());
                pngLoaded = true;
            }
        }
        if (!pngLoaded) {
            cover = IMG_LoadTexture(renderer, (Util::getWorkingPath() + Util::separator() + "default.png").c_str());
        }
    }

    refreshData();
}

//*******************************
// GuiEditor::render
//*******************************
void GuiEditor::render() {
    shared_ptr<Gui> gui(Gui::getInstance());

    int line = 0;
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);

    // Game.ini

    gui->renderTextLine("-=" + gameIni.values["title"] + "=-", line++, offset, true);

    if (!internal) {
        gui->renderTextLine(_("Folder:") + " " + gameIni.entry + "", line++, offset, true);
    } else {
        gui->renderTextLine(_("Folder:") + " " + gameData->folder + "", line++, offset, true);
    }

    gui->renderTextLine(_("Published by:") + " " + gameIni.values["publisher"], line++, offset, true);

    gui->renderTextLine(_("Year:") +" "+ gameIni.values["year"] + "   " + _("Players:") + " " +
                        gameIni.values["players"], line++, offset, true);

    gui->renderTextLine(_("Memory Card:") + " " +
                        (gameIni.values["memcard"] == "SONY" ? string(_("Internal")) : gameIni.values["memcard"] + " " +
                                                                                    _("(Custom)")),
                        line++, offset, true);

    gui->renderTextLineOptions(
            _("Favorite:") + (gameIni.values["favorite"] == "1" ? string("|@Check|") : string("|@Uncheck|")),
            line++, offset,
            false, 300);

    // pcsx.cfg

    gui->renderTextLineOptions(
            _("Lock data:") + (gameIni.values["automation"] == "0" ? string("|@Check|") : string("|@Uncheck|")),
            line++, offset,
            false, 300);

    gui->renderTextLineOptions(_("High res:") + (highres == 1 ? string("|@Check|") : string("|@Uncheck|")), line++, offset,
                               false, 300);

    gui->renderTextLineOptions(_("SpeedHack:") + (speedhack == 1 ? string("|@Check|") : string("|@Uncheck|")), line++,
                               offset, false, 300);
    gui->renderTextLineOptions(_("Scanlines:") + (scanlines == 1 ? string("|@Check|") : string("|@Uncheck|")), line++,
                               offset, false, 300);
    gui->renderTextLineOptions(_("Scanline Level:") + " " + to_string(scanlineLevel), line++, offset, false, 300);
    gui->renderTextLineOptions(_("Clock:") + " " + to_string(clock), line++, offset, false, 300);
    gui->renderTextLineOptions(_("Frameskip:") + " " + to_string(frameskip), line++, offset, false, 300);
    if (!internal) {
        gui->renderTextLineOptions(_("Plugin:") + gpu, line++, offset, false, 300);
    }
    gui->renderTextLineOptions(_("Spu Interpolation:") + to_string(interpolation), line++, offset, false, 300);

    gui->renderSelectionBox(selOption, offset, 300);

    string guiMenu = "|@T| " + _("Rename");

    if (!internal) {
        guiMenu += "  |@S| " + _("Change MC") + " ";

        if (gameIni.values["memcard"] == "SONY") {
            guiMenu += "|@Start| " + _("Share MC") + "  ";
        }
    }

    guiMenu += " |@O| " + _("Go back") + "|";

    gui->renderStatus(guiMenu);

    SDL_Rect rect;
    rect.x = atoi(gui->themeData.values["ecoverx"].c_str());
    rect.y = atoi(gui->themeData.values["ecovery"].c_str());
    rect.w = 226;
    rect.h = 226;

    SDL_RenderCopy(renderer, cover, NULL, &rect);

    SDL_RenderPresent(renderer);
}

//*******************************
// GuiEditor::loop
//*******************************
void GuiEditor::loop() {
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
                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                case SDL_JOYHATMOTION:

                    if (gui->mapper.isDown(&e)) {

                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selOption++;
                        if (selOption > 14) {
                            selOption = 14;
                        }
                        render();
                    }
                    if (gui->mapper.isUp(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selOption--;
                        if (selOption < 5) {
                            selOption = 5;
                        }
                        render();
                    }


                    if (gui->mapper.isRight(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        processOptionChange(true);

                        render();
                    }
                    if (gui->mapper.isLeft(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        processOptionChange(false);

                        render();
                    }
                    break;

                case SDL_JOYBUTTONDOWN:
                    if (!internal) {
                        if (gameIni.values["memcard"] == "SONY") {
                            if (e.jbutton.button == gui->_cb(PCS_BTN_START, &e)) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                                keyboard->label = _("Enter new name for memory card");
                                keyboard->result = gameIni.values["title"];
                                keyboard->show();
                                string result = keyboard->result;
                                bool cancelled = keyboard->cancelled;
                                delete (keyboard);

                                if (result.empty()) {
                                    cancelled = true;
                                }

                                if (!cancelled) {
                                    Memcard *memcard = new Memcard(gui->path);
                                    string savePath =
                                            gui->path + Util::separator() + "!SaveStates" + Util::separator() +
                                            gameIni.entry +
                                            Util::separator() + "memcards";
                                    memcard->storeToRepo(savePath, result);
                                    gameIni.values["memcard"] = result;
                                    gameIni.save(gameIni.path);
                                }
                                render();
                            };
                        }
                    } else {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                    }


                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {
                        if (!internal) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            GuiSelectMemcard *selector = new GuiSelectMemcard(renderer);
                            selector->cardSelected = gameIni.values["memcard"];
                            selector->show();

                            if (selector->selected != -1)
                                if (selector->selected == 0) {
                                    gameIni.values["memcard"] = "SONY";
                                    gameIni.save(gameIni.path);
                                } else {
                                    gameIni.values["memcard"] = selector->cards[selector->selected];
                                    gameIni.save(gameIni.path);
                                }
                            delete (selector);
                        } else {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                        }
                        render();
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        SDL_DestroyTexture(cover);
                        cover = nullptr;
                        menuVisible = false;

                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = _("Enter new game name");
                        keyboard->result = gameIni.values["title"];
                        keyboard->show();
                        string result = keyboard->result;
                        bool cancelled = keyboard->cancelled;
                        delete (keyboard);

                        if (result.empty()) {
                            cancelled = true;
                        }

                        if (!cancelled) {
                            if (!internal) {
                                gameIni.values["title"] = result;
                                gameIni.values["automation"] = "0";
                                gameIni.save(gameIni.path);
                                changes = true;
                            } else {
                                lastName = result;
                                changes = true;
                            }
                        }
                        refreshData();
                        render();
                    };
            }
        }
    }
}