//
// Created by screemer on 2019-01-25.
//

#include "gui_editor.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "gui_keyboard.h"
#include "gui_selectmemcard.h"
#include "../engine/memcard.h"
#include "../engine/cfgprocessor.h"
#include "../lang.h"


void GuiEditor::init() {
    shared_ptr<Gui> gui(Gui::getInstance());


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
}

void GuiEditor::render() {
    shared_ptr<Gui> gui(Gui::getInstance());


    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + game.values["title"] + "=-", 0, offset, true);
    gui->renderTextLine(_("Folder:")+" " + game.entry + "", 1, offset, true);
    gui->renderTextLine(_("Published by:")+" " + game.values["publisher"], 2, offset, true);
    gui->renderTextLine(_("Year:") + game.values["year"] + "   "+_("Players:") +
                        game.values["players"], 3, offset, true);


    gui->renderTextLine(_("Memory Card:")+" " +
                        (game.values["memcard"] == "SONY" ? string(_("Internal")) : game.values["memcard"] + _("(Custom)")),
                        4, offset, true);

    gui->renderTextLine(_("Lock data:") + (game.values["automation"] == "0" ? string("|@Check|") : string("|@Uncheck|"))
                        + "  "+_("High res:") + (game.values["highres"] == "1" ? string("|@Check|") : string("|@Uncheck|")),
                        5, offset,
                        true);


    string guiMenu = "|@Select| "+_("Lock")+"  |@Start| "+_("Hi/Lo Res")+"   |@X| "+_("Rename")+"  |@S| "+_("Change MC")+" ";

    if (game.values["memcard"] == "SONY") {
        guiMenu += "|@T| "+_("Share MC")+"  ";
    }


    guiMenu += " |@O| "+_("Go back")+"|";

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
                case SDL_JOYBUTTONDOWN:

                    if (game.values["memcard"] == "SONY") {
                        if (e.jbutton.button == PCS_BTN_TRIANGLE) {
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
                                        gui->path + Util::separator() + "!SaveStates" + Util::separator() + game.entry +
                                        Util::separator() + "memcards";
                                memcard->storeToRepo(savePath, result);
                                game.values["memcard"] = result;
                                game.save(game.path);
                            }
                            render();

                        };
                    }


                    if (e.jbutton.button == PCS_BTN_SELECT) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (game.values["automation"] == "1") {
                            game.values["automation"] = "0";
                        } else {
                            game.values["automation"] = "1";
                        }
                        game.save(game.path);
                        render();

                    };


                    if (e.jbutton.button == PCS_BTN_START) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (game.values["highres"] == "1") {
                            game.values["highres"] = "0";
                        } else {
                            game.values["highres"] = "1";
                        }

                        game.save(game.path);
                        CfgProcessor *processor = new CfgProcessor();

                        processor->replace(game.entry, gui->path, "gpu_neon.enhancement_enable",
                                           "gpu_neon.enhancement_enable = " + game.values["highres"]);
                        delete processor;
                        render();
                    };



                    if (e.jbutton.button == PCS_BTN_SQUARE) {
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
                        render();

                    };


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        SDL_DestroyTexture(cover);
                        cover = nullptr;
                        menuVisible = false;

                    };

                    if (e.jbutton.button == PCS_BTN_CROSS) {
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
                            game.values["title"] = result;
                            game.values["automation"] = "0";
                            game.save(game.path);
                            changes = true;
                        }

                        render();


                    };


            }

        }
    }
}