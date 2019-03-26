//
// Created by screemer on 2018-12-19.
//

#include "gui.h"
#include "gui_about.h"
#include "gui_splash.h"
#include "gui_options.h"
#include "gui_memcards.h"
#include "gui_manager.h"
#include "gui_confirm.h"
#include "../ver_migration.h"
#include "../lang.h"
#include "../launcher/gui_launcher.h"


void Gui::logText(string message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}


extern "C"
{
void logText(char *message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}
}

Uint8 Gui::getR(string val) {
    return atoi(Util::commaSep(val, 0).c_str());
}

Uint8 Gui::getG(string val) {
    return atoi(Util::commaSep(val, 1).c_str());
}

Uint8 Gui::getB(string val) {
    return atoi(Util::commaSep(val, 2).c_str());
}


void Gui::getTextAndRect(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font,
                         SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    string fg = themeData.values["text_fg"];
    SDL_Color textColor = {getR(fg), getG(fg), getB(fg), 0};

    if (strlen(text) == 0) {
        *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        rect->x = 0;
        rect->y = 0;
        rect->h = 0;
        rect->w = 0;
        return;
    }

    surface = TTF_RenderUTF8_Blended(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}


void Gui::renderBackground() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundImg, nullptr, &backgroundRect);
}

int Gui::renderLogo(bool small) {
    if (!small) {
        SDL_RenderCopy(renderer, logo, nullptr, &logoRect);
        return 0;
    } else {
        SDL_Rect rect;
        rect.x = atoi(themeData.values["opscreenx"].c_str());
        rect.y = atoi(themeData.values["opscreeny"].c_str());
        rect.w = logoRect.w / 3;
        rect.h = logoRect.h / 3;
        SDL_RenderCopy(renderer, logo, nullptr, &rect);
        return rect.y + rect.h;
    }
}

SDL_Texture *Gui::loadThemeTexture(SDL_Renderer *renderer, string themePath, string defaultPath, string texname) {
    SDL_Texture *tex;
    if (Util::exists(themePath + themeData.values[texname])) {
        tex = IMG_LoadTexture(renderer, (themePath + themeData.values[texname]).c_str());
    } else {
        tex = IMG_LoadTexture(renderer, (defaultPath + defaultData.values[texname]).c_str());
    }
    return tex;
}

void Gui::loadAssets() {
    string defaultPath = Util::getWorkingPath() + Util::separator() + "theme" + Util::separator() + "default" +
                         Util::separator();
    themePath = Util::getWorkingPath() + Util::separator() + "theme" + Util::separator() + cfg.inifile.values["theme"] +
                Util::separator();

    themeData.load(defaultPath + "theme.ini");
    defaultData.load(defaultPath + "theme.ini");
    themeData.load(themePath + "theme.ini");

    bool reloading = false;

    if (backgroundImg != nullptr) {
        SDL_DestroyTexture(backgroundImg);
        SDL_DestroyTexture(logo);
        SDL_DestroyTexture(buttonT);
        SDL_DestroyTexture(buttonO);
        SDL_DestroyTexture(buttonX);
        SDL_DestroyTexture(buttonS);
        SDL_DestroyTexture(buttonStart);
        SDL_DestroyTexture(buttonSelect);
        SDL_DestroyTexture(buttonL1);
        SDL_DestroyTexture(buttonR1);
        SDL_DestroyTexture(buttonL2);
        SDL_DestroyTexture(buttonR2);
        SDL_DestroyTexture(buttonCheck);
        SDL_DestroyTexture(buttonUncheck);
        SDL_DestroyTexture(cdJewel);
        TTF_CloseFont(font);
        Mix_FreeChunk(cursor);
        Mix_FreeChunk(cancel);
        Mix_FreeChunk(home_down);
        Mix_FreeChunk(home_up);
        reloading = true;
        backgroundImg = nullptr;
    }


    logoRect.x = atoi(themeData.values["lpositionx"].c_str());
    logoRect.y = atoi(themeData.values["lpositiony"].c_str());
    logoRect.w = atoi(themeData.values["lw"].c_str());
    logoRect.h = atoi(themeData.values["lh"].c_str());

    backgroundImg = loadThemeTexture(renderer, themePath, defaultPath, "background");
    logo = loadThemeTexture(renderer, themePath, defaultPath, "logo");

    buttonO = loadThemeTexture(renderer, themePath, defaultPath, "circle");
    buttonX = loadThemeTexture(renderer, themePath, defaultPath, "cross");
    buttonT = loadThemeTexture(renderer, themePath, defaultPath, "triangle");
    buttonS = loadThemeTexture(renderer, themePath, defaultPath, "square");
    buttonSelect = loadThemeTexture(renderer, themePath, defaultPath, "select");
    buttonStart = loadThemeTexture(renderer, themePath, defaultPath, "start");
    buttonL1 = loadThemeTexture(renderer, themePath, defaultPath, "l1");
    buttonR1 = loadThemeTexture(renderer, themePath, defaultPath, "r1");
    buttonL2 = loadThemeTexture(renderer, themePath, defaultPath, "l2");
    buttonR2 = loadThemeTexture(renderer, themePath, defaultPath, "r2");
    buttonCheck = loadThemeTexture(renderer, themePath, defaultPath, "check");
    buttonUncheck = loadThemeTexture(renderer, themePath, defaultPath, "uncheck");
    if (cfg.inifile.values["jewel"] != "none") {
        if (cfg.inifile.values["jewel"] == "default") {
            cdJewel = IMG_LoadTexture(renderer, (Util::getWorkingPath() + "/evoimg/nofilter.png").c_str());
        } else {
            cdJewel = IMG_LoadTexture(renderer,
                                      (Util::getWorkingPath() + "/evoimg/frames/" +
                                       cfg.inifile.values["jewel"]).c_str());
        }
    } else
    {
        cdJewel = nullptr;
    }
    string fontPath = (themePath + themeData.values["font"]);
    font = TTF_OpenFont(fontPath.c_str(), atoi(themeData.values["fsize"].c_str()));


    if (music != nullptr) {

        Mix_FreeMusic(music);
        music = nullptr;
    }

    if (Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        printf("Unable to open audio: %s\n", Mix_GetError());
    }

    cursor = Mix_LoadWAV((this->getSonySoundPath() + "/cursor.wav").c_str());
    cancel = Mix_LoadWAV((this->getSonySoundPath() + "/cancel.wav").c_str());
    home_up = Mix_LoadWAV((this->getSonySoundPath() + "/home_up.wav").c_str());
    home_down = Mix_LoadWAV((this->getSonySoundPath() + "/home_down.wav").c_str());
    resume = Mix_LoadWAV((this->getSonySoundPath() + "/resume_new.wav").c_str());

    if (cfg.inifile.values["nomusic"] != "true")
        if (themeData.values["loop"] != "-1") {


            music = Mix_LoadMUS((themePath + themeData.values["music"]).c_str());
            if (music == nullptr) { printf("Unable to load Wav file: %s\n", Mix_GetError()); }
            if (Mix_PlayMusic(music, themeData.values["loop"] == "1" ? -1 : 0) == -1) {
                printf("Unable to play music file: %s\n", Mix_GetError());
            }

        }

}

void Gui::waitForGamepad() {
    int joysticksFound = SDL_NumJoysticks();
    while (joysticksFound == 0) {
        drawText(_("PLEASE CONNECT GAMEPAD TO PLAYSTATION CLASSIC"));
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        joysticksFound = SDL_NumJoysticks();
    }
}

void Gui::criticalException(string text) {
    drawText(text);
    while (true) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();

                }
            }
            if (e.type == SDL_QUIT)
                return;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                return;

            if (e.type == SDL_JOYBUTTONDOWN) {
                return;
            }
        }
    }
}


void Gui::display(bool forceScan, string path, Database *db, bool resume) {
    joysticks.clear();
    this->db = db;
    this->path = path;
    this->forceScan = forceScan;
    if (forceScan) overrideQuickBoot = true;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    printf("We compiled against SDL version %d.%d.%d ...\n",
           compiled.major, compiled.minor, compiled.patch);
    printf("But we are linking against SDL version %d.%d.%d.\n",
           linked.major, linked.minor, linked.patch);

    Mix_Init(0);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    SDL_Window *window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                          0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    loadAssets();


    if (!resume) {
        auto *splashScreen = new GuiSplash(renderer);
        splashScreen->show();
        delete splashScreen;

        drawText(_("Importing internal games"));
        Util::execUnixCommad("/media/Autobleem/rc/backup_internal.sh");

        /*
        drawText(_("Upgrading AutoBleem - Please wait..."));
        auto *migration = new VerMigration();
        migration->migrate(db);
        delete (migration);
        */

        if (cfg.inifile.values["quick"] != "true")
            waitForGamepad();
    } else {
        resumingGui = true;
        overrideQuickBoot = true;
    }


}

void Gui::saveSelection() {
    ofstream os;
    string path = cfg.inifile.values["cfg"];
    os.open(path);
    os << "#!/bin/sh" << endl << endl;
    os << "AB_SELECTION=" << menuOption << endl;
    os << "AB_THEME=" << cfg.inifile.values["stheme"] << endl;
    os << "AB_PCSX=" << cfg.inifile.values["pcsx"] << endl;
    os << "AB_MIP=" << cfg.inifile.values["mip"] << endl;

    os.flush();
    os.close();
}


bool otherMenuShift = false;
bool powerOffShift = false;


bool Gui::quickBoot() {

    int currentTime = SDL_GetTicks();
    string splashText = _("AutoBleem") + " " + cfg.inifile.values["version"];
    if (cfg.inifile.values["quick"] == "true") {
        splashText += " (" + _("Quick boot") + " - " + _("Hold") + " |@O| " + _("Menu") + ")";
    }

    while (true) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();

                }
            }
            if (e.type == SDL_QUIT)
                return false;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                return false;


            if (e.type == SDL_JOYBUTTONDOWN) {
                overrideQuickBoot = true;
                return false;
            }
        }
        drawText(splashText);

        int delay = 1000;

        if (cfg.inifile.values["delay"] != "") {
            delay = delay * atoi(cfg.inifile.values["delay"].c_str());
        }
        int newTime = SDL_GetTicks();
        int secs = delay / 1000 - (newTime - currentTime) / 1000;
        if (newTime - currentTime > delay) {
            return true;
        }
    }
}


void Gui::menuSelection() {
    shared_ptr<Scanner> scanner(Scanner::getInstance());

    SDL_Joystick *joystick;
    if (joysticks.empty())
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            joystick = SDL_JoystickOpen(i);
            joysticks.push_back(joystick);
            cout << "--" << SDL_JoystickName(joystick) << endl;
        }
    // Check if all OK
    if (scanner->noGamesFound) {
        criticalException(_("WARNING: NO GAMES FOUND. PRESS ANY BUTTON."));
    }
    //
    if (!coverdb->isValid()) {
        criticalException(_("WARNING: NO COVER DB FOUND. PRESS ANY BUTTON."));
    }
    if (!overrideQuickBoot) {
        bool quickBootCfg = (cfg.inifile.values["quick"] == "true");
        if (quickBootCfg && !forceScan) {
            if (quickBoot()) {

                if (cfg.inifile.values["quickmenu"] == "UI") {
                    if (cfg.inifile.values["ui"] == "classic") {
                        this->menuOption = MENU_OPTION_RUN;
                        return;
                    } else {
                        auto launcherScreen = new GuiLauncher(renderer);
                        launcherScreen->show();
                        delete launcherScreen;
                    }
                } else
                {
                    if (Util::exists("/media/RetroArch/retroarch")) {
                        this->menuOption = MENU_OPTION_RETRO;
                        return;
                    } else
                    {
                        auto launcherScreen = new GuiLauncher(renderer);
                        launcherScreen->show();
                        delete launcherScreen;
                    }
                }
            };

        }
    }
    otherMenuShift = false;
    powerOffShift = false;
    string retroarch = cfg.inifile.values["retroarch"];
    string adv = cfg.inifile.values["adv"];
    string mainMenu = "|@Start| " + _("AutoBleem") + "    |@X|  " + _("Re/Scan") + " ";
    if (cfg.inifile.values["ui"] == "classic") {
        mainMenu += "  |@O|  " + _("Original") + "  ";
    }
    if (retroarch == "true") {
        mainMenu += "|@S|  " + _("RetroArch") + "   ";
    }
    mainMenu += "|@T|  " + _("About") + "  |@Select|  " + _("Options") + " ";
    if (adv == "true") {
        mainMenu += "|@L1| " + _("Advanced");
    }
    mainMenu += " |@L2|+|@R2|" + _("Power Off");

    string forceScanMenu = _("Games changed. Press") + "  |@X|  " + _("to scan") + "|";
    string otherMenu = "|@X|  " + _("Memory Cards") + "   |@O|  " + _("Game Manager");
    cout << SDL_NumJoysticks() << "joysticks were found." << endl;


    if (!forceScan) {
        drawText(mainMenu);

    } else {
        drawText(forceScanMenu);

    }
    bool menuVisible = true;
    while (menuVisible) {
        watchJoystickPort();
        if (startingGame) {
            drawText(runningGame->title);
            this->menuOption = MENU_OPTION_START;
            menuVisible = false;
            startingGame = false;
            return;
        }

        if (resumingGui) {
            auto launcherScreen = new GuiLauncher(renderer);
            launcherScreen->show();
            delete launcherScreen;
            drawText("");
            resumingGui = false;
            menuSelection();
            menuVisible = false;

        }
        SDL_Event e;
        if (SDL_PollEvent(&e)) {

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SLEEP) {
                    drawText(_("POWERING OFF... PLEASE WAIT"));
                    Util::powerOff();

                }
            }



            // this is for pc Only
            if (e.type == SDL_QUIT) {

                menuVisible = false;
            }
            switch (e.type) {
                case SDL_JOYBUTTONUP:
                    if (adv != "false") {
                        if (!forceScan) {
                            if (e.jbutton.button == PCS_BTN_L1) {
                                Mix_PlayChannel(-1, cursor, 0);
                                drawText(mainMenu);
                                otherMenuShift = false;
                            }
                            if (e.jbutton.button == PCS_BTN_L2) {
                                Mix_PlayChannel(-1, cursor, 0);
                                powerOffShift = false;
                            }
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (adv != "false") {
                        if (!forceScan) {
                            if (e.jbutton.button == PCS_BTN_L1) {
                                Mix_PlayChannel(-1, cursor, 0);
                                drawText(otherMenu);
                                otherMenuShift = true;
                            }
                            if (e.jbutton.button == PCS_BTN_L2) {
                                Mix_PlayChannel(-1, cursor, 0);

                                powerOffShift = true;
                            }
                        }
                    }


                    if (powerOffShift) {
                        if (e.jbutton.button == PCS_BTN_R2) {
                            Mix_PlayChannel(-1, cursor, 0);
                            drawText(_("POWERING OFF... PLEASE WAIT"));
#if defined(__x86_64__) || defined(_M_X64)
                            exit(0);
#else
                            Util::execUnixCommad("shutdown -h now");
                                    sync();
                                    exit(1);
#endif
                        };
                    }

                    if (!otherMenuShift) {
                        if (!forceScan)
                            if (e.jbutton.button == PCS_BTN_START) {
                                if (cfg.inifile.values["ui"] == "classic") {
                                    Mix_PlayChannel(-1, cursor, 0);
                                    this->menuOption = MENU_OPTION_RUN;
                                    menuVisible = false;
                                } else {
                                    Mix_PlayChannel(-1, cursor, 0);
                                    drawText(_("Starting EvolutionUI"));
                                    loadAssets();
                                    auto launcherScreen = new GuiLauncher(renderer);
                                    launcherScreen->show();
                                    delete launcherScreen;

                                    menuSelection();
                                    menuVisible = false;
                                }
                            };

                        if (!forceScan)
                            if (retroarch != "false") {
                                if (e.jbutton.button == PCS_BTN_SQUARE) {
                                    Mix_PlayChannel(-1, cursor, 0);
                                    if (!Util::exists("/media/RetroArch/retroarch")) {
                                        auto confirm = new GuiConfirm(renderer);
                                        confirm->label = _("RetroArch is not installed");
                                        confirm->show();
                                        bool result = confirm->result;
                                        delete confirm;
                                        if (result) {
                                            this->menuOption = MENU_OPTION_RETRO;
                                            menuVisible = false;
                                        } else {
                                            menuSelection();
                                            menuVisible = false;
                                        }


                                    } else {
                                        this->menuOption = MENU_OPTION_RETRO;
                                        menuVisible = false;
                                    }
                                };
                            }

                        if (e.jbutton.button == PCS_BTN_CROSS) {
                            Mix_PlayChannel(-1, cursor, 0);
                            this->menuOption = MENU_OPTION_SCAN;

                            menuVisible = false;
                        };
                        if (e.jbutton.button == PCS_BTN_TRIANGLE) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto *aboutScreen = new GuiAbout(renderer);
                            aboutScreen->show();
                            delete aboutScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                        if (e.jbutton.button == PCS_BTN_SELECT) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto options = new GuiOptions(renderer);
                            options->show();
                            delete options;
                            menuSelection();
                            menuVisible = false;
                        };
                        if (!forceScan)
                            if (cfg.inifile.values["ui"] == "classic")
                                if (e.jbutton.button == PCS_BTN_CIRCLE) {
                                    Mix_PlayChannel(-1, cancel, 0);
                                    this->menuOption = MENU_OPTION_SONY;
                                    menuVisible = false;


                                };
                        break;
                    } else {
                        if (e.jbutton.button == PCS_BTN_CROSS) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto memcardsScreen = new GuiMemcards(renderer);
                            memcardsScreen->show();
                            delete memcardsScreen;

                            menuSelection();
                            menuVisible = false;
                        };

                        if (e.jbutton.button == PCS_BTN_CIRCLE) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto managerScreen = new GuiManager(renderer);
                            managerScreen->show();
                            delete managerScreen;

                            menuSelection();
                            menuVisible = false;
                        };


                    }


            }

        }
    }

}

void Gui::finish() {

    if (Mix_PlayingMusic()) {
        Mix_FadeOutMusic(300);
        while (Mix_PlayingMusic()) {

        }
    } else {
        usleep(300 * 1000);
    }

    SDL_DestroyTexture(backgroundImg);
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(buttonT);
    SDL_DestroyTexture(buttonO);
    SDL_DestroyTexture(buttonX);
    SDL_DestroyTexture(buttonS);
    SDL_DestroyTexture(buttonStart);
    SDL_DestroyTexture(buttonSelect);
    SDL_DestroyTexture(buttonL1);
    SDL_DestroyTexture(buttonR1);
    SDL_DestroyTexture(buttonL2);
    SDL_DestroyTexture(buttonR2);
    SDL_DestroyTexture(buttonCheck);
    SDL_DestroyTexture(buttonUncheck);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(cdJewel);
    TTF_CloseFont(font);
    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_FreeChunk(cursor);
    Mix_FreeChunk(cancel);
    Mix_FreeChunk(home_down);
    Mix_FreeChunk(home_up);
    Mix_CloseAudio();
    music = nullptr;
    backgroundImg = nullptr;
}


void Gui::getEmojiTextTexture(SDL_Renderer *renderer, string text, TTF_Font *font, SDL_Texture **texture,
                              SDL_Rect *rect) {
    if (text.empty()) text = " ";
    if (text.back() != '|') {
        text = text + "|";
    }

    vector<SDL_Texture *> textTexures;
    vector<string> textParts;
    std::string delimiter = "|";

    size_t pos = 0;
    std::string token;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        token = text.substr(0, pos);
        if (!token.empty())
            textParts.push_back(token);
        text.erase(0, pos + delimiter.length());
    }

    for (string str:textParts) {
        if (str.empty()) continue;
        if (str[0] == '@') {
            string icon = str.substr(1);
            if (icon == "Start") {
                textTexures.push_back(buttonStart);
            }
            if (icon == "S") {
                textTexures.push_back(buttonS);
            }
            if (icon == "O") {
                textTexures.push_back(buttonO);
            }
            if (icon == "Select") {
                textTexures.push_back(buttonSelect);
            }
            if (icon == "L1") {
                textTexures.push_back(buttonL1);
            }
            if (icon == "R1") {
                textTexures.push_back(buttonR1);
            }
            if (icon == "L2") {
                textTexures.push_back(buttonL2);
            }
            if (icon == "R2") {
                textTexures.push_back(buttonR2);
            }
            if (icon == "T") {
                textTexures.push_back(buttonT);
            }
            if (icon == "X") {
                textTexures.push_back(buttonX);
            }
            if (icon == "Check") {
                textTexures.push_back(buttonCheck);
            }
            if (icon == "Uncheck") {
                textTexures.push_back(buttonUncheck);
            }
        } else {
            SDL_Texture *textTex;
            SDL_Rect textRec;
            getTextAndRect(renderer, 0, atoi(themeData.values["ttop"].c_str()), str.c_str(), font, &textTex,
                           &textRec);
            textTexures.push_back(textTex);
        }
    }

    int w = 0;
    int h = 0;

    for (SDL_Texture *tex:textTexures) {

        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);

        w += tw;
        if (th > h) h = th;
    }


    *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, *texture);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int xpos = 0;
    for (SDL_Texture *tex:textTexures) {
        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);


        SDL_Rect posRect;
        posRect.x = xpos;

        posRect.y = 0;

        if (th != h) {
            posRect.y = (h - th) / 2;
        }
        posRect.w = tw;
        posRect.h = th;
        xpos += tw;
        SDL_RenderCopy(renderer, tex, nullptr, &posRect);
    }
    rect->w = w;
    rect->h = h;
    rect->x = 0;
    rect->y = 0;
    SDL_SetRenderTarget(renderer, nullptr);

    for (SDL_Texture *tex:textTexures) {
        if ((tex != buttonSelect) &&
            (tex != buttonS) &&
            (tex != buttonStart) &&
            (tex != buttonO) &&
            (tex != buttonT) &&
            (tex != buttonL1) &&
            (tex != buttonR1) &&
            (tex != buttonL2) &&
            (tex != buttonR2) &&
            (tex != buttonCheck) &&
            (tex != buttonUncheck) &&
            (tex != buttonX))

            SDL_DestroyTexture(tex);
    }
    textTexures.clear();
}

void Gui::renderStatus(string text) {

    string bg = themeData.values["text_bg"];

    SDL_Texture *textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["textalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(themeData.values["textx"].c_str());
    rect.y = atoi(themeData.values["texty"].c_str());
    rect.w = atoi(themeData.values["textw"].c_str());
    rect.h = atoi(themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    getEmojiTextTexture(renderer, text, font, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    textRec.y = atoi(themeData.values["ttop"].c_str());
    if (textRec.w > atoi(themeData.values["textw"].c_str())) textRec.w = atoi(themeData.values["textw"].c_str());
    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
    SDL_DestroyTexture(textTex);
}

void Gui::drawText(string text) {
    renderBackground();
    renderLogo(false);
    renderStatus(text);
    SDL_RenderPresent(renderer);
}

void Gui::renderLabelBox(int line, int offset) {
    SDL_Texture *textTex;
    SDL_Rect textRec;

    string bg = themeData.values["label_bg"];

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);

    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());


    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5;
    rectSelection.y = offset + textRec.h * (line);
    rectSelection.w = rect2.w - 10;
    rectSelection.h = textRec.h;


    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["keyalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rectSelection);
}

void Gui::renderSelectionBox(int line, int offset)
{
    renderSelectionBox(line,offset,0);
}
void Gui::renderSelectionBox(int line, int offset, int xoffset) {
    SDL_Texture *textTex;
    SDL_Rect textRec;

    string fg = themeData.values["text_fg"];

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);

    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());


    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5 + xoffset;
    rectSelection.y = offset + textRec.h * (line);
    rectSelection.w = rect2.w - 10 - xoffset;
    rectSelection.h = textRec.h;

    SDL_SetRenderDrawColor(renderer, getR(fg), getG(fg), getB(fg), 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawRect(renderer, &rectSelection);
}

int Gui::renderTextLine(string text, int line, int offset) {
    return renderTextLine(text, line, offset, false);
}

int Gui::renderTextLineOptions(string text, int line, int offset, bool center) {
    return renderTextLineOptions(text,line,offset,center,0);
}


int Gui::renderTextLineOptions(string text, int line, int offset, bool center, int xoffset) {
    int button = -1;
    if (text.find("|@Check|") != std::string::npos) {
        button = 1;
    }
    if (text.find("|@Uncheck|") != std::string::npos) {
        button = 0;
    }
    if (button != -1) {
        text = text.substr(0, text.find("|"));
    }

    int h = renderTextLine(text, line, offset, center, xoffset);

    SDL_Texture *buttonTex = nullptr;
    SDL_Rect rect;


    if (button == -1) {
        return h;
    }

    SDL_Rect textRec;
    SDL_Rect rect2;

    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());
    getTextAndRect(renderer, 0, 0, "*", font, &buttonTex, &textRec);
    int lineh = textRec.h;
    SDL_DestroyTexture(buttonTex);
    if (button == 1) {
        getEmojiTextTexture(renderer, "|@Check|", font, &buttonTex, &textRec);
    } else if (button == 0) {
        getEmojiTextTexture(renderer, "|@Uncheck|", font, &buttonTex, &textRec);
    }

    textRec.x = rect2.x + rect2.w - 10 - textRec.w;
    textRec.y = (lineh * line) + offset;

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (center) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }

    SDL_RenderCopy(renderer, buttonTex, nullptr, &textRec);
    SDL_DestroyTexture(buttonTex);
    return h;
}

int Gui::renderTextLine(string text, int line, int offset, bool center)
{
    return renderTextLine(text,line,offset,center,0);
}
int Gui::renderTextLine(string text, int line, int offset, bool center, int xoffset) {


    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_Texture *textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);
    SDL_DestroyTexture(textTex);
    int lineh = textRec.h;
    getEmojiTextTexture(renderer, text, font, &textTex, &textRec);
    textRec.x = rect2.x + 10 + xoffset;
    textRec.y = (lineh * line) + offset;

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (center) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }


    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
    SDL_DestroyTexture(textTex);


    return textRec.h;
}

void Gui::renderTextChar(string text, int line, int offset, int posx) {
    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_Texture *textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);
    getTextAndRect(renderer, posx, (textRec.h * line) + offset,
                   text.c_str(), font, &textTex, &textRec);

    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
    SDL_DestroyTexture(textTex);
}

void Gui::renderTextBar() {
    string bg = themeData.values["main_bg"];
    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["mainalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_RenderFillRect(renderer, &rect2);

}

void Gui::renderFreeSpace() {
    SDL_Texture *textTex;
    SDL_Rect textRec;
    SDL_Rect rect;

    rect.x = atoi(themeData.values["fsposx"].c_str());
    rect.y = atoi(themeData.values["fsposy"].c_str());
    getTextAndRect(renderer, 0, 0, "*", font, &textTex, &textRec);
    getEmojiTextTexture(renderer, _("Free space") + " : " + Util::getAvailableSpace(), font, &textTex, &textRec);
    rect.w = textRec.w;
    rect.h = textRec.h;
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, textTex, nullptr, &rect);
}

string Gui::getSonyImagePath() {
#if defined(__x86_64__) || defined(_M_X64)
    return "./sony/images";
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"/images";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data/images";
    }
    return path;
#endif
}

string Gui::getSonySoundPath() {
#if defined(__x86_64__) || defined(_M_X64)
    return "./sony/sounds";
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"/sounds";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data/sounds";
    }
    return path;
#endif
}

string Gui::getSonyFontPath() {
#if defined(__x86_64__) || defined(_M_X64)
    return "./sony/font";
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"/font";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data/font";
    }
    return path;
#endif
}

string Gui::getSonyRootPath() {
#if defined(__x86_64__) || defined(_M_X64)
    return "./sony";
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data";
    }
    return path;
#endif
}


void Gui::watchJoystickPort() {

    int numJoysticks = SDL_NumJoysticks();
    if (numJoysticks != joysticks.size()) {
        cout << "Pad changed" << endl;
        for (SDL_Joystick *joy:joysticks) {
            if (SDL_JoystickGetAttached(joy)) {
                SDL_JoystickClose(joy);
            }
            joysticks.clear();
        }
        SDL_Joystick *joystick;
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            joystick = SDL_JoystickOpen(i);
            joysticks.push_back(joystick);
            cout << "Pad connected" << endl;
        }

    }
}