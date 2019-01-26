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
#include "memcard.h"


void GuiEditor::init() {
    shared_ptr<Gui> gui(Gui::getInstance());


    if (this->game.values["memcard"] != "SONY") {
        string cardpath =
                gui->path + Util::separator() + "!MemCards" + Util::separator() + this->game.values["memcard"];
        if (!Util::exists(cardpath)) {
            this->game.values["memcard"] = "SONY";
        }
    }
}

void GuiEditor::render() {
    shared_ptr<Gui> gui(Gui::getInstance());


    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + game.values["title"] + "=-", 0, offset, true);
    gui->renderTextLine("Folder: " + game.entry + "", 1, offset, true);
    gui->renderTextLine("Published by: " + game.values["publisher"] + "   Year:" + game.values["year"] + "   Players:" +
                        game.values["players"], 2, offset, true);
    gui->renderTextLine("Memory Card: " +
                        (game.values["memcard"] == "SONY" ? string("Internal") : game.values["memcard"] + "(Custom)"),
                        3, offset, true);
    gui->renderTextLine("Block updates: " + (game.values["automation"] == "0" ? string("True") : string("False")), 4,
                        offset, true);
    gui->renderTextLine("", 5, offset, false);

    string guiMenu = "|@X| Rename  |@S| Change MC ";
    if (game.values["memcard"] == "SONY") {
        guiMenu += "|@T| Share MC  ";
    }


    if (game.values["automation"] == "1") {
        guiMenu += "|@Start| Block Data  ";
    }

    guiMenu += "|@O| Go back|";

    gui->renderStatus(guiMenu);
    SDL_RenderPresent(renderer);
}

void GuiEditor::loop() {
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
                case SDL_JOYBUTTONDOWN:

                    if (game.values["memcard"] == "SONY") {
                        if (e.jbutton.button == PCS_BTN_TRIANGLE) {
                            GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                            keyboard->label = "Enter new name for memory card";
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

                    if (game.values["automation"] == "1") {
                        if (e.jbutton.button == PCS_BTN_START) {
                            game.values["automation"] = "0";
                            game.save(game.path);
                            render();

                        };
                    }


                    if (e.jbutton.button == PCS_BTN_SQUARE) {
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

                        menuVisible = false;

                    };

                    if (e.jbutton.button == PCS_BTN_CROSS) {
                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = "Enter new game name";
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