//
// Created by screemer on 2019-01-23.
//
#pragma once

#include <string>
#include "../util.h"

//******************
// Memcard
//******************
class Memcard {
public:
    void newCard(std::string name);
    void deleteCard(std::string name);
    void backup(std::string path);
    void restore(std::string path);
    bool swapIn(std::string path, std::string name);
    void swapOut(std::string path, std::string name);
    std::vector<std::string> list();
    void storeToRepo(std::string path, std::string name);
    void rename(std::string oldName, std::string newName);
    void restoreAll(std::string mainDir);

    Memcard(std::string path) {
        this->path = path;
    }
private:
    std::string path;
};
