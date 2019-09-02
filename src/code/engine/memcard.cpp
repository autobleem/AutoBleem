//
// Created by screemer on 2019-01-23.
//

#include "memcard.h"
#include "inifile.h"
#include "../DirEntry.h"
#include "../environment.h"

using namespace std;

//*******************************
// Memcard::newCard
//*******************************
void Memcard::newCard(string name)
{
    string curPath = path + sep + "!MemCards/"+name;
    string autobleemPath = Env::getWorkingPath();
    if (!DirEntry::exists(curPath))
    {
        DirEntry::createDir(curPath);
        DirEntry::copy(autobleemPath + sep + "memcard/card1.mcd", curPath + sep + "card1.mcd");
        DirEntry::copy(autobleemPath + sep + "memcard/card2.mcd", curPath + sep + "card2.mcd");
        DirEntry::copy(autobleemPath + sep + "memcard/card1.mcd", curPath + sep + "card1.bak");
        DirEntry::copy(autobleemPath + sep + "memcard/card2.mcd", curPath + sep + "card2.bak");
    }
}

//*******************************
// Memcard::deleteCard
//*******************************
void Memcard::deleteCard(string name)
{
    string curPath = path + sep + "!MemCards/" + name;
    string autobleemPath = Env::getWorkingPath();
    if (DirEntry::exists(curPath))
    {
        DirEntry::rmDir(curPath);
    }
}

//*******************************
// Memcard::swapIn
//*******************************
bool Memcard::swapIn(string path, string name)
{
    backup(path);
    string customPath = this->path + sep + "!MemCards/" + name;
    if (!DirEntry::exists(customPath))
    {
        restore(path);
        return false;
    } else
    {
        DirEntry::rmDir(path + sep + "memcards");
        DirEntry::createDir(path + sep + "memcards");
        for (const DirEntry & entry : DirEntry::diru(customPath))
        {
            DirEntry::copy(customPath + sep + entry.name, path + sep + "memcards" + sep + entry.name);
        }
        return true;
    }
}

//*******************************
// Memcard::storeToRepo
//*******************************
void Memcard::storeToRepo(string path, string name)
{
    string customPath = this->path + sep + "!MemCards/" + name;
    if (!DirEntry::exists(customPath))
    {
        DirEntry::createDir(customPath);
    }

    // copy memcard from game to repository
    for (const DirEntry & entry : DirEntry::diru(path))
    {
        string input = path + sep + entry.name;
        string output = customPath + sep + entry.name;
        DirEntry::copy(input,output);
    }
}

//*******************************
// Memcard::rename
//*******************************
void Memcard::rename(string oldName, string newName)
{
    string oldPath = this->path + sep + "!MemCards/"+oldName;
    string newPath = this->path + sep + "!MemCards/"+newName;

    if (DirEntry::exists(newPath))
    {
        // we already have memcard with this name
        return;
    }

    std::rename(oldPath.c_str(),newPath.c_str());

    // now go to all game ini's and find out if needs updated
    for (const DirEntry & entry: DirEntry::diru(path)) {
        if (!DirEntry::isDirectory(path + sep + entry.name)) continue;
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;

        string gameIniPath = this->path + sep + entry.name + sep + "Game.ini";
        if (DirEntry::exists(gameIniPath))
        {
            Inifile inifile;
            inifile.load(gameIniPath);

            if (inifile.values["memcard"]==oldName)
            {
                inifile.values["memcard"]=newName;
                inifile.save(gameIniPath);
            }
        }
    }
}

//*******************************
// Memcard::list
//*******************************
vector<string> Memcard::list()
{
    vector<string> memcards;
    string customPath = this->path + sep + "!MemCards";
    for (const DirEntry & entry: DirEntry::diru(customPath))
    {
        if (DirEntry::isDirectory(customPath + sep + entry.name)) {
            memcards.push_back(entry.name);
        }
    }
    return memcards;
}

//*******************************
// Memcard::swapOut
//*******************************
void Memcard::swapOut(string path, string name)
{
    string customPath = this->path + sep + "!MemCards/"+name;
    if (!DirEntry::exists(customPath))
    {
        restore(path);
    } else
    {
        for (const DirEntry & entry : DirEntry::diru(customPath))
        {
            DirEntry::copy(path + sep + "memcards" + sep + entry.name, customPath + sep + entry.name);
        }
        restore(path);
    }
}

//*******************************
// Memcard::restoreAll
//*******************************
void Memcard::restoreAll(string mainDir)
{
    for (const DirEntry & entry: DirEntry::diru(mainDir))
    {
        string path = mainDir + sep + entry.name;
        restore(path);
    }
}

//*******************************
// Memcard::backup
//*******************************
void Memcard::backup(const string & path)
{
    string curPath = path + sep + "backup";
    if (!DirEntry::exists(curPath)) {
        DirEntry::createDir(curPath);
    } else
    {
        return;
    }

    string original = path + sep + "memcards";
    for (const DirEntry & entry : DirEntry::diru(original))
    {
        DirEntry::copy(original + sep + entry.name, curPath + sep + entry.name);
    }
}

//*******************************
// Memcard::restore
//*******************************
void Memcard::restore(string path)
{
    string curPath = path + sep + "backup";
    if (!DirEntry::exists(curPath)) {
        return;
    }

    string original = path + sep + "memcards";
    DirEntry::rmDir(original);
    DirEntry::createDir(original);

    for (const DirEntry & entry : DirEntry::diru(curPath))
    {
        DirEntry::copy(curPath + sep + entry.name, original + sep + entry.name);
    }
    DirEntry::rmDir(curPath);
}