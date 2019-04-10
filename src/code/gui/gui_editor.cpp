//
// Created by screemer on 2019-01-25.
//

#include "gui_editor.h"
#include "gui.h"
#include "gui_keyboard.h"
#include "gui_selectmemcard.h"
#include "../engine/memcard.h"
#include "../engine/cfgprocessor.h"
#include "../lang.h"

#define OPT_LOCK           5
#define OPT_HIGHRES        6
#define OPT_SPEEDHACK      7
#define OPT_SCANLINES      8
#define OPT_SCANLINELV     9
#define OPT_CLOCK_PSX      10
#define OPT_FRAMESKIP      11
#define OPT_PLUGIN         12
#define OPT_INTERPOLATION  13


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
        case OPT_LOCK:
            if (!internal) {
                if (direction == true) {
                    if (game.values["automation"] == "1") {
                        game.values["automation"] = "0";
                    }
                } else {
                    if (game.values["automation"] == "0") {
                        game.values["automation"] = "1";
                    }
                }
                game.save(game.path);
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
            game.values["highres"] = to_string(highres);


            processor->replace(game.entry, path, "gpu_neon.enhancement_enable",
                               "gpu_neon.enhancement_enable = " + game.values["highres"], internal);
            if (!internal) {
                game.save(game.path);
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

            processor->replace(game.entry, path, "gpu_neon.enhancement_no_main",
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
            processor->replace(game.entry, path, "scanlines",
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

            processor->replace(game.entry, path, "scanline_level",
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

            processor->replace(game.entry, path, "psx_clock",
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

            processor->replace(game.entry, path, "frameskip3",
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

            processor->replace(game.entry, path, "spu_config.iUseInterpolation",
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
                processor->replace(game.entry, path, "Gpu3",
                                   "Gpu3 = " + gpu, internal);
                refreshData();
            }
            break;

    }
    delete (processor);
}

void GuiEditor::refreshData() {
    shared_ptr<Gui> gui(Gui::getInstance());
    CfgProcessor *processor = new CfgProcessor();
    string path = gui->path;
    if (internal) {
        path = gameData->ssFolder;
    }
    highres = atoi(processor->getValue(game.entry, path, "gpu_neon.enhancement_enable", internal).c_str());
    speedhack = atoi(processor->getValue(game.entry, path, "gpu_neon.enhancement_no_main", internal).c_str());
    clock = strtol(processor->getValue(game.entry, path, "psx_clock", internal).c_str(), NULL, 16);
    gpu = processor->getValue(game.entry, path, "gpu3", internal);
    frameskip = atoi(processor->getValue(game.entry, path, "frameskip3", internal).c_str());
    dither = atoi(processor->getValue(game.entry, path, "gpu_peops.iUseDither", internal).c_str());
    scanlines = atoi(processor->getValue(game.entry, path, "scanlines", internal).c_str());
    scanlineLevel = strtol(processor->getValue(game.entry, path, "scanline_level", internal).c_str(), NULL, 16);
    interpolation = strtol(processor->getValue(game.entry, path, "spu_config.iUseInterpolation", internal).c_str(),
                           NULL,
                           16);

    delete processor;
}

void GuiEditor::init() {
    shared_ptr<Gui> gui(Gui::getInstance());
    if (!internal) {
        if (this->game.values["memcard"] != "SONY") {
            string cardpath =
                    gui->path + Util::separator() + "!MemCards" + Util::separator() + this->game.values["memcard"];
            if (!Util::exists(cardpath)) {
                this->game.values["memcard"] = "SONY";
            }
        }

        bool pngLoaded = false;
        for (DirEntry entry:Util::diru(gui->path + Util::separator() + game.entry)) {
            if (Util::matchExtension(entry.name, EXT_PNG)) {
                cover = IMG_LoadTexture(renderer, (gui->path + Util::separator() + game.entry + Util::separator() +
                                                   entry.name).c_str());
                pngLoaded = true;
            }
        }
        if (!pngLoaded) {
            cover = IMG_LoadTexture(renderer, (Util::getWorkingPath() + Util::separator() + "default.png").c_str());
        }
    } else {
        // recover ini
        this->game.values["title"] = gameData->title;
        this->game.values["publisher"] = gameData->publisher;
        this->game.values["year"] = to_string(gameData->year);
        this->game.values["players"] = to_string(gameData->players);
        this->game.values["memcard"] = gameData->memcard;

        if (this->game.values["memcard"] != "SONY") {
            string cardpath =
                    gui->path + Util::separator() + "!MemCards" + Util::separator() + this->game.values["memcard"];
            if (!Util::exists(cardpath)) {
                this->game.values["memcard"] = "SONY";
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

void GuiEditor::render() {
    shared_ptr<Gui> gui(Gui::getInstance());


    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + game.values["title"] + "=-", 0, offset, true);
    if (!internal) {
        gui->renderTextLine(_("Folder:") + " " + game.entry + "", 1, offset, true);
    } else {
        gui->renderTextLine(_("Folder:") + " " + gameData->folder + "", 1, offset, true);
    }
    gui->renderTextLine(_("Published by:") + " " + game.values["publisher"], 2, offset, true);
    gui->renderTextLine(_("Year:") + game.values["year"] + "   " + _("Players:") +
                        game.values["players"], 3, offset, true);


    gui->renderTextLine(_("Memory Card:") + " " +
                        (game.values["memcard"] == "SONY" ? string(_("Internal")) : game.values["memcard"] +
                                                                                    _("(Custom)")),
                        4, offset, true);

    gui->renderTextLineOptions(
            _("Lock data:") + (game.values["automation"] == "0" ? string("|@Check|") : string("|@Uncheck|")),
            5, offset,
            false, 300);

    gui->renderTextLineOptions(_("High res:") + (highres == 1 ? string("|@Check|") : string("|@Uncheck|")), 6, offset,
                               false, 300);

    gui->renderTextLineOptions(_("SpeedHack:") + (speedhack == 1 ? string("|@Check|") : string("|@Uncheck|")), 7,
                               offset, false, 300);
    gui->renderTextLineOptions(_("Scanlines:") + (scanlines == 1 ? string("|@Check|") : string("|@Uncheck|")), 8,
                               offset, false, 300);
    gui->renderTextLineOptions(_("Scanline Level:") + " " + to_string(scanlineLevel), 9, offset, false, 300);
    gui->renderTextLineOptions(_("Clock:") + " " + to_string(clock), 10, offset, false, 300);
    gui->renderTextLineOptions(_("Frameskip:") + " " + to_string(frameskip), 11, offset, false, 300);
    if (!internal) {
        gui->renderTextLineOptions(_("Plugin:") + gpu, 12, offset, false, 300);
    }
    gui->renderTextLineOptions(_("Spu Interpolation:") + to_string(interpolation), 13, offset, false, 300);

    gui->renderSelectionBox(selOption, offset, 300);


    string guiMenu = "|@T| " + _("Rename");

    if (!internal) {
        guiMenu += "  |@S| " + _("Change MC") + " ";

        if (game.values["memcard"] == "SONY") {
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

                    if (e.jaxis.axis == 1) {
                        if (e.jaxis.value > PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selOption++;
                            if (selOption > 13) {
                                selOption = 13;
                            }
                            render();
                        }
                        if (e.jaxis.value < -PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            selOption--;
                            if (selOption < 5) {
                                selOption = 5;
                            }
                            render();
                        }
                    }
                    if (e.jaxis.axis == 0) {
                        if (e.jaxis.value > PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            processOptionChange(true);

                            render();
                        }
                        if (e.jaxis.value < -PCS_DEADZONE) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            processOptionChange(false);
                        }
                        render();
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (!internal) {
                        if (game.values["memcard"] == "SONY") {
                            if (e.jbutton.button == gui->_cb(PCS_BTN_START,&e)) {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                                keyboard->label = _("Enter new name for memory card");
                                keyboard->result = game.values["title"];
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
                                            game.entry +
                                            Util::separator() + "memcards";
                                    memcard->storeToRepo(savePath, result);
                                    game.values["memcard"] = result;
                                    game.save(game.path);
                                }
                                render();

                            };
                        }
                    } else
                        {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                        }


                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE,&e)) {
                        if (!internal) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            GuiSelectMemcard *selector = new GuiSelectMemcard(renderer);
                            selector->cardSelected = game.values["memcard"];
                            selector->show();

                            if (selector->selected != -1)
                                if (selector->selected == 0) {
                                    game.values["memcard"] = "SONY";
                                    game.save(game.path);
                                } else {
                                    game.values["memcard"] = selector->cards[selector->selected];
                                    game.save(game.path);
                                }
                            delete (selector);
                        } else
                        {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                        }
                        render();

                    };


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE,&e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        SDL_DestroyTexture(cover);
                        cover = nullptr;
                        menuVisible = false;

                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE,&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = _("Enter new game name");
                        keyboard->result = game.values["title"];
                        keyboard->show();
                        string result = keyboard->result;
                        bool cancelled = keyboard->cancelled;
                        delete (keyboard);

                        if (result.empty()) {
                            cancelled = true;
                        }


                        if (!cancelled) {
                            if (!internal) {
                                game.values["title"] = result;
                                game.values["automation"] = "0";
                                game.save(game.path);
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