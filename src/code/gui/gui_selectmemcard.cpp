//
// Created by screemer on 2019-01-25.
//

#include "gui_selectmemcard.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "gui.h"
#include "../engine/memcard.h"
#include "gui_confirm.h"
#include "gui_keyboard.h"
#include "../lang.h"

void GuiSelectMemcard::init() {
    cards.clear();


    shared_ptr<Gui> gui(Gui::getInstance());
    Memcard *memcardOps = new Memcard(gui->path);
    cards = memcardOps->list();

    maxVisible = atoi(gui->themeData.values["lines"].c_str());
    firstVisible = 0;
    lastVisible = firstVisible + maxVisible;


    if (!cardSelected.empty()) {
        for (int i = 0; i < cards.size(); i++) {
            if (cards[i] == cardSelected) {
                selected = i + 1;
            }
        }
    }

    vector<string>::iterator it;
    it = cards.begin();
    cards.insert(it, string(_("(Internal)")));

    delete memcardOps;

}

void GuiSelectMemcard::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    int offset = gui->renderLogo(true);
    gui->renderTextLine(_("-=Select memory card=-"), 0, offset, true);

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
                      "   |@L1|/|@R1| " + _("Page") + "     |@X| " + _("Select") + "  |@O| " + ("Cancel") + "|");
    SDL_RenderPresent(renderer);
}

void GuiSelectMemcard::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());
    bool menuVisible = true;
    while (menuVisible) {
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
                    if (e.jbutton.button ==  gui->_cb(PCS_BTN_R1,&e)) {

                        Mix_PlayChannel(-1, gui->home_up, 0);
                        selected += maxVisible;
                        if (selected >= cards.size()) {
                            selected = cards.size() - 1;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible + maxVisible;
                        render();
                    };
                    if (e.jbutton.button ==  gui->_cb(PCS_BTN_L1,&e)) {

                        Mix_PlayChannel(-1, gui->home_down, 0);
                        selected -= maxVisible;
                        if (selected < 0) {
                            selected = 0;
                        }
                        firstVisible = selected;
                        lastVisible = firstVisible + maxVisible;
                        render();
                    };

                    if (e.jbutton.button ==  gui->_cb(PCS_BTN_CIRCLE,&e)) {

                        Mix_PlayChannel(-1, gui->cancel, 0);
                        selected = -1;
                        menuVisible = false;

                    };
                    if (e.jbutton.button ==  gui->_cb(PCS_BTN_CROSS,&e)) {

                        Mix_PlayChannel(-1, gui->cursor, 0);
                        menuVisible = false;
                    };


            }

        }
    }
}