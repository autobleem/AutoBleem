//
// Created by screemer on 2019-02-22.
//

#include "ps_stateselector.h"
#include "../gui/gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gui_launcher.h"

using namespace std;

//*******************************
// PsStateSelector::cleanSaveStateImages
//*******************************
void PsStateSelector::cleanSaveStateImages()
{
    for (int i=0;i<4;i++)
        slotImg[i] = nullptr;
}

//*******************************
// PsStateSelector::loadSaveStateImages
//*******************************
void PsStateSelector::loadSaveStateImages(PsGamePtr & game, bool saving)
{
    if (saving)
    {
        operation = OP_SAVE;
    } else
    {
        operation = OP_LOAD;
    }
    for (int i=0;i<4;i++)
    {
        slotImg[i] = nullptr;
        slotActive[i]=false;
        if (!saving) {
            if (game->isResumeSlotActive(i)) {
                slotImg[i] = Gfx::loadImage( game->findResumePicture(i).c_str());
                slotActive[i]=true;
            }
        } else
        {
            if (game->isResumeSlotActive(i)) {
                slotImg[i] = Gfx::loadImage( game->findResumePicture(i).c_str());

            }
            slotActive[i]=true;
        }
    }
}

//*******************************
// PsStateSelector::render
//*******************************
void PsStateSelector::render()
{
    if (visible)
    {
        float scale = 2.7f;
        x=10;
        y=220;
        SDL_SetRenderDrawColor(renderer,0,0,0,200);
        SDL_Rect rect;
        rect.x=0;
        rect.y=100;
        rect.w=1280;
        rect.h=720-200;
        SDL_RenderFillRect(renderer,&rect);

        int w = 118 * scale;
        int h = 118 * scale;
        SDL_Rect input, output;
        input.x = 0, input.y = 0;
        input.h = 118, input.w = 118;
        output.x = x ;
        output.y = y ;
        output.w = w;
        output.h = h;

        string text = _("SELECT RESUME SLOT TO LOAD");

        if (operation==OP_SAVE)
        {
            text = _("SELECT SLOT TO SAVE STATE");
        }

        GuiLauncher::renderText(0, 110, _(text), brightWhite, font30, POS_CENTER, false);   // center=true, background=false

        shared_ptr<Gui> gui(Gui::getInstance());
        SDL_Shared<SDL_Texture> infoText;
        SDL_Rect infoRect, infoDest;

        if (operation==OP_LOAD) {
            gui->getEmojiTextTexture(renderer,
                                     "|@T| " + _("Delete") + "     |@X| " + _("Select") + "     |@O| " + _("Cancel") +
                                     "|", font24, &infoText, &infoRect);
        } else
        {
            gui->getEmojiTextTexture(renderer,
                                     "|@X| " + _("Select") + "     |@O| " + _("Cancel") +
                                     "|", font24, &infoText, &infoRect);
        }
        infoDest.x=640-infoRect.w/2;
        infoDest.y=150;
        infoDest.w=infoRect.w;
        infoDest.h=infoRect.h;

        SDL_RenderCopy(renderer,infoText,&infoRect, &infoDest);

        for (int i=0;i<4;i++)
        {
            output.x = x+(118*scale)*i;

            if (selSlot==i)
            {
                SDL_SetTextureColorMod(frame, 255, 128, 128);
            } else
            {
                SDL_SetTextureColorMod(frame, 255, 255, 255);
            }
            SDL_RenderCopy(renderer, frame, &input, &output);
            SDL_SetTextureColorMod(frame, 255, 255, 255);


            if (slotImg[i]!= nullptr)
            {
                Uint32 format;
                int access;
                int wt,ht;

                SDL_QueryTexture(slotImg[i], &format, &access, &wt, &ht);
                input.x=0; input.y=0; input.w=wt; input.h=ht;
                SDL_Rect imgOut;
                imgOut.x = x+(118*scale)*i + 67;
                imgOut.y = y+90;
                imgOut.w=184;
                imgOut.h=140;

                SDL_RenderCopy(renderer, slotImg[i], &input, &imgOut);
            }

            GuiLauncher::renderText(output.x+60, 270, _("Slot")+" "+to_string(i+1), brightWhite, font24, POS_LEFT, false); // center=false, background=false
        }
    }
}