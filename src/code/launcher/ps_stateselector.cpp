//
// Created by screemer on 2019-02-22.
//

#include "ps_stateselector.h"
#include "../lang.h"
#include "../gui/gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void PsStateSelector::renderText(int x, int y, string text, Uint8 r, Uint8 g, Uint8 b, TTF_Font *font, bool center) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Color textColor = {r, g, b, 0};

    if (text.size() == 0) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect.x = 0;
        rect.y = 0;
        rect.h = 0;
        rect.w = 0;
    } else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        text_width = surface->w;
        text_height = surface->h;
        SDL_FreeSurface(surface);
        rect.x = x;
        rect.y = y;
        rect.w = text_width;
        rect.h = text_height;
    }
    SDL_Rect inputRect;
    inputRect.x = 0;
    inputRect.y = 0;
    inputRect.w = rect.w;


    inputRect.h = rect.h;

    if (center) rect.x=640-text_width/2;

    SDL_RenderCopy(renderer, texture, &inputRect, &rect);
    SDL_DestroyTexture(texture);


}

void PsStateSelector::cleanSaveStateImages()
{
    for (int i=0;i<4;i++)
    {
        if (slotImg[i]!= nullptr)
        {
            SDL_DestroyTexture(slotImg[i]);
        }
    }
}

void PsStateSelector::loadSaveStateImages(PsGame *game, bool saving)
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
                slotImg[i] = IMG_LoadTexture(renderer, game->findResumePicture(i).c_str());
                slotActive[i]=true;
            }
        } else
        {
            if (game->isResumeSlotActive(i)) {
                slotImg[i] = IMG_LoadTexture(renderer, game->findResumePicture(i).c_str());

            }
            slotActive[i]=true;
        }
    }
}

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


        renderText(0,110,_(text),255,255,255,font30,true);

        shared_ptr<Gui> gui(Gui::getInstance());
        SDL_Texture *infoText;
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
        SDL_DestroyTexture(infoText);

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

            renderText(output.x+60, 270, _("Slot")+" "+to_string(i+1), 255, 255, 255, font24,false);



        }



    }
}