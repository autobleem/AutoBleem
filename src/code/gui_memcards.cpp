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
#include "memcard.h"
#include "gui_confirm.h"
#include "gui_keyboard.h"

void GuiMemcards::init() {
    shared_ptr<Gui> gui(Gui::getInstance());
    Memcard *memcardOps = new Memcard(gui->path);
    cards = memcardOps->list();
    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible;
    delete memcardOps;

}

void GuiMemcards::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine("-=Custom Memory Cards=-", 0, offset, true);

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

    gui->renderStatus("Card " + to_string(selected + 1) + "/" + to_string(cards.size()) +
                              "   |@L1|/|@R1| Page   |@S| New Card   |@T| Delete Card   |@O| Go back|");
    SDL_RenderPresent(renderer);
}

void GuiMemcards::loop() {
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
                            if (selected >= cards.size()) {
                                selected = 0;
                                firstVisible = selected;
                                lastVisible = firstVisible+maxVisible;
                            }
                            render();
                        }
                        if (e.jaxis.value < -3200) {
                            selected--;
                            if (selected < 0) {
                                selected = cards.size()-1;
                                firstVisible = selected;
                                lastVisible = firstVisible+maxVisible;
                            }
                            render();
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (e.jbutton.button == PCS_BTN_R1) {
                        selected+=maxVisible;
                        if (selected >= cards.size()) {
                            selected = cards.size() - 1;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible+maxVisible;
                        render();
                    };
                    if (e.jbutton.button == PCS_BTN_L1) {
                        selected-=maxVisible;
                        if (selected < 0) {
                            selected = 0;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible+maxVisible;
                        render();
                    };


                    if (e.jbutton.button == PCS_BTN_CIRCLE) {

                        menuVisible = false;

                    };
                    if (e.jbutton.button == PCS_BTN_TRIANGLE) {

                        if (cards.size() != 0) {


                            GuiConfirm *guiConfirm = new GuiConfirm(renderer);
                            guiConfirm->label = "Delete card '" + cards[selected] + "' ?";
                            guiConfirm->show();
                            bool result = guiConfirm->result;
                            delete (guiConfirm);

                            if (result) {
                                Memcard *memcardOps = new Memcard(gui->path);
                                memcardOps->deleteCard(cards[selected]);
                                cards = memcardOps->list();
                                delete memcardOps;
                            }
                            render();
                        }
                    };

                    if (e.jbutton.button == 500) {
                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = "Enter new name for card '"+cards[selected]+"'";
                        keyboard->result = cards[selected];
                        keyboard->show();
                        string result = keyboard->result;
                        bool cancelled = keyboard->cancelled;
                        delete (keyboard);

                        if (result.empty()) {
                            cancelled = true;
                        }

                        string testResult = result;
                        if (Util::matchesLowercase("sony", testResult)) {
                            cancelled = true;
                        }

                        for (string card:cards)
                        {
                            if (card==result)
                            {
                                // orevent overwrite other card
                                cancelled = true;
                            }
                        }

                        if (!cancelled) {
                            Memcard *memcardOps = new Memcard(gui->path);
                            memcardOps->rename(cards[selected],result);
                            delete memcardOps;
                            init();
                            int pos=0;
                            for (string card:cards)
                            {
                                if (card==result)
                                {
                                    selected=pos;
                                    firstVisible=pos;
                                    lastVisible=firstVisible+maxVisible;
                                }
                                pos++;
                            }
                        }
                        render();
                    }

                    if (e.jbutton.button == PCS_BTN_SQUARE) {

                        GuiKeyboard *keyboard = new GuiKeyboard(renderer);
                        keyboard->label = "Enter new card name";
                        keyboard->show();
                        string result = keyboard->result;
                        bool cancelled = keyboard->cancelled;
                        delete (keyboard);

                        if (result.empty()) {
                            cancelled = true;
                        }

                        string testResult = result;
                        if (Util::matchesLowercase("sony", testResult)) {
                            cancelled = true;
                        }

                        if (!cancelled) {
                            Memcard *memcardOps = new Memcard(gui->path);
                            memcardOps->newCard(result);
                            cards = memcardOps->list();
                            int i = 0;
                            for (string card:cards) {
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