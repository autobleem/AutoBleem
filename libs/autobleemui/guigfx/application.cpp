//
// Created by screemer on 2019-12-11.
//


#include <SDL2/SDL_mixer.h>
#include "application.h"

using namespace std;

SDL_Shared<SDL_Renderer> Application::renderer = nullptr;
SDL_Shared<SDL_Window> Application::window = nullptr;

//*******************************
// Application
//*******************************

//********************
// Application::Application
//********************
Application::Application() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    window = SDL_CreateWindow("AutoBleem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

#if defined(__x86_64__) || defined(_M_X64)

#else
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif


    TTF_Init();
    fonts[FONT_30] = TTF_OpenFont((getCurrentThemeFontPath() + sep + "SST-Bold.ttf").c_str(), 28);
    fonts[FONT_15] = TTF_OpenFont((getCurrentThemeFontPath() + sep + "SST-Bold.ttf").c_str(), 15);
    fonts[FONT_24] = TTF_OpenFont((getCurrentThemeFontPath() + sep + "SST-Medium.ttf").c_str(), 22);

    mapper.init();

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
}

//********************
// Application::Applicationtion
//********************
Application::~Application() {
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

//*******************************
// Application::getCurrentThemePath
//*******************************
string Application::getCurrentThemePath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = Env::getPathToThemesDir() + sep + cfg.inifile.values["theme"];
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath();
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data";
    }
    return path;
#endif
}

//*******************************
// Application::getCurrentThemeImagePath
//*******************************
string Application::getCurrentThemeImagePath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = getCurrentThemePath() + sep + "images";
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath() + sep + "images";
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "/images";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data/images";
    }
    return path;
#endif
}

//*******************************
// Application::getCurrentThemeSoundPath
//*******************************
string Application::getCurrentThemeSoundPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = getCurrentThemePath() + sep + "sounds";
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath() + sep + "sounds";
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "/sounds";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data/sounds";
    }
    return path;
#endif
}

//*******************************
// Application::getCurrentThemeFontPath
//*******************************
string Application::getCurrentThemeFontPath() {
#if defined(__x86_64__) || defined(_M_X64)
    string path = getCurrentThemePath() + sep + "font";
    if (!DirEntry::exists(path)) {
        path = Env::getSonyPath() + sep + "font";
    }
    return path;
#else
    string path =  "/media/themes/" + cfg.inifile.values["theme"] + "/font";
    if (!DirEntry::exists(path))
    {
        path = "/usr/sony/share/data/font";
    }
    return path;
#endif
}