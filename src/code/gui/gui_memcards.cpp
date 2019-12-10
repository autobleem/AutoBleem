//
// Created by screemer on 2019-01-24.
//

#include "gui_memcards.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "../engine/memcard.h"
#include "gui_confirm.h"
#include "gui_keyboard.h"
using namespace std;

//*******************************
// GuiMemcards::init
//*******************************
void GuiMemcards::init() {
    shared_ptr<Gui> gui(Gui::getInstance());
    Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
    cards = memcardOps->list();
    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible;
    delete memcardOps;
}

//*******************************
// GuiMemcards::render
//*******************************
void GuiMemcards::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=" + _("Custom Memory Cards") + "=-", 0, offset, POS_CENTER);

    if (selected >= cards.size()) {
        selected = cards.size() - 1;
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
        if (i >= cards.size()) {
            break;
        }
        gui->renderTextLine(cards[i], pos, offset);
        pos++;
    }

    if (!cards.size() == 0) {
        gui->renderSelectionBox(selected - firstVisible + 1, offset);
    }

    gui->renderStatus(_("Card") + " " + to_string(selected + 1) + "/" + to_string(cards.size()) +
                      "   |@L1|/|@R1| " + _("Page") + "   |@X| " + _("Rename") + "  |@S| " + _("New Card") +
                      "   |@T| " + _("Delete") + "  |@O| " + _("Go back") + "|");
    SDL_RenderPresent(renderer);
}

//*******************************
// GuiMemcards::loop
//*******************************
void GuiMemcards::loop() {
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
                        if (selected >= cards.size()) {
                            selected = 0;
                            firstVisible = selected;
                            lastVisible = firstVisible + maxVisible;
                        }
                        render();
                    }
                    if (gui->mapper.isUp(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        selected--;
                        if (selected < 0) {
                            selected = cards.size() - 1;
                            firstVisible = selected;
                            lastVisible = firstVisible + maxVisible;
                        }
                        render();
                    }

                    break;
                case SDL_JOYBUTTONDOWN:

                    if (e.jbutton.button == gui->_cb(PCS_BTN_R1, &e)) {
                        Mix_PlayChannel(-1, gui->home_up, 0);
                        selected += maxVisible;
                        if (selected >= cards.size()) {
                            selected = cards.size() - 1;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible + maxVisible;
                        render();
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_L1, &e)) {
                        Mix_PlayChannel(-1, gui->home_down, 0);
                        selected -= maxVisible;
                        if (selected < 0) {
                            selected = 0;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible + maxVisible;
                        render();
                    };


                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (cards.size() != 0) {
                            GuiConfirm *guiConfirm = new GuiConfirm(renderer);
                            guiConfirm->label = _("Delete card") + " '" + cards[selected] + "' ?";
                            guiConfirm->show();
                            bool result = guiConfirm->result;
                            delete (guiConfirm);

                            if (result) {
                                Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
                                memcardOps->deleteCard(cards[selected]);
                                cards = memcardOps->list();
                                delete memcardOps;
                            }
                            render();
                        }
                    };

                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {

                        Mix_PlayChannel(-1, gui->cursor, 0);
                        if (cards.empty()) {
                            continue;
                        }
                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = _("Enter new name for card") + " '" + cards[selected] + "'";
                        keyboard->result = cards[selected];
                        keyboard->show();
                        string result = keyboard->result;
                        bool cancelled = keyboard->cancelled;
                        delete (keyboard);

                        if (result.empty()) {
                            cancelled = true;
                        }

                        string testResult = result;
                        if (Util::compareCaseInsensitive("sony", testResult)) {
                            cancelled = true;
                        }

                        for (const string & card:cards) {
                            if (card == result) {
                                // orevent overwrite other card
                                cancelled = true;
                            }
                        }

                        if (!cancelled) {
                            Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
                            memcardOps->rename(cards[selected], result);
                            delete memcardOps;
                            init();
                            int pos = 0;
                            for (const string & card:cards) {
                                if (card == result) {
                                    selected = pos;
                                    firstVisible = pos;
                                    lastVisible = firstVisible + maxVisible;
                                }
                                pos++;
                            }
                        }
                        render();
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {

                        Mix_PlayChannel(-1, gui->cursor, 0);
                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = _("Enter new card name");
                        keyboard->show();
                        string result = keyboard->result;
                        bool cancelled = keyboard->cancelled;
                        delete (keyboard);

                        if (result.empty()) {
                            cancelled = true;
                        }

                        string testResult = result;
                        if (Util::compareCaseInsensitive("sony", testResult)) {
                            cancelled = true;
                        }

                        if (!cancelled) {
                            Memcard *memcardOps = new Memcard(gui->pathToGamesDir);
                            memcardOps->newCard(result);
                            cards = memcardOps->list();
                            int i = 0;
                            for (const string & card:cards) {
                                if (card == result) {
                                    selected = i;
                                    firstVisible = i;
                                    lastVisible = firstVisible + maxVisible;

                                    if (cards.size() > maxVisible) {
                                        if (lastVisible >= cards.size()) {
                                            lastVisible = cards.size() - 1;
                                            firstVisible = lastVisible - maxVisible;
                                        }
                                    }
                                }
                                i++;
                            }
                            delete memcardOps;
                        }
                        render();
                    };
            }
        }
    }
}