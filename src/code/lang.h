//
// Created by screemer on 2/4/19.
//

#pragma once

#include <map>
#include <string>
#include <memory>
#include "util.h"
#include "main.h"
using namespace std;

string _(string input);

//******************
// Lang
//******************
class Lang {
public:

    string currentLang;
    string translate(string input);
    void dump(string fileName);
    void load(string langfile);
    vector<string> listLanguages();

    Lang(Lang const &) = delete;
    Lang &operator=(Lang const &) = delete;
    static std::shared_ptr<Lang> getInstance() {
        static std::shared_ptr<Lang> s{new Lang};
        return s;
    }
private:
    Lang() {};
    map<string,string> langData;
    vector<string> newData;

};
