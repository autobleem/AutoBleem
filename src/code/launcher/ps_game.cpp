//
// Created by screemer on 2/12/19.
//

#include "ps_game.h"
#include "../util.h"
#include "../engine/inifile.h"
#include "../gui/gui.h"


void PsGame::loadTex(SDL_Renderer *renderer) {
    if (coverPng == nullptr) {
        string imagePath = this->folder + Util::separator() + this->base + ".png";
        coverPng = IMG_LoadTexture(renderer, imagePath.c_str());
    }

}

void PsGame::freeTex() {
    if (coverPng != nullptr) {
        SDL_DestroyTexture(coverPng);
        coverPng = nullptr;
    }


}

PsGame *PsGame::clone() {
    PsGame *clone = new PsGame();
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
    return clone;
}

void PsGame::setMemCard(string name)
{
   this->memcard = name;
   Inifile * ini = new Inifile();
   ini->load(this->folder+"/Game.ini");
   ini->values["memcard"] = name;
   ini->save(this->folder+"/Game.ini");
   shared_ptr<Gui> gui(Gui::getInstance());
   gui->db->updateMemcard(this->gameId,name);
}

string PsGame::findResumePicture()
{
    // try to do it in silly Sony way
    string filenamefile = ssFolder+"filename.txt.res";
    if (Util::exists(filenamefile))
    {
        ifstream is(filenamefile.c_str());
        if (is.is_open()) {

            std::string line;
            std::getline(is, line);
            std::getline(is, line);

            // last line is our filename
            string pngfile = ssFolder+"screenshots/"+line+".png.res";
            if (Util::exists(pngfile))
            {
                return pngfile;
            }
            is.close();
        }
    }
    return "";
}