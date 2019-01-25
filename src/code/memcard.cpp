//
// Created by screemer on 2019-01-23.
//

#include "memcard.h"
void Memcard::newCard(string name)
{
    string curPath = path+Util::separator()+"!MemCards/"+name;
    string autobleemPath = Util::getWorkingPath();
    if (!Util::exists(curPath))
    {
        Util::createDir(curPath);
        Util::copy(autobleemPath+Util::separator()+"memcard/card1.mcd",curPath+Util::separator()+"card1.mcd");
        Util::copy(autobleemPath+Util::separator()+"memcard/card2.mcd",curPath+Util::separator()+"card2.mcd");
        Util::copy(autobleemPath+Util::separator()+"memcard/card1.mcd",curPath+Util::separator()+"card1.bak");
        Util::copy(autobleemPath+Util::separator()+"memcard/card2.mcd",curPath+Util::separator()+"card2.bak");

    }
}

void Memcard::deleteCard(string name)
{
    string curPath = path+Util::separator()+"!MemCards/"+name;
    string autobleemPath = Util::getWorkingPath();
    if (Util::exists(curPath))
    {


        Util::rmDir(curPath);

    }
}

bool Memcard::swapIn(string path, string name)
{
    backup(path);
    string customPath = this->path+Util::separator()+"!MemCards/"+name;
    if (!Util::exists(customPath))
    {
        restore(path);
        return false;
    } else
    {
        Util::rmDir(path+Util::separator()+"memcards");
        Util::createDir(path+Util::separator()+"memcards");
        for (DirEntry entry:Util::diru(customPath))
        {
            Util::copy(customPath+Util::separator()+entry.name,path+Util::separator()+"memcards"+Util::separator()+entry.name);
        }
        return true;
    }

}

void Memcard::storeToRepo(string path, string name)
{

    string customPath = this->path+Util::separator()+"!MemCards/"+name;
    if (!Util::exists(customPath))
    {
        newCard(name);
    }

    // copy memcard from game to repository
    for (DirEntry entry:Util::diru(path))
    {
        string input = path+Util::separator()+entry.name;
        string output = customPath+Util::separator()+name+Util::separator()+entry.name;
        Util::copy(input,output);
    }



}

vector<string> Memcard::list()
{
    vector<string> memcards;
    string customPath = this->path+Util::separator()+"!MemCards";
    for (DirEntry entry: Util::diru(customPath))
    {
        if (entry.dir)
        {
            memcards.push_back(entry.name);
        }
    }
    return memcards;
}

void Memcard::swapOut(string path, string name)
{
    string customPath = this->path+Util::separator()+"!MemCards/"+name;
    if (!Util::exists(customPath))
    {
        restore(path);
    } else
    {

        for (DirEntry entry:Util::diru(customPath))
        {
            Util::copy(path+Util::separator()+"memcards"+Util::separator()+entry.name,customPath+Util::separator()+entry.name);
        }
        restore(path);

    }
}

void Memcard::restoreAll(string mainDir)
{
    for (DirEntry entry: Util::diru(mainDir))
    {
        string path = mainDir+Util::separator()+entry.name;
        restore(path);
    }
}

void Memcard::backup(string path)
{
    string curPath = path+Util::separator()+"backup";
    if (!Util::exists(curPath)) {
        Util::createDir(curPath);
    } else
    {
        return;
    }

    string original = path+Util::separator()+"memcards";
    for (DirEntry entry:Util::diru(original))
    {
        Util::copy(original+Util::separator()+entry.name,curPath+Util::separator()+entry.name);
    }
}

void Memcard::restore(string path)
{
    string curPath = path+Util::separator()+"backup";
    if (!Util::exists(curPath)) {
        return;
    }

    string original = path+Util::separator()+"memcards";
    Util::rmDir(original);
    Util::createDir(original);

    for (DirEntry entry:Util::diru(curPath))
    {
        Util::copy(curPath+Util::separator()+entry.name,original+Util::separator()+entry.name);
    }
    Util::rmDir(curPath);
}