//
// Created by screemer on 2019-01-24.
//

#include "gui_splash.h"
#include "gui.h"
#include "../engine/scanner.h"
using namespace std;

//*******************************
// GuiSplash::render
//*******************************
void GuiSplash::render() {
    std::shared_ptr<Gui> gui(Gui::getInstance());

    Gfx::setImageBlend(GfxTheme::backgroundImg);
    GfxSize size = Gfx::getImageSize(GfxTheme::backgroundImg);

    SDL_Rect backgroundRect;
    backgroundRect.x = 0;
    backgroundRect.y = 0;
    backgroundRect.w = size.w;
    backgroundRect.h = size.h;
    SDL_QueryTexture(GfxTheme::logo, NULL, NULL, &size.w, &size.h);

    GfxImage  textTex;
    SDL_Rect textRec;
    string splashText = _("AutoBleem")+" " + gui->cfg.inifile.values["version"];
    if (gui->cfg.inifile.values["quick"] == "true") {
        splashText += " ("+_("Quick boot")+" - |@O| "+_("Menu");
        splashText += ")";
    }

    GfxTheme::getEmojiTextTexture(splashText.c_str(), gui->themeFont,  &textTex, &textRec);

    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    textRec.y = atoi(GfxTheme::get("ttop").c_str());
    Gfx::clear();
    SDL_SetTextureAlphaMod(GfxTheme::backgroundImg, alpha);
    SDL_SetTextureAlphaMod(GfxTheme::logo, alpha);
    SDL_SetTextureAlphaMod(textTex, alpha);
    Mix_VolumeMusic(alpha / 3);

    SDL_Rect logoRect;
    logoRect.x = atoi(GfxTheme::get("lpositionx").c_str());
    logoRect.y = atoi(GfxTheme::get("lpositiony").c_str());
    logoRect.w = atoi(GfxTheme::get("lw").c_str());
    logoRect.h = atoi(GfxTheme::get("lh").c_str());

    Gfx::drawImage(GfxTheme::backgroundImg,0,0);
    SDL_RenderCopy(Application::renderer, GfxTheme::logo, NULL, &logoRect);

    string bg = GfxTheme::get("text_bg");

    int bg_alpha = atoi(GfxTheme::get("textalpha").c_str()) * alpha / 255;

    SDL_SetRenderDrawColor(Application::renderer, GfxTheme::getR(bg), GfxTheme::getG(bg), GfxTheme::getB(bg), bg_alpha);
    SDL_SetRenderDrawBlendMode(Application::renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(GfxTheme::get("textx").c_str());
    rect.y = atoi(GfxTheme::get("texty").c_str());
    rect.w = atoi(GfxTheme::get("textw").c_str());
    rect.h = atoi(GfxTheme::get("texth").c_str());
    SDL_RenderFillRect(Application::renderer, &rect);

    SDL_RenderCopy(Application::renderer, textTex, NULL, &textRec);
    Gfx::flip();
}

//*******************************
// GuiSplash::loop
//*******************************
void GuiSplash::loop() {
    shared_ptr<Gui> gui(Gui::getInstance());

    Mix_VolumeMusic(0);
    alpha = 0;
    start = SDL_GetTicks();
    while (1) {
        gui->watchJoystickPort();
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    gui->drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();
                }
            }
            if (e.type == SDL_QUIT)
                break;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                break;

            if (e.type == SDL_JOYBUTTONDOWN) {
                gui->overrideQuickBoot = true;
            }
        }
        render();
        int current = SDL_GetTicks();
        int time = current - start;
        if (time > 2) {
            if (alpha < 255) {
                alpha += 2;
                if (alpha > 255) {
                    alpha = 255;
                }
            } else {

                break;
            }
            start = SDL_GetTicks();
        }
    }
}