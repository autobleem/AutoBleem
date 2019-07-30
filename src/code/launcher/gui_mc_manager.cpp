//
// Created by screemer on 2019-07-30.
//

#include "gui_mc_manager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include "../gui/gui.h"
#include "../lang.h"

void GuiMcManager::init() {
    loadAssets();
}

void GuiMcManager::loadAssets() {
    shared_ptr<Gui> gui(Gui::getInstance());
    mcGrid = IMG_LoadTexture(renderer, (gui->getSonyImagePath() + "/MC/Dot_Matrix.png").c_str());
    mcPencil = IMG_LoadTexture(renderer, (gui->getSonyImagePath() + "/MC/Pencil_Carsor.png").c_str());
    fontJIS = Fonts::openFont(gui->getSonyFontPath() + "/SSTJapanese-Regular.ttf", 20);

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

void GuiMcManager::pencilDown()
{
    if (pencilRow!=4)
    {
        pencilRow++;
    }
}
void GuiMcManager::pencilUp()
{
    if (pencilRow!=0)
    {
        pencilRow--;
    }
}
void GuiMcManager::pencilLeft()
{
    if (pencilColumn!=0)
    {
        pencilColumn--;
    } else
    {
        pencilColumn = 2;
        if (pencilMemcard==1) pencilMemcard=2; else pencilMemcard=1;
    }
}
void GuiMcManager::pencilRight()
{
    if (pencilColumn!=2)
    {
        pencilColumn++;
    } else
    {
        pencilColumn = 0;
        if (pencilMemcard==1) pencilMemcard=2; else pencilMemcard=1;
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

void GuiMcManager::renderStatic() {
    shared_ptr<Gui> gui(Gui::getInstance());
    // use evoUI background
    SDL_Rect backgroundRect;
    int bakcw, backh;
    SDL_RenderClear(renderer);
    SDL_SetTextureBlendMode(backgroundImg, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(backgroundImg, NULL, NULL, &bakcw, &backh);
    backgroundRect.x = 0;
    backgroundRect.y = 0;
    backgroundRect.w = bakcw;
    backgroundRect.h = backh;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundImg, nullptr, &backgroundRect);
    gui->renderTextBar();
    gui->renderTextLine(_("Memory Card Manager"), 1, 1, true);
    gui->renderStatus(
            "|@S| " + _("Format Memory Card") + " | " + "|@T| " + _("Delete Block") + " | " + "|@O| " + _("Go back") +
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
        if ((pencilMemcard==memcard) && (pencilRow==line) && (pencilColumn==col))
        {
            frame = animFrame;
        }
        output.x = start + (xShift * col) + xDecal;
        output.y = yStart + (yShift * line) + yDecal;
        if (currentCard->get_slot_is_used(i)) {
            SDL_RenderCopy(renderer, currentCard->get_slot_icon(i,frame), nullptr, &output);
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

    gui->renderTextLine(title, 3, 1, true, true, fontJIS);
    gui->renderTextLine(gameID, 3, 1, true, true);
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
                case SDL_JOYBUTTONUP:
                    if (e.jbutton.button == gui->_cb(PCS_BTN_CIRCLE, &e)) {
                        Mix_PlayChannel(-1, gui->cancel, 0);
                        menuVisible = false;
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
        if (counter>5) {
            animFrame++;
            if (animFrame > 2) animFrame = 0;
            counter = 0;
        }
        render();
    }
}

