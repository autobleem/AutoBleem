//
// Created by screemer on 2019-02-22.
//

#include "ps_stateselector.h"

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

        SDL_RenderCopy(renderer, frame, &input, &output);

        output.x = x+(118*scale);
        SDL_RenderCopy(renderer, frame, &input, &output);
        output.x = x+(118*scale)*2;
        SDL_RenderCopy(renderer, frame, &input, &output);
        output.x = x+(118*scale)*3;
        SDL_RenderCopy(renderer, frame, &input, &output);
    }
}