//
// Created by screemer on 2/12/19.
//

#include "ps_game.h"
#include "../util.h"
#include "../engine/inifile.h"
#include "../gui/gui.h"
using namespace std;

void PsGame::loadTex(SDL_Renderer *renderer) {
    shared_ptr<Gui> gui(Gui::getInstance());

    if (coverPng == nullptr) {

        SDL_Texture *renderSurface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 226,
                                                       226);
        SDL_SetRenderTarget(renderer, renderSurface);
        SDL_RenderClear(renderer);
        SDL_Rect fullRect;

        string imagePath = this->folder + Util::separator() + this->base + ".png";
        if (Util::exists(imagePath)) {
            coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
        } else coverPng = nullptr;

        if (coverPng != nullptr) {
            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.h = 226, fullRect.w = 226;

            Uint32 format;
            int access;
            SDL_QueryTexture(coverPng, &format, &access, &fullRect.w, &fullRect.h);

            SDL_Rect outputRect;
            if (gui->cdJewel != nullptr) {
                outputRect.x = 23;
                outputRect.y = 5;
                outputRect.h = 217;
                outputRect.w = 199;
            } else {
                outputRect.x = 0;
                outputRect.y = 0;
                outputRect.h = 226;
                outputRect.w = 226;
            }
            if (coverPng != nullptr) {
                SDL_RenderCopy(renderer, coverPng, &fullRect, &outputRect);
            }
            if (coverPng != nullptr) {
                SDL_DestroyTexture(coverPng);
                coverPng = nullptr;
            }


            fullRect.x = 0;
            fullRect.y = 0;
            fullRect.h = 226, fullRect.w = 226;
            if (gui->cdJewel != nullptr) {
                SDL_RenderCopy(renderer, gui->cdJewel, &fullRect, &fullRect);
            }
            coverPng = renderSurface;
        }
        SDL_SetRenderTarget(renderer, nullptr);
    }

}

void PsGame::freeTex() {
    if (coverPng != nullptr) {

        SDL_DestroyTexture(coverPng);
        coverPng = nullptr;
    }


}


bool PsGame::isCleanExit() {
    string filenamefile = ssFolder + "filename.txt";
    return Util::exists(filenamefile);
}

#if 0
shared_ptr<PsGame> PsGame::clone() {
    shared_ptr<PsGame> clone{new PsGame};
    clone->visible = this->visible;
    clone->gameId = this->gameId;
    clone->folder = this->folder;
    clone->title = this->title;
    clone->base = this->base;
    clone->ssFolder = this->ssFolder;
    clone->memcard = this->memcard;
    clone->players = this->players;
    clone->publisher = this->publisher;
    clone->coverPng = this->coverPng;
    clone->year = this->year;
    clone->internal = this->internal;
    clone->favorite = this->favorite;

    return clone;
}
#endif

void PsGame::setMemCard(string name) {
    this->memcard = name;
    Inifile *ini = new Inifile();
    ini->load(this->folder + "/Game.ini");
    ini->values["memcard"] = name;
    ini->save(this->folder + "/Game.ini");
    shared_ptr<Gui> gui(Gui::getInstance());
    gui->db->updateMemcard(this->gameId, name);
}

void PsGame::removeResumePoint(int slot) {
    // TODO: Remove ssfile
    string filenamefile = ssFolder + "filename.txt.res";
    string filenamepoint = ssFolder + "filename." + to_string(slot) + ".txt.res";
    if (Util::exists(filenamepoint)) {
        filenamefile = filenamepoint;
    }
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            string ssfile = ssFolder + "sstates/" + line + ".00" + to_string(slot) + ".res";
            remove(ssfile.c_str());
            // last line is our filename
            if (slot == 0) {
                string slot0img = ssFolder + "screenshots/" + line + ".png.res";
                remove(slot0img.c_str());

            } else {
                string slotnimg = ssFolder + "screenshots/" + line + "." + to_string(slot) + ".png.res";
                remove(slotnimg.c_str());
            }
            is.close();
        }
    }
}

bool PsGame::isResumeSlotActive(int slot) {
    string filenamefile = ssFolder + "filename.txt.res";
    string filenamepoint = ssFolder + "filename." + to_string(slot) + ".txt.res";
    if (Util::exists(filenamepoint)) {
        filenamefile = filenamepoint;
    }
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            if (slot == 0) {
                string slot0img = ssFolder + "screenshots/" + line + ".png.res";
                if (Util::exists(slot0img)) {
                    return true;
                }
            } else {
                string slotnimg = ssFolder + "screenshots/" + line + "." + to_string(slot) + ".png.res";
                if (Util::exists(slotnimg)) {
                    return true;
                }
            }
            is.close();
        }
    }
    return false;
}

void PsGame::storeResumePicture(int slot) {
    string filenamefile = ssFolder + "filename.txt.res";
    string filenamepoint = ssFolder + "filename." + to_string(slot) + ".txt.res";
    if (Util::exists(filenamepoint)) {
        filenamefile = filenamepoint;
    }
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            string inputImg = ssFolder + "screenshots/" + line + ".png";
            if (!Util::exists(inputImg)) {
                return;
            }
            string slotImg;
            // last line is our filename
            if (slot == 0) {
                slotImg = ssFolder + "screenshots/" + line + ".png.res";

            } else {
                slotImg = ssFolder + "screenshots/" + line + "." + to_string(slot) + ".png.res";

            }
            is.close();

            remove(slotImg.c_str());
            Util::copy(inputImg, slotImg);
            remove(inputImg.c_str());

        }
    }

}

string PsGame::findResumePicture(int slot) {
    string filenamefile = ssFolder + "filename.txt.res";
    string filenamepoint = ssFolder + "filename." + to_string(slot) + ".txt.res";
    if (Util::exists(filenamepoint)) {
        filenamefile = filenamepoint;
    }
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            if (slot == 0) {
                string slot0img = ssFolder + "screenshots/" + line + ".png.res";
                if (Util::exists(slot0img)) {
                    return slot0img;
                }
            } else {
                string slotnimg = ssFolder + "screenshots/" + line + "." + to_string(slot) + ".png.res";
                if (Util::exists(slotnimg)) {
                    return slotnimg;
                }
            }
            is.close();
        }
    }
    return "";
}


string PsGame::findResumePicture() {
    // try to do it in silly Sony way
    string filenamefile = ssFolder + "filename.txt.res";
    for (int i = 0; i < 4; i++) {
        string filenamepoint = ssFolder + "filename." + to_string(i) + ".txt.res";
        if (Util::exists(filenamepoint)) {
            filenamefile = filenamepoint;
            break;
        }
    }
    if (Util::exists(filenamefile)) {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            string pngfile = ssFolder + "screenshots/" + line + ".png.res";
            if (Util::exists(pngfile)) {
                return pngfile;
            }
            is.close();
        }
    }
    return "";
}