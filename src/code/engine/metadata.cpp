//
// Created by screemer on 2018-12-16.
//

#include "metadata.h"
#include "database.h"
#include "../gui/gui.h"
using namespace std;

//*******************************
// Metadata::lookupBySerial
//*******************************
bool Metadata::lookupBySerial(string serial) {
    shared_ptr<Gui> gui(Gui::getInstance());
    for (int i = 0; i < 3; i++) {
            Database *db= gui->coverdb->covers[i];
            if (db== nullptr) continue;

            if (db->querySerial(serial, this)) {
                this->lastRegion = gui->coverdb->regionStr[i];
                return true;
            }
    }

    return false;
}

//*******************************
// Metadata::lookupByTitle
//*******************************
bool Metadata::lookupByTitle(string title) {
    shared_ptr<Gui> gui(Gui::getInstance());
    for (int i = 0; i < 3; i++) {
        Database *db= gui->coverdb->covers[i];
        if (db== nullptr) continue;

            if (db->queryTitle(title, this)) {
                this->lastRegion = gui->coverdb->regionStr[i];
                return true;
            }

    }

    return false;
}