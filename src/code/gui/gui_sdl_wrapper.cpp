#include "gui_sdl_wrapper.h"

void SDL_DelRes(SDL_Window   *r) { SDL_DestroyWindow(r);   }
void SDL_DelRes(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
void SDL_DelRes(SDL_Texture  *r) { SDL_DestroyTexture(r);  }
void SDL_DelRes(SDL_Surface  *r) { SDL_FreeSurface(r);     }
