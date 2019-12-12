//
// Created by screemer on 2018-12-19.
//

#include <autobleem/autobleemui.h>
#include "gui.h"
#include "gui_about.h"
#include "gui_splash.h"
#include "gui_options.h"
#include "gui_memcards.h"
#include "gui_manager.h"
#include "gui_confirm.h"
#include <SDL2/SDL_image.h>
#include "../launcher/gui_launcher.h"
#include "gui_padconfig.h"
#include <fstream>
#include <unistd.h>
#include <autobleem/util.h>
#include <iostream>
#include <iomanip>
#include "../engine/scanner.h"
#include <json.h>
#include <fifo_map.h>


using namespace std;
using namespace nlohmann;

// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template<class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using ordered_json = basic_json<my_workaround_fifo_map>;


#define RA_PLAYLIST "AutoBleem.lpl"


                                    //*******************************
                                    // Gui
                                    //*******************************

//*******************************
// Gui::logText
//*******************************
void Gui::logText(const string &message) {
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
// Gui::loadAssets
//*******************************
void Gui::loadAssets(bool reloadMusic) {


    bool loaded = GfxTheme::load( getCurrentThemePath() + sep );
    if (!loaded)
    {
        GfxTheme::load(Env::getPathToThemesDir() + sep + "default" + sep);
        cfg.inifile.values["theme"] = "default";
        cfg.save();
    }


    if (cursor != nullptr) {
        Mix_FreeChunk(cursor);
        Mix_FreeChunk(cancel);
        Mix_FreeChunk(home_down);
        Mix_FreeChunk(home_up);
        Mix_FreeChunk(resume);
    }



    if (cfg.inifile.values["jewel"] != "none") {
        if (cfg.inifile.values["jewel"] == "default") {
            cdJewel = Gfx::loadImage( (Env::getWorkingPath() + sep + "evoimg/nofilter.png").c_str());
        } else {
            cdJewel = Gfx::loadImage(
                                      (Env::getWorkingPath() + sep + "evoimg/frames/" +
                                       cfg.inifile.values["jewel"]).c_str());
        }
    } else {
        cdJewel = nullptr;
    }
    string fontPath = (GfxTheme::themePath + GfxTheme::get("font"));
    int fontSize = 0;
    string fontSizeString = GfxTheme::get("fsize");
    if (fontSizeString != "")
        fontSize = atoi(fontSizeString.c_str());
    themeFont = Fonts::openFont(fontPath, fontSize);

    if (reloadMusic) {
        if (music != nullptr) {

            Mix_FreeMusic(music);
            music = nullptr;
        }
    }
    bool customMusic = false;
    int freq = 32000;
    string musicPath = GfxTheme::get("music");
    if (cfg.inifile.values["music"] != "--") {
        customMusic = true;
        musicPath = cfg.inifile.values["music"];
    }

    if (DirEntry::getFileExtension(musicPath) == "ogg") {
        freq = 44100;
    }

    if (reloadMusic) {
        int numtimesopened, frequency, channels;
        Uint16 format;
        numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);
        for (int i = 0; i < numtimesopened; i++) {
            Mix_CloseAudio();
        }
        numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);

        if (Mix_OpenAudio(freq, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
            printf("Unable to open audio: %s\n", Mix_GetError());
        }
    }

    cursor = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "cursor.wav").c_str());
    if (!cursor)
    {
        cursor = nullptr;
    }
    cancel = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "cancel.wav").c_str());
    home_up = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "home_up.wav").c_str());
    home_down = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "home_down.wav").c_str());
    resume = Mix_LoadWAV((this->getCurrentThemeSoundPath() + sep + "resume_new.wav").c_str());

    if (reloadMusic)
    if (cfg.inifile.values["nomusic"] != "true")
        if (GfxTheme::get("loop") != "-1") {


            if (!customMusic) {
                music = Mix_LoadMUS((GfxTheme::themePath + GfxTheme::get("music")).c_str());
                if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                if (Mix_PlayMusic(music, GfxTheme::get("loop") == "1" ? -1 : 0) == -1) {
                    printf("Unable to play music file: %s\n", Mix_GetError());
                }
            } else {
                music = Mix_LoadMUS((Env::getWorkingPath() + sep + "music/" + musicPath).c_str());
                if (music == nullptr) { printf("Unable to load Music file: %s\n", Mix_GetError()); }
                if (Mix_PlayMusic(music, -1) == -1) {
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
#if defined(__x86_64__) || defined(_M_X64)

#else
        SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    }
}

//*******************************
// Gui::criticalException
//*******************************
void Gui::criticalException(const string &text) {
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
void Gui::display(bool forceScan, const string &_pathToGamesDir, Database *db, bool resume) {
    joysticks.clear();
    joynames.clear();
    this->db = db;
    this->pathToGamesDir = _pathToGamesDir;
    this->forceScan = forceScan;
    if (forceScan) overrideQuickBoot = true;


    loadAssets();

    if (!resume) {
        auto *splashScreen = new GuiSplash();
        splashScreen->show();
        delete splashScreen;

        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            SDL_Joystick *joystick = SDL_JoystickOpen(i);
            if (!mapper.isKnownPad(SDL_JoystickInstanceID(joystick))) {
                cout << "New pad type" << endl;
                auto cfgPad = new GuiPadConfig();
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
    os << "AB_THEME=" << cfg.inifile.values["theme"] << endl;
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

int Gui::_cb(int button, SDL_Event *e) {
    return mapper.translateButton(button, e);
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
    if (scanner->noGamesFoundDuringScan) {
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
                        auto launcherScreen = new GuiLauncher();
                        launcherScreen->show();
                        delete launcherScreen;
                    }
                } else {
                    if (DirEntry::exists(Env::getPathToRetroarchDir() + sep + "retroarch")) {
                        this->menuOption = MENU_OPTION_RETRO;
                        return;
                    } else {
                        auto launcherScreen = new GuiLauncher();
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
            auto launcherScreen = new GuiLauncher();
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
                            if (e.jbutton.button == _cb(PCS_BTN_L1, &e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                drawText(mainMenu);
                                otherMenuShift = false;
                            }
                            if (e.jbutton.button == _cb(PCS_BTN_L2, &e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                powerOffShift = false;
                            }
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (adv != "false") {
                        if (!forceScan) {
                            if (e.jbutton.button == _cb(PCS_BTN_L1, &e)) {
                                Mix_PlayChannel(-1, cursor, 0);
                                drawText(otherMenu);
                                otherMenuShift = true;
                            }
                            if (e.jbutton.button == _cb(PCS_BTN_L2, &e)) {
                                Mix_PlayChannel(-1, cursor, 0);

                                powerOffShift = true;
                            }
                        }
                    }

                    if (powerOffShift) {
                        if (e.jbutton.button == _cb(PCS_BTN_R2, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            drawText(_("POWERING OFF... PLEASE WAIT"));
#if defined(__x86_64__) || defined(_M_X64)
                            exit(0);
#else
                            Util::execUnixCommand("shutdown -h now");
                                    sync();
                                    exit(1);
#endif
                        };
                    }

                    if (!otherMenuShift) {
                        if (!forceScan)
                            if (e.jbutton.button == _cb(PCS_BTN_START, &e)) {
                                if (cfg.inifile.values["ui"] == "classic") {
                                    Mix_PlayChannel(-1, cursor, 0);
                                    this->menuOption = MENU_OPTION_RUN;
                                    menuVisible = false;
                                } else {
                                    if (lastSet < 0) {
                                        lastSet = SET_ALL;
                                        lastSelIndex=0;
                                        resumingGui = false;
                                    }
                                    Mix_PlayChannel(-1, cursor, 0);
                                    drawText(_("Starting EvolutionUI"));
                                    loadAssets(false);
                                    auto launcherScreen = new GuiLauncher();
                                    launcherScreen->show();
                                    delete launcherScreen;

                                    menuSelection();
                                    menuVisible = false;
                                }
                            };

                        if (!forceScan)
                            if (retroarch != "false") {
                                if (e.jbutton.button == _cb(PCS_BTN_SQUARE, &e)) {
                                    Mix_PlayChannel(-1, cursor, 0);
                                    if (!DirEntry::exists(Env::getPathToRetroarchDir() + sep + "retroarch")) {

                                        auto confirm = new GuiConfirm();
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
                                        exportDBToRetroarch();
                                        this->menuOption = MENU_OPTION_RETRO;
                                        menuVisible = false;
                                    }
                                };
                            }

                        if (e.jbutton.button == _cb(PCS_BTN_CROSS, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            this->menuOption = MENU_OPTION_SCAN;

                            menuVisible = false;
                        };
                        if (e.jbutton.button == _cb(PCS_BTN_TRIANGLE, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto *aboutScreen = new GuiAbout();
                            aboutScreen->show();
                            delete aboutScreen;

                            menuSelection();
                            menuVisible = false;
                        };
                        if (e.jbutton.button == _cb(PCS_BTN_SELECT, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto options = new GuiOptions();
                            options->show();
                            delete options;
                            menuSelection();
                            menuVisible = false;
                        };
                        if (!forceScan)
                            if (cfg.inifile.values["ui"] == "classic")
                                if (e.jbutton.button == _cb(PCS_BTN_CIRCLE, &e)) {
                                    Mix_PlayChannel(-1, cancel, 0);
                                    this->menuOption = MENU_OPTION_SONY;
                                    menuVisible = false;
                                };
                        break;
                    } else {
                        if (e.jbutton.button == _cb(PCS_BTN_CROSS, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto memcardsScreen = new GuiMemcards();
                            memcardsScreen->show();
                            delete memcardsScreen;

                            menuSelection();
                            menuVisible = false;
                        };

                        if (e.jbutton.button == _cb(PCS_BTN_CIRCLE, &e)) {
                            Mix_PlayChannel(-1, cursor, 0);
                            auto managerScreen = new GuiManager();
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
    cursor = nullptr;
    cancel = nullptr;
    home_down = nullptr;
    home_up = nullptr;
    resume = nullptr;
}


//*******************************
// Gui::renderStatus
//*******************************
void Gui::renderStatus(const string &text, int posy) {
    string bg = GfxTheme::get("text_bg");

    GfxImage  textTex;
    SDL_Rect textRec;
    SDL_SetRenderDrawColor(renderer, GfxTheme::getR(bg), GfxTheme::getG(bg), GfxTheme::getB(bg), atoi(GfxTheme::get("textalpha").c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect;
    rect.x = atoi(GfxTheme::get("textx").c_str());
    rect.y = atoi(GfxTheme::get("texty").c_str());
    rect.w = atoi(GfxTheme::get("textw").c_str());
    rect.h = atoi(GfxTheme::get("texth").c_str());
    SDL_RenderFillRect(renderer, &rect);

    GfxTheme::getEmojiTextTexture(text, themeFont, &textTex, &textRec);
    int screencenter = 1280 / 2;
    textRec.x = screencenter - (textRec.w / 2);
    textRec.y = atoi(GfxTheme::get("ttop").c_str());
    if (posy!=-1)
    {
        textRec.y=posy;
    }
    if (textRec.w > atoi(GfxTheme::get("textw").c_str())) textRec.w = atoi(GfxTheme::get("textw").c_str());
    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
}

//*******************************
// Gui::drawText
//*******************************
void Gui::drawText(const string &text) {
    GfxTheme::drawBackground();
    GfxTheme::drawLogo(false);
    renderStatus(text);
    Gfx::flip();
}

//*******************************
// Gui::renderLabelBox
//*******************************
void Gui::renderLabelBox(int line, int offset) {
    GfxImage  textTex;

    string bg = GfxTheme::get("label_bg");

    GfxSize size = GfxTheme::getTextSize("*",themeFont);



    SDL_Rect rect2;
    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());

    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5;
    rectSelection.y = offset + size.h * (line);
    rectSelection.w = rect2.w - 10;
    rectSelection.h = size.h;


    SDL_SetRenderDrawColor(renderer, GfxTheme::getR(bg), GfxTheme::getG(bg), GfxTheme::getB(bg), atoi(GfxTheme::get("keyalpha").c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &rectSelection);
}

//*******************************
// Gui::renderSelectionBox
//*******************************
void Gui::renderSelectionBox(int line, int offset) {
    renderSelectionBox(line, offset, 0);
}

//*******************************
// Gui::renderSelectionBox
//*******************************
void Gui::renderSelectionBox(int line, int offset, int xoffset) {
    GfxImage  textTex;


    string fg = GfxTheme::get("text_fg");


    GfxSize size = GfxTheme::getTextSize("*", themeFont);



    SDL_Rect rect2;
    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());

    SDL_Rect rectSelection;
    rectSelection.x = rect2.x + 5 + xoffset;
    rectSelection.y = offset + size.h * (line);
    rectSelection.w = rect2.w - 10 - xoffset;
    rectSelection.h = size.h;

    SDL_SetRenderDrawColor(renderer, GfxTheme::getR(fg), GfxTheme::getG(fg), GfxTheme::getB(fg), 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawRect(renderer, &rectSelection);
}

//*******************************
// Gui::renderTextLine
//*******************************
int Gui::renderTextLine(const string &text, int line, int offset) {
    return renderTextLine(text, line, offset, POS_LEFT);
}

//*******************************
// Gui::renderTextLineOptions
//*******************************
int Gui::renderTextLineOptions(const string &text, int line, int offset, int position) {
    return renderTextLineOptions(text, line, offset, position, 0);
}

//*******************************
// Gui::renderTextLineOptions
//*******************************
int Gui::renderTextLineOptions(const string &_text, int line, int offset, int position, int xoffset) {
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

    int h = renderTextLine(text, line, offset, position, xoffset);

    GfxImage  buttonTex;
    SDL_Rect rect;

    if (button == -1) {
        return h;
    }

    SDL_Rect textRec;
    SDL_Rect rect2;

    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());
    GfxSize size = GfxTheme::getTextSize("*",themeFont);
    textRec.x=0; textRec.y=0; textRec.w=size.w; textRec.h=size.h;

    int lineh = textRec.h;
    if (button == 1) {
        GfxTheme::getEmojiTextTexture("|@Check|", themeFont, &buttonTex, &textRec);
    } else if (button == 0) {
        GfxTheme::getEmojiTextTexture( "|@Uncheck|", themeFont, &buttonTex, &textRec);
    }

    textRec.x = rect2.x + rect2.w - 10 - textRec.w;
    textRec.y = (lineh * line) + offset;

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (position==POS_CENTER) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }
    if (position==POS_RIGHT) {
        textRec.x = 1280 - textRec.x - textRec.w;
    }

    SDL_RenderCopy(renderer, buttonTex, nullptr, &textRec);
    return h;
}

//*******************************
// Gui::renderTextLine
//*******************************
int Gui::renderTextLine(const string &text, int line, int offset, int position) {
    return renderTextLine(text, line, offset, position, 0);
}

int Gui::renderTextLine(const string &text, int line, int offset,  int position, int xoffset)
{
    return renderTextLine(text,line,offset,position,xoffset,themeFont);
}

int Gui::renderTextLine(const string &text, int line, int offset,  int position, int xoffset, TTF_Font_Shared font) {
    SDL_Rect rect2;
    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());

    GfxImage  textTex;
    SDL_Rect textRec;

    GfxSize size = GfxTheme::getTextSize("*",themeFont);
    textRec.x=0; textRec.y=0; textRec.w=size.w; textRec.h=size.h;

    int lineh = textRec.h;
    GfxTheme::getEmojiTextTexture( text, font, &textTex, &textRec);
    textRec.x = rect2.x + 10 + xoffset;
    textRec.y = (lineh * line) + offset;

    if (line<0)
    {
        line=-line;
        textRec.y=line;
    }

    if (textRec.w >= (1280 - rect2.x * 4)) {
        textRec.w = (1280 - rect2.x * 4);
    }
    if (position==POS_CENTER) {
        textRec.x = (1280 / 2) - textRec.w / 2;
    }
    if (position==POS_RIGHT) {
        textRec.x = 1280 - textRec.x - textRec.w;
    }

    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);

    return textRec.h;
}

//*******************************
// Gui::renderTextChar
//*******************************
void Gui::renderTextChar(const string &text, int line, int offset, int posx) {
    SDL_Rect rect2;
    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());

    GfxImage  textTex;
    SDL_Rect textRec;


    GfxSize size = GfxTheme::getTextSize("*",themeFont);
    textRec.x=0; textRec.y=0; textRec.w=size.w; textRec.h=size.h;
    textRec.h= size.h;
    textTex = GfxTheme::getTextImage(text.c_str(), themeFont);
    GfxSize size2 = Gfx::getImageSize(textTex);
    textRec.x = posx; textRec.y = (textRec.h * line) + offset;
    textRec.w=size2.w;



    SDL_RenderCopy(renderer, textTex, nullptr, &textRec);
}

//*******************************
// Gui::renderTextBar
//*******************************
void Gui::renderTextBar() {
    string bg = GfxTheme::get("main_bg");
    SDL_SetRenderDrawColor(renderer, GfxTheme::getR(bg), GfxTheme::getG(bg), GfxTheme::getB(bg), atoi(GfxTheme::get("mainalpha").c_str()));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect rect2;
    rect2.x = atoi(GfxTheme::get("opscreenx").c_str());
    rect2.y = atoi(GfxTheme::get("opscreeny").c_str());
    rect2.w = atoi(GfxTheme::get("opscreenw").c_str());
    rect2.h = atoi(GfxTheme::get("opscreenh").c_str());

    SDL_RenderFillRect(renderer, &rect2);
}

//*******************************
// Gui::renderFreeSpace
//*******************************
void Gui::renderFreeSpace() {
    GfxImage  textTex;
    SDL_Rect textRec;
    SDL_Rect rect;

    rect.x = atoi(GfxTheme::get("fsposx").c_str());
    rect.y = atoi(GfxTheme::get("fsposy").c_str());

    GfxTheme::getEmojiTextTexture( _("Free space") + " : " + Util::getAvailableSpace(), themeFont, &textTex, &textRec);
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

void Gui::exportDBToRetroarch() {
    ordered_json j;
    j["version"]="1.0";

    PsGames gamesList;
    db->getGames(&gamesList);
    sort(gamesList.begin(), gamesList.end(), sortByTitle);

    ordered_json items = ordered_json::array();
    // copy the gamesList into json object
    for_each(begin(gamesList), end(gamesList), [&](PsGamePtr &game)
    {
        ordered_json item = ordered_json::object();

        string gameFile = (game->folder + sep + game->base);
        if (!DirEntry::matchExtension(game->base, ".pbp")) {
            gameFile += ".cue";
        }
        gameFile += "";

        string base;
        if (DirEntry::isPBPFile(game->base)) {
            base = game->base.substr(0, game->base.length() - 4);
        } else {
            base = game->base;
        }
        if (DirEntry::exists(game->folder + sep + base + ".m3u")) {
            gameFile = game->folder + sep + base + ".m3u";
        }


        item["path"]=gameFile;
        item["label"]=game->title;
        item["core_path"]=Env::getPathToRetroarchCoreFile();
        item["core_name"]="DETECT";
        item["crc32"]="00000000|crc";
        item["db_name"]=RA_PLAYLIST;

        items.push_back(item);
    });

    j["items"] = items;

    cout << j.dump() << endl;
    std::ofstream o(Env::getPathToRetroarchDir() + sep + "playlists/" + RA_PLAYLIST);
    o << std::setw(2) << j << std::endl;
    o.flush();
    o.close();
}