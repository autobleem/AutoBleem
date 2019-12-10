//
// Created by screemer on 2019-07-30.
//

#include "gui_mc_manager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include "../gui/gui.h"
#include "../gui/gui_confirm.h"
#include "../gui/gui_selectmemcard.h"


void GuiMcManager::init() {
    rightCardName_ori = rightCardName;
    cardPath_ori = card2path;
    loadAssets();
}

void GuiMcManager::loadAssets() {
    shared_ptr<Gui> gui(Gui::getInstance());
    mcGrid = IMG_LoadTexture(renderer, (gui->getCurrentThemeImagePath() + sep + "MC/Dot_Matrix.png").c_str());
    mcPencil = IMG_LoadTexture(renderer, (gui->getCurrentThemeImagePath() + sep + "MC/Pencil_Carsor.png").c_str());
    fontJIS = Fonts::openFont(Env::getWorkingPath() + sep + "japanese.ttf", 20);

    memcard1 = new CardEdit(renderer);
    memcard2 = new CardEdit(renderer);

    memcard1->load_file(card1path);
    memcard2->load_file(card2path);

    pencilPos.w = 42;
    pencilPos.h = 42;
    pencilPos.x = mc1XStart;
    pencilPos.y = 150;
    pencilColumn = 0;
    pencilRow = 0;
    pencilMemcard = 1;

}

void GuiMcManager::pencilDown() {
    if (pencilRow != 4) {
        pencilRow++;
    }
}

void GuiMcManager::pencilUp() {
    if (pencilRow != 0) {
        pencilRow--;
    }
}

void GuiMcManager::pencilLeft() {
    if (pencilColumn != 0) {
        pencilColumn--;
    } else {
        pencilColumn = 2;
        if (pencilMemcard == 1) pencilMemcard = 2; else pencilMemcard = 1;
    }
}

void GuiMcManager::pencilRight() {
    if (pencilColumn != 2) {
        pencilColumn++;
    } else {
        pencilColumn = 0;
        if (pencilMemcard == 1) pencilMemcard = 2; else pencilMemcard = 1;
    }


}

void GuiMcManager::renderPencil(int memcard, int col, int row) {
    const int pencilShiftX = 80;
    const int pencilShiftY = 80;
    if (memcard == 1) {
        pencilPos.x = mc1XStart + (col * pencilShiftX);
    }
    if (memcard == 2) {
        pencilPos.x = mc2XStart + (col * pencilShiftX);
    }
    pencilPos.y = mcYStart + (row * pencilShiftY);
    SDL_RenderCopy(renderer, mcPencil, nullptr, &pencilPos);
}

void GuiMcManager::trySave()
{
    if (changes)
    {
        auto confirm = new GuiConfirm(renderer);
        confirm->label = _("Do you want to save memcards data ?");
        confirm->show();
        if (confirm->result) {
            memcard1->save_file(card1path);
            memcard2->save_file(card2path);
            changes = false;
        }
        delete (confirm);
        changes = false;
    }
}

void GuiMcManager::renderStatic() {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->renderBackground();
    gui->renderTextBar();
    gui->renderTextLine(_("Memory Card Manager"), 1, 1, POS_CENTER);
    gui->renderStatus(
            "|@Start| " + _("Select Right Card") +
            " | |@Select| " + _("Defragment Card") +
            "   | " + "|@X| " + _("Reload Cards") +
            "   | " + "|@T| " + _("Delete") +
            " | " + "|@S| " + _("Copy") +
            " | " + "|@O| " + _("Go back") +
            "|");

    //Draw dot matrix image
    SDL_Rect input, output;
    SDL_QueryTexture(mcGrid, NULL, NULL, &input.w, &input.h);
    SDL_QueryTexture(mcGrid, NULL, NULL, &output.w, &output.h);
    input.x = 0, input.y = 0;
    output.x = 80;
    output.y = 80;
    SDL_RenderCopy(renderer, mcGrid, &input, &output);
    output.x = 940;
    output.y = 80;
    SDL_RenderCopy(renderer, mcGrid, &input, &output);

}

void GuiMcManager::renderMemCardIcons(int memcard) {
    const int xStartMC1 = 80, xStartMC2 = 940, yStart = 80, xDecal = 10, yDecal = 10, xShift = 80, yShift = 80;
    SDL_Rect output;
    output.h = 75;
    output.w = 75;

    int start;
    CardEdit *currentCard;
    if (memcard == 1) {
        start = xStartMC1;
        currentCard = memcard1;
    }

    if (memcard == 2) {
        start = xStartMC2;
        currentCard = memcard2;
    }


    for (int i = 0; i < 15; i++) {
        int col = i % 3;
        int line = i / 3;
        int frame = 0;
        if ((pencilMemcard == memcard) && (pencilRow == line) && (pencilColumn == col)) {
            frame = animFrame;
        }
        output.x = start + (xShift * col) + xDecal;
        output.y = yStart + (yShift * line) + yDecal;
        if (currentCard->get_slot_is_used(i)) {
            SDL_RenderCopy(renderer, currentCard->get_slot_icon(i, frame), nullptr, &output);
        }
    }
}

void GuiMcManager::renderMetaInfo() {
    shared_ptr<Gui> gui(Gui::getInstance());

    CardEdit *card;
    if (pencilMemcard == 1) {
        card = memcard1;

    } else {
        card = memcard2;
    }

    string title = card->get_slot_title(pencilColumn + pencilRow * 3);
    string gameID = card->get_slot_gameID(pencilColumn + pencilRow * 3);
    string pCode = card->get_slot_Pcode(pencilColumn + pencilRow * 3);

    string nextSlot = to_string(card->next_slot_map[pencilColumn + pencilRow * 3]);

    gui->renderTextLine(title, -500, 1, POS_CENTER, true, fontJIS);
    gui->renderTextLine(gameID, 3, 1, POS_CENTER, true);
    gui->renderTextLine(pCode, 4, 1, POS_CENTER, true);



    gui->renderTextLine(leftCardName, -500, 1, POS_LEFT, true);
    gui->renderTextLine(rightCardName, -500, 1, POS_RIGHT, true);
}

void GuiMcManager::render() {
    shared_ptr<Gui> gui(Gui::getInstance());
    // render static elements
    renderStatic();
    //Draw Memcard images and meta info
    renderMemCardIcons(1);
    renderMemCardIcons(2);
    renderMetaInfo();

    //Draw the pencil
    renderPencil(pencilMemcard, pencilColumn, pencilRow);
    SDL_RenderPresent(renderer);

}

void GuiMcManager::loop() {
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
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        trySave();
                        menuVisible = false;
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CROSS, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        trySave();
                        memcard1->load_file(card1path);
                        memcard2->load_file(card2path);
                        changes = false;
                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_SELECT, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        CardEdit *newCard = new CardEdit(renderer);
                        CardEdit *src;
                        if (pencilMemcard == 1) {
                            src = memcard1;

                        } else {
                            src = memcard2;

                        }
                        int last = 0;
                        for (int slot = 0; slot < 15; slot++) {
                            if (!src->is_slot_top(slot))
                            {
                                continue;
                            }
                            int gameSize = src->getGameSlots(slot).size();
                            vector<int> destSlots = newCard->findEmptySlot(gameSize);

                            if (destSlots.size()>=0)
                            {
                                Mix_PlayChannel(-1, gui->cursor, 0);
                                unsigned char * buffer;
                                int exportSize = src->getExportSize(slot);
                                buffer = new unsigned char[exportSize];
                                src->exportGame(slot,buffer);
                                newCard->importGame(buffer,exportSize);
                                delete buffer;
                                changes = true;

                            }
                        }
                        if (pencilMemcard == 1) {
                            memcard1 = newCard;
                            delete(src);
                        } else
                        {
                            memcard2 = newCard;
                            delete(src);
                        };
                    }

                    if (e.jbutton.button == gui->_cb(PCS_BTN_START, &e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        trySave();
                        auto select = new GuiSelectMemcard(renderer);
                        select->listType=MC_MANAGER;
                        select->show();
                        if (select->selected!=-1)
                        {
                            if (select->selected==0) {
                                rightCardName = rightCardName_ori;
                                card2path = cardPath_ori;
                                memcard2->load_file(card2path);
                            } else
                            {
                                // this is custom
                                int cardNumCustom=atoi(select->cardSelected.substr(1,1).c_str());
                                string memcard = select->cardSelected.substr(4);
                                string cardPath =  Env::getPathToMemCardsDir() + sep + memcard  + "/card" + to_string(cardNumCustom) + ".mcd";

                                rightCardName = select->cardSelected;
                                card2path = cardPath;
                                cout << "Card:" << cardPath << endl;
                                memcard2->load_file(card2path);
                            }
                            changes = false;
                        }
                        delete select;
                    }
                    if (e.jbutton.button == gui->_cb(PCS_BTN_TRIANGLE, &e)) {
                        CardEdit *card;
                        if (pencilMemcard == 1) {
                            card = memcard1;
                        } else {
                            card = memcard2;
                        }
                        int slot = pencilColumn + pencilRow * 3;
                        if (!card->is_slot_top(slot)) {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                            continue;
                        }
                        if (card->get_slot_is_free(slot)) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            continue;
                        }
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        card->delete_game(slot);
                        changes=true;


                    };
                    if (e.jbutton.button == gui->_cb(PCS_BTN_SQUARE, &e)) {
                        CardEdit *src, *dest;
                        if (pencilMemcard == 1) {
                            src = memcard1;
                            dest = memcard2;
                        } else {
                            src = memcard2;
                            dest= memcard1;
                        }
                        int slot = pencilColumn + pencilRow * 3;
                        if (!src->is_slot_top(slot)) {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                            continue;
                        }
                        if (src->get_slot_is_free(slot)) {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            continue;
                        }

                        int gameSize = src->getGameSlots(slot).size();
                        vector<int> destSlots = dest->findEmptySlot(gameSize);

                        if (destSlots.size()>=0)
                        {
                            Mix_PlayChannel(-1, gui->cursor, 0);
                            unsigned char * buffer;
                            int exportSize = src->getExportSize(slot);
                            buffer = new unsigned char[exportSize];

                            src->exportGame(slot,buffer);
                            dest->importGame(buffer,exportSize);
                            delete buffer;
                            changes = true;

                        } else
                        {
                            Mix_PlayChannel(-1, gui->cancel, 0);
                        }


                    };
                case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                case SDL_JOYHATMOTION:
                    if (gui->mapper.isCenter(&e)) {

                    }
                    if (gui->mapper.isLeft(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        pencilLeft();
                    }
                    if (gui->mapper.isRight(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        pencilRight();
                    }
                    if (gui->mapper.isUp(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        pencilUp();
                    }
                    if (gui->mapper.isDown(&e)) {
                        Mix_PlayChannel(-1, gui->cursor, 0);
                        pencilDown();
                    }

            }
        }
        counter++;
        if (counter > 5) {
            animFrame++;
            if (animFrame > 2) animFrame = 0;
            counter = 0;
        }
        render();
    }
}

