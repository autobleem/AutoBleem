//
// Created by screemer on 2019-01-23.
//

#ifndef AUTOBLEEM_GUI_MEMCARD_H
#define AUTOBLEEM_GUI_MEMCARD_H

#include <string>
#include "util.h"
using namespace std;

class Memcard {
public:
    void newCard(string name);
    void deleteCard(string name);
    void backup(string path);
    void restore(string path);
    bool swapIn(string path, string name);
    void swapOut(string path, string name);
    vector<string> list();
    void storeToRepo(string path, string name);

    void restoreAll(string mainDir);

    Memcard(string path) {
        this->path = path;
    }
private:
    string path;
};


#endif //AUTOBLEEM_GUI_MEMCARD_H
