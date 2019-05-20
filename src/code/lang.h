//
// Created by screemer on 2/4/19.
//

#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>

std::string _(std::string input);

//******************
// Lang
//******************
class Lang {
public:

    std::string currentLang;
    std::string translate(std::string input);
    void dump(std::string fileName);
    void load(std::string langfile);
    std::vector<std::string> listLanguages();

    Lang(Lang const &) = delete;
    Lang &operator=(Lang const &) = delete;
    static std::shared_ptr<Lang> getInstance() {
        static std::shared_ptr<Lang> s{new Lang};
        return s;
    }
private:
    Lang() {};
    std::map<std::string,std::string> langData;
    std::vector<std::string> newData;
};
