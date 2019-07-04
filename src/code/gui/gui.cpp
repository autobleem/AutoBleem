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
#include <SDL2/SDL_image.h>
#include "../ver_migration.h"
#include "../lang.h"
#include "../launcher/gui_launcher.h"
#include "gui_padconfig.h"
#include <fstream>
#include <unistd.h>
#include "../util.h"
#include <iostream>
#include "../engine/scanner.h"

using namespace std;

//********************
// GuiBase::GuiBase
//********************
GuiBase::GuiBase() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Init();
    fonts[FONT_30] = TTF_OpenFont((getSonyFontPath() + "/SST-Bold.ttf").c_str(), 28);
    fonts[FONT_15] = TTF_OpenFont((getSonyFontPath() + "/SST-Bold.ttf").c_str(), 15);
    fonts[FONT_24] = TTF_OpenFont((getSonyFontPath() + "/SST-Medium.ttf").c_str(), 22);
}

//********************
// GuiBase::~GuiBase
//********************
GuiBase::~GuiBase() {
    SDL_Quit();
}

//*******************************
// GuiBase::getSonyImagePath
//*******************************
string GuiBase::getSonyImagePath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path =  Util::getWorkingPath() + Util::separator() + "themes" + Util::separator() + cfg.inifile.values["stheme"] + "/images";
    if (!Util::exists(path))
    {
        path = "./sony/images";
    }
    return path;
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"/images";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data/images";
    }
    return path;
#endif
}

//*******************************
// GuiBase::getSonySoundPath
//*******************************
string GuiBase::getSonySoundPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path =  Util::getWorkingPath() + Util::separator() + "themes" + Util::separator() + cfg.inifile.values["stheme"] + "/sounds";
    if (!Util::exists(path))
    {
        path = "./sony/sounds";
    }
    return path;
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"/sounds";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data/sounds";
    }
    return path;
#endif
}

//*******************************
// GuiBase::getSonyFontPath
//*******************************
string GuiBase::getSonyFontPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path =  Util::getWorkingPath() + Util::separator() + "themes" + Util::separator() + cfg.inifile.values["stheme"] + "/font";
    if (!Util::exists(path))
    {
        path = "./sony/font";
    }
    return path;
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"/font";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data/font";
    }
    return path;
#endif
}

//*******************************
// GuiBase::getSonyRootPath
//*******************************
string GuiBase::getSonyRootPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path =  Util::getWorkingPath() + Util::separator() + cfg.inifile.values["stheme"];
    if (!Util::exists(path))
    {
        path = "./sony";
    }
    return path;
#else
    string path =  "/media/themes/"+cfg.inifile.values["stheme"]+"";
    if (!Util::exists(path))
    {
        path = "/usr/sony/share/data";
    }
    return path;
#endif
}


//*******************************
// Gui::logText
//*******************************
void Gui::logText(const string & message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}


extern "C"
{
//*******************************
// Gui::logText
//*******************************
void logText(char *message) {
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->drawText(message);
}
}

//*******************************
// Gui::getR
//*******************************
Uint8 Gui::getR(const string & val) {
    return atoi(Util::commaSep(val, 0).c_str());
}

//*******************************
// Gui::getG
//*******************************
Uint8 Gui::getG(const string & val) {
    return atoi(Util::commaSep(val, 1).c_str());
}

//*******************************
// Gui::getB
//*******************************
Uint8 Gui::getB(const string & val) {
    return atoi(Util::commaSep(val, 2).c_str());
}

//*******************************
// Gui::getTextAndRect
//*******************************
void Gui::getTextAndRect(SDL_Shared<SDL_Renderer> renderer, int x, int y, const char *text, TTF_Font_Shared font,
                         SDL_Shared<SDL_Texture> *texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Shared<SDL_Surface> surface;
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
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

//*******************************
// Gui::renderBackground
//*******************************
void Gui::renderBackground() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundImg, nullptr, &backgroundRect);
}

//*******************************
// Gui::renderLogo
//*******************************
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

//*******************************
// Gui::loadThemeTexture
//*******************************
SDL_Shared<SDL_Texture> Gui::loadThemeTexture(SDL_Shared<SDL_Renderer> renderer, string themePath, string defaultPath, string texname) {
    SDL_Shared<SDL_Texture> tex = nullptr;
    if (Util::exists(themePath + themeData.values[texname])) {
        tex = IMG_LoadTexture(renderer, (themePath + themeData.values[texname]).c_str());
    } else {
        tex = IMG_LoadTexture(renderer, (defaultPath + defaultData.values[texname]).c_str());
    }
    return tex;
}

//*******************************
// Gui::loadAssets
//*******************************
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
    int fontSize = 0;
    string fontSizeString = themeData.values["fsize"];
    if (fontSizeString != "")
        fontSize = atoi(fontSizeString.c_str());
    auto temp = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (temp) {
        cout << "Success opening font " << fontPath << " of size " << fontSize << endl;
        themeFont = temp;
    }
    else {
        cout << "FAILURE opening font " << fontPath << " of size " << fontSize << endl;
        themeFont = fonts[FONT_24]; // do not set themeFont to nullptr
    }

    if (music != nullptr) {

        Mix_FreeMusic(music);
        music = nullptr;
    }

    bool customMusic = false;
    int freq = 32000;
    string musicPath = themeData.values["music"];
    if (cfg.inifile.values["music"] != "--")
    {
        customMusic = true;
        musicPath = cfg.inifile.values["music"];
    }

    if (Util::getFileExtension(musicPath)=="ogg")
    {
        freq = 44100;
    }

    int numtimesopened, frequency, channels;
    Uint16 format;
    numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
    for (int i=0;i<numtimesopened;i++)
    {
        Mix_CloseAudio();
    }
    numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);

    if (Mix_OpenAudio(freq, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        printf("Unable to open audio: %s\n", Mix_GetError());
    }

    cursor = Mix_LoadWAV((this->getSonySoundPath() + "/cursor.wav").c_str());
    cancel = Mix_LoadWAV((this->getSonySoundPath() + "/cancel.wav").c_str());
    home_up = Mix_LoadWAV((this->getSonySoundPath() + "/home_up.wav").c_str());
    home_down = Mix_LoadWAV((this->getSonySoundPath() + "/home_down.wav").c_str());
    resume = Mix_LoadWAV((this->getSonySoundPath() + "/resume_new.wav").c_str());

    if (cfg.inifile.values["nomusic"] != "true")
        if (themeData.values["loop"] != "-1") {


            if (!customMusic) {
                music = Mix_LoadMUS((themePath + themeData.values["music"]).c_str());
                if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                if (Mix_PlayMusic(music, themeData.values["loop"] == "1" ? -1 : 0) == -1) {
                    printf("Unable to play music file: %s\n", Mix_GetError());
                }
            } else {
                music = Mix_LoadMUS((Util::getWorkingPath() +"/music/"+ musicPath).c_str());
                if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                if (Mix_PlayMusic(music,  -1 ) == -1) {
                    printf("Unable to play music file: %s\n", Mix_GetError());
                }
            }

        }

}

//*******************************
// Gui::waitForGamepad
//*******************************
void Gui::waitForGamepad() {
    int joysticksFound = SDL_NumJoysticks();
    while (joysticksFound == 0) {
        drawText(_("PLEASE CONNECT GAMEPAD TO PLAYSTATION CLASSIC"));
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        joysticksFound = SDL_NumJoysticks();
    }




}

//*******************************
// Gui::criticalException
//*******************************
void Gui::criticalException(const string & text) {
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

//*******************************
// Gui::display
//*******************************
void Gui::display(bool forceScan, string path, Database *db, bool resume) {
    joysticks.clear();
    joynames.clear();
    this->db = db;
    this->path = path;
    this->forceScan = forceScan;
    if (forceScan) overrideQuickBoot = true;

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

    loadAssets();


    if (!resume) {
        auto *splashScreen = new GuiSplash(renderer);
        splashScreen->show();
        delete splashScreen;

        drawText(_("Importing internal games"));
        Util::execUnixCommad("/media/Autobleem/rc/backup_internal.sh");



            for (int i = 0; i < SDL_NumJoysticks(); i++) {
                SDL_Joystick* joystick = SDL_JoystickOpen(i);
                if (!mapper.isKnownPad(SDL_JoystickInstanceID(joystick)))
                {
                    cout << "New pad type" <<endl;
                    auto cfgPad = new GuiPadConfig(renderer);
                    cfgPad->joyid = SDL_JoystickInstanceID(joystick);
                    cfgPad->show();
                    delete cfgPad;
                }

            }


        if (cfg.inifile.values["quick"] != "true")
            waitForGamepad();
    } else {
        resumingGui = true;
        overrideQuickBoot = true;
    }


}

//*******************************
// Gui::saveSelection
//*******************************
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

//*******************************
// Gui::quickBoot
//*******************************
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

        int delay = TicksPerSecond;

        if (cfg.inifile.values["delay"] != "") {
            delay = delay * atoi(cfg.inifile.values["delay"].c_str());
        }
        int newTime = SDL_GetTicks();
        int secs = (delay / TicksPerSecond) - (newTime - currentTime) / TicksPerSecond;
        if (newTime - currentTime > delay) {
            return true;
        }
    }
}

int Gui::_cb(int button, SDL_Event *e)
{
    return mapper.translateButton(button,e);
}


//*******************************
// Gui::menuSelection
//*******************************
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
                            if (e.jbutton.button == _cb(PCS_BTN_L1,&e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                drawText(mainMenu);
                                otherMenuShift = false;
                            }
                            if (e.jbutton.button == _cb(PCS_BTN_L2,&e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                powerOffShift = false;
                            }
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:

                    if (adv != "false") {
                        if (!forceScan) {
                            if (e.jbutton.button == _cb(PCS_BTN_L1,&e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                drawText(otherMenu);
                                otherMenuShift = true;
                            }
                            if (e.jbutton.button == _cb(PCS_BTN_L2,&e)) {
                                Mix_PlayChannel(-1, cursor, 0);

                                powerOffShift = true;
                            }
                        }
                    }


                    if (powerOffShift) {
                        if (e.jbutton.button == _cb(PCS_BTN_R2,&e)) {
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
                            if (e.jbutton.button == _cb(PCS_BTN_START,&e)) {
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
                                if (e.jbutton.button == _cb(PCS_BTN_SQUARE,&e)) {
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

                        if (e.jbutton.button == _cb(PCS_BTN_CROSS,&e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            this->menuOption = MENU_OPTION_SCAN;

                            menuVisible = false;
                        };
                        if (e.jbutton.button == _cb(PCS_BTN_TRIANGLE,&e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto *aboutScreen = new GuiAbout(renderer);
                            aboutScreen->show();
                            delete aboutScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                        if (e.jbutton.button == _cb(PCS_BTN_SELECT,&e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto options = new GuiOptions(renderer);
                            options->show();
                            delete options;
                            menuSelection();
                            menuVisible = false;
                        };
                        if (!forceScan)
                            if (cfg.inifile.values["ui"] == "classic")
                                if (e.jbutton.button == _cb(PCS_BTN_CIRCLE,&e)) {
                                    Mix_PlayChannel(-1, cancel, 0);
                                    this->menuOption = MENU_OPTION_SONY;
                                    menuVisible = false;


                                };
                        break;
                    } else {
                        if (e.jbutton.button == _cb(PCS_BTN_CROSS,&e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto memcardsScreen = new GuiMemcards(renderer);
                            memcardsScreen->show();
                            delete memcardsScreen;

                            menuSelection();
                            menuVisible = false;
                        };

                        if (e.jbutton.button == _cb(PCS_BTN_CIRCLE,&e)) {
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

//*******************************
// Gui::finish
//*******************************
void Gui::finish() {

    if (Mix_PlayingMusic()) {
        Mix_FadeOutMusic(300);
        while (Mix_PlayingMusic()) {

        }
    } else {
        usleep(300 * TicksPerSecond);
    }

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

//*******************************
// Gui::getEmojiTextTexture
//*******************************
void Gui::getEmojiTextTexture(SDL_Shared<SDL_Renderer> renderer, string text, TTF_Font_Shared font, SDL_Shared<SDL_Texture> *texture,
                              SDL_Rect *rect) {
    if (text.empty()) text = " ";
    if (text.back() != '|') {
        text = text + "|";
    }

    vector<SDL_Shared<SDL_Texture>> textTexures;
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

    for (const string & str:textParts) {
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
            SDL_Shared<SDL_Texture> textTex = nullptr;
            SDL_Rect textRec;
            getTextAndRect(renderer, 0, atoi(themeData.values["ttop"].c_str()), str.c_str(), font, &textTex,
                           &textRec);
            textTexures.push_back(textTex);
        }
    }

    int w = 0;
    int h = 0;

    for (SDL_Shared<SDL_Texture> tex:textTexures) {

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
    for (SDL_Shared<SDL_Texture> tex:textTexures) {
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

    textTexures.clear();
}

//*******************************
// Gui::renderStatus
//*******************************
void Gui::renderStatus(const string & text) {

    string bg = themeData.values["text_bg"];

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, getR(bg), getG(bg), getB(bg), atoi(themeData.values["textalpha"].c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(themeData.values["textx"].c_str());
    rect.y = atoi(themeData.values["texty"].c_str());
    rect.w = atoi(themeData.values["textw"].c_str());
    rect.h = atoi(themeData.values["texth"].c_str());
    SDL_RenderFillRect(renderer, &rect);

    getEmojiTextTexture(renderer, text, themeFont, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    textRec.y = atoi(themeData.values["ttop"].c_str());
    if (textRec.w > atoi(themeData.values["textw"].c_str())) textRec.w = atoi(themeData.values["textw"].c_str());
    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
}

//*******************************
// Gui::drawText
//*******************************
void Gui::drawText(const string & text) {
    renderBackground();
    renderLogo(false);
    renderStatus(text);
    SDL_RenderPresent(renderer);
}

//*******************************
// Gui::renderLabelBox
//*******************************
void Gui::renderLabelBox(int line, int offset) {
    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    string bg = themeData.values["label_bg"];

    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);

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

//*******************************
// Gui::renderSelectionBox
//*******************************
void Gui::renderSelectionBox(int line, int offset)
{
    renderSelectionBox(line,offset,0);
}

//*******************************
// Gui::renderSelectionBox
//*******************************
void Gui::renderSelectionBox(int line, int offset, int xoffset) {
    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    string fg = themeData.values["text_fg"];

    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);

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

//*******************************
// Gui::renderTextLine
//*******************************
int Gui::renderTextLine(const string & text, int line, int offset) {
    return renderTextLine(text, line, offset, false);
}

//*******************************
// Gui::renderTextLineOptions
//*******************************
int Gui::renderTextLineOptions(const string & text, int line, int offset, bool center) {
    return renderTextLineOptions(text,line,offset,center,0);
}

//*******************************
// Gui::renderTextLineOptions
//*******************************
int Gui::renderTextLineOptions(const string & _text, int line, int offset, bool center, int xoffset) {
    string text = _text;
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

    SDL_Shared<SDL_Texture> buttonTex = nullptr;
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
    getTextAndRect(renderer, 0, 0, "*", themeFont, &buttonTex, &textRec);
    int lineh = textRec.h;
    if (button == 1) {
        getEmojiTextTexture(renderer, "|@Check|", themeFont, &buttonTex, &textRec);
    } else if (button == 0) {
        getEmojiTextTexture(renderer, "|@Uncheck|", themeFont, &buttonTex, &textRec);
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
    return h;
}

//*******************************
// Gui::renderTextLine
//*******************************
int Gui::renderTextLine(const string & text, int line, int offset, bool center)
{
    return renderTextLine(text,line,offset,center,0);
}
int Gui::renderTextLine(const string & text, int line, int offset, bool center, int xoffset) {


    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);
    int lineh = textRec.h;
    getEmojiTextTexture(renderer, text, themeFont, &textTex, &textRec);
    textRec.x = rect2.x + 10 + xoffset;
    textRec.y = (lineh * line) + offset;

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (center) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }


    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);


    return textRec.h;
}

//*******************************
// Gui::renderTextChar
//*******************************
void Gui::renderTextChar(const string & text, int line, int offset, int posx) {
    SDL_Rect rect2;
    rect2.x = atoi(themeData.values["opscreenx"].c_str());
    rect2.y = atoi(themeData.values["opscreeny"].c_str());
    rect2.w = atoi(themeData.values["opscreenw"].c_str());
    rect2.h = atoi(themeData.values["opscreenh"].c_str());

    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;

    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);
    getTextAndRect(renderer, posx, (textRec.h * line) + offset,
                   text.c_str(), themeFont, &textTex, &textRec);

    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
}

//*******************************
// Gui::renderTextBar
//*******************************
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

//*******************************
// Gui::renderFreeSpace
//*******************************
void Gui::renderFreeSpace() {
    SDL_Shared<SDL_Texture> textTex;
    SDL_Rect textRec;
    SDL_Rect rect;

    rect.x = atoi(themeData.values["fsposx"].c_str());
    rect.y = atoi(themeData.values["fsposy"].c_str());
    getTextAndRect(renderer, 0, 0, "*", themeFont, &textTex, &textRec);
    getEmojiTextTexture(renderer, _("Free space") + " : " + Util::getAvailableSpace(), themeFont, &textTex, &textRec);
    rect.w = textRec.w;
    rect.h = textRec.h;
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, textTex, nullptr, &rect);
}

//*******************************
// Gui::watchJoystickPort
//*******************************
void Gui::watchJoystickPort() {

    int numJoysticks = SDL_NumJoysticks();
    if (numJoysticks != joysticks.size()) {
        cout << "Pad changed" << endl;
        for (SDL_Joystick *joy:joysticks) {
            if (SDL_JoystickGetAttached(joy)) {
                SDL_JoystickClose(joy);
            }
            joysticks.clear();
            joynames.clear();
        }
        SDL_Joystick *joystick;
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            joystick = SDL_JoystickOpen(i);
            joysticks.push_back(joystick);
            joynames.push_back(SDL_JoystickName(joystick));
            cout << "Pad connected" << endl;
            cout << "--" << SDL_JoystickName(joystick) << endl;

        }

    }
}