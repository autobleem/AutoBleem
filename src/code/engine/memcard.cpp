//
// Created by screemer on 2019-01-23.
//

#include "memcard.h"
#include "inifile.h"
#include "../DirEntry.h"

using namespace std;

//*******************************
// Memcard::newCard
//*******************************
void Memcard::newCard(string name)
{
    string curPath = path+DirEntry::separator() + "!MemCards/"+name;
    string autobleemPath = DirEntry::getWorkingPath();
    if (!DirEntry::exists(curPath))
    {
        DirEntry::createDir(curPath);
        DirEntry::copy(autobleemPath+DirEntry::separator()+"memcard/card1.mcd",curPath+DirEntry::separator()+"card1.mcd");
        DirEntry::copy(autobleemPath+DirEntry::separator()+"memcard/card2.mcd",curPath+DirEntry::separator()+"card2.mcd");
        DirEntry::copy(autobleemPath+DirEntry::separator()+"memcard/card1.mcd",curPath+DirEntry::separator()+"card1.bak");
        DirEntry::copy(autobleemPath+DirEntry::separator()+"memcard/card2.mcd",curPath+DirEntry::separator()+"card2.bak");
    }
}

//*******************************
// Memcard::deleteCard
//*******************************
void Memcard::deleteCard(string name)
{
    string curPath = path+DirEntry::separator()+"!MemCards/"+name;
    string autobleemPath = DirEntry::getWorkingPath();
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
    string customPath = this->path+DirEntry::separator()+"!MemCards/"+name;
    if (!DirEntry::exists(customPath))
    {
        restore(path);
        return false;
    } else
    {
        DirEntry::rmDir(path+DirEntry::separator()+"memcards");
        DirEntry::createDir(path+DirEntry::separator()+"memcards");
        for (const DirEntry & entry:DirEntry::diru(customPath))
        {
            DirEntry::copy(customPath + DirEntry::separator() + entry.name,path + DirEntry::separator() + "memcards" + DirEntry::separator() + entry.name);
        }
        return true;
    }
}

//*******************************
// Memcard::storeToRepo
//*******************************
void Memcard::storeToRepo(string path, string name)
{
    string customPath = this->path+DirEntry::separator() + "!MemCards/" + name;
    if (!DirEntry::exists(customPath))
    {
        DirEntry::createDir(customPath);
    }

    // copy memcard from game to repository
    for (const DirEntry & entry:DirEntry::diru(path))
    {
        string input = path+DirEntry::separator()+entry.name;
        string output = customPath+DirEntry::separator()+entry.name;
        DirEntry::copy(input,output);
    }
}

//*******************************
// Memcard::rename
//*******************************
void Memcard::rename(string oldName, string newName)
{
    string oldPath = this->path+DirEntry::separator()+"!MemCards/"+oldName;
    string newPath = this->path+DirEntry::separator()+"!MemCards/"+newName;

    if (DirEntry::exists(newPath))
    {
        // we already have memcard with this name
        return;
    }

    std::rename(oldPath.c_str(),newPath.c_str());

    // now go to all game ini's and find out if needs updated
    for (const DirEntry & entry: DirEntry::dir(path)) {
        if (entry.name[0] == '.') continue;
        if (!DirEntry::isDirectory(path+DirEntry::separator()+entry.name)) continue;
        if (entry.name == "!SaveStates") continue;
        if (entry.name == "!MemCards") continue;

        string gameIniPath = this->path+DirEntry::separator()+entry.name+DirEntry::separator()+"Game.ini";
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
    string customPath = this->path+DirEntry::separator()+"!MemCards";
    for (const DirEntry & entry: DirEntry::diru(customPath))
    {
        if (DirEntry::isDirectory(customPath + DirEntry::separator() + entry.name)) {
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
    string customPath = this->path+DirEntry::separator()+"!MemCards/"+name;
    if (!DirEntry::exists(customPath))
    {
        restore(path);
    } else
    {
        for (const DirEntry & entry:DirEntry::diru(customPath))
        {
            DirEntry::copy(path + DirEntry::separator() + "memcards" + DirEntry::separator() + entry.name,customPath + DirEntry::separator() + entry.name);
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
        string path = mainDir + DirEntry::separator() + entry.name;
        restore(path);
    }
}

//*******************************
// Memcard::backup
//*******************************
void Memcard::backup(const string & path)
{
    string curPath = path + DirEntry::separator() + "backup";
    if (!DirEntry::exists(curPath)) {
        DirEntry::createDir(curPath);
    } else
    {
        return;
    }

    string original = path + DirEntry::separator() + "memcards";
    for (const DirEntry & entry:DirEntry::diru(original))
    {
        DirEntry::copy(original+DirEntry::separator()+entry.name,curPath+DirEntry::separator()+entry.name);
    }
}

//*******************************
// Memcard::restore
//*******************************
void Memcard::restore(string path)
{
    string curPath = path+DirEntry::separator()+"backup";
    if (!DirEntry::exists(curPath)) {
        return;
    }

    string original = path+DirEntry::separator()+"memcards";
    DirEntry::rmDir(original);
    DirEntry::createDir(original);

    for (const DirEntry & entry:DirEntry::diru(curPath))
    {
        DirEntry::copy(curPath+DirEntry::separator()+entry.name,original+DirEntry::separator()+entry.name);
    }
    DirEntry::rmDir(curPath);
}