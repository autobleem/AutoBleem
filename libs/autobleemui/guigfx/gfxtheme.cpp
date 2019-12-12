//
// Created by screemer on 2019-12-12.
//

#include <autobleem/environment.h>
#include <autobleem/direntry.h>
#include "gfxtheme.h"
#include "gfx.h"
#include "gui.h"

string GfxTheme::themePath;
Inifile GfxTheme::themeData;
Inifile GfxTheme::defaultData;

GfxImage  GfxTheme::backgroundImg;
GfxImage  GfxTheme::logo;
GfxImage  GfxTheme::buttonX;
GfxImage  GfxTheme::buttonO;
GfxImage  GfxTheme::buttonT;
GfxImage  GfxTheme::buttonS;
GfxImage  GfxTheme::buttonStart;
GfxImage  GfxTheme::buttonSelect;
GfxImage  GfxTheme::buttonL1;
GfxImage  GfxTheme::buttonR1;
GfxImage  GfxTheme::buttonL2;
GfxImage  GfxTheme::buttonR2;
GfxImage  GfxTheme::buttonCheck;
GfxImage  GfxTheme::buttonUncheck;

GfxImage
GfxTheme::loadThemeTexture(string themePath, string defaultPath, string texname) {
    GfxImage  tex = nullptr;
    if (DirEntry::exists(themePath + themeData.values[texname])) {
        tex = Gfx::loadImage( (themePath + themeData.values[texname]).c_str());
    } else {
        tex = Gfx::loadImage( (defaultPath + defaultData.values[texname]).c_str());
    }
    return tex;
}

bool GfxTheme::load(string currentThemePath)
{
    bool result = true;
    themePath = currentThemePath;
    string defaultPath = Env::getPathToThemesDir() + sep + "default" + sep;
    themePath = currentThemePath + sep;

    if (!DirEntry::exists(themePath + "theme.ini"))
    {
        themePath=defaultPath;
        result = false;
    }
    themeData.load(defaultPath + "theme.ini");
    defaultData.load(defaultPath + "theme.ini");
    themeData.load(themePath + "theme.ini");

    backgroundImg = loadThemeTexture(themePath, defaultPath, "background");
    logo = loadThemeTexture(themePath, defaultPath, "logo");

    buttonO = loadThemeTexture( themePath, defaultPath, "circle");
    buttonX = loadThemeTexture( themePath, defaultPath, "cross");
    buttonT = loadThemeTexture( themePath, defaultPath, "triangle");
    buttonS = loadThemeTexture( themePath, defaultPath, "square");
    buttonSelect = loadThemeTexture( themePath, defaultPath, "select");
    buttonStart = loadThemeTexture( themePath, defaultPath, "start");
    buttonL1 = loadThemeTexture( themePath, defaultPath, "l1");
    buttonR1 = loadThemeTexture( themePath, defaultPath, "r1");
    buttonL2 = loadThemeTexture( themePath, defaultPath, "l2");
    buttonR2 = loadThemeTexture( themePath, defaultPath, "r2");
    buttonCheck = loadThemeTexture(themePath, defaultPath, "check");
    buttonUncheck = loadThemeTexture( themePath, defaultPath, "uncheck");
    return result;
}


void GfxTheme::drawBackground() {
    Gfx::clear();
    Gfx::drawImage(GfxTheme::backgroundImg,0,0,1280,720);
}


int GfxTheme::drawLogo(bool small) {
    SDL_Rect logoRect;
    logoRect.x = atoi(get("lpositionx").c_str());
    logoRect.y = atoi(get("lpositiony").c_str());
    logoRect.w = atoi(get("lw").c_str());
    logoRect.h = atoi(get("lh").c_str());
    if (!small) {
        Gfx::drawImage(GfxTheme::logo,logoRect.x,logoRect.y,logoRect.w,logoRect.h);
        return 0;
    } else {
        Gfx::drawImage(GfxTheme::logo, atoi(GfxTheme::get("opscreenx").c_str()), atoi(GfxTheme::get("opscreeny").c_str()), logoRect.w/3, logoRect.h/3);
        return atoi(GfxTheme::get("opscreeny").c_str()) + logoRect.h/3;
    }
}

//*******************************
// Gui::getTextAndRect
//*******************************
GfxImage GfxTheme::getTextImage(const char *text, TTF_Font_Shared font) {
    GfxImage texture;

    SDL_Shared<SDL_Surface> surface;
    string fg = get("text_fg");
    SDL_Color textColor = {getR(fg), getG(fg), getB(fg), 0};

    if (strlen(text) == 0) {
        texture = SDL_CreateTexture(Application::renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 0, 0);
        return texture;
    }

    surface = TTF_RenderUTF8_Blended(font, text, textColor);
    texture = SDL_CreateTextureFromSurface(Application::renderer, surface);


    return texture;
}

GfxSize GfxTheme::getTextSize(const char *text, TTF_Font_Shared font)
{
    GfxSize size;
    GfxImage texture;

    SDL_Shared<SDL_Surface> surface;
    string fg = get("text_fg");
    SDL_Color textColor = {getR(fg), getG(fg), getB(fg), 0};

    if (strlen("*") == 0) {
        size.w=0; size.h=0;
        return size;
    }

    surface = TTF_RenderUTF8_Blended(font, text, textColor);
    texture = SDL_CreateTextureFromSurface(Application::renderer, surface);
    size = Gfx::getImageSize(texture);

    return size;
}



void GfxTheme::getEmojiTextTexture(string text, TTF_Font_Shared font,
                              GfxImage  *texture,
                              SDL_Rect *rect) {
    if (text.empty()) text = " ";
    if (text.back() != '|') {
        text = text + "|";
    }

    vector<GfxImage > textTexures;
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

    for (const string &str:textParts) {
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

            GfxImage  textTex = getTextImage( str.c_str(), font);
            textTexures.push_back(textTex);
        }
    }

    int w = 0;
    int h = 0;

    for (GfxImage  tex:textTexures) {
        Uint32 format;
        int access;
        int tw, th;
        SDL_QueryTexture(tex, &format, &access, &tw, &th);

        w += tw;
        if (th > h) h = th;
    }

    *texture = SDL_CreateTexture(Application::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(Application::renderer, *texture);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);

    Gfx::clear();

    int xpos = 0;
    for (GfxImage  tex:textTexures) {
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
        SDL_RenderCopy(Application::renderer, tex, nullptr, &posRect);
    }
    rect->w = w;
    rect->h = h;
    rect->x = 0;
    rect->y = 0;
    SDL_SetRenderTarget(Application::renderer, nullptr);

    textTexures.clear();
}


Uint8 GfxTheme::getR(const string &val) {
    return atoi(Util::commaSep(val, 0).c_str());
}


Uint8 GfxTheme::getG(const string &val) {
    return atoi(Util::commaSep(val, 1).c_str());
}


Uint8 GfxTheme::getB(const string &val) {
    return atoi(Util::commaSep(val, 2).c_str());
}

