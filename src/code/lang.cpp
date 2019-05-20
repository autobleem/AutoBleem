//
// Created by screemer on 2/4/19.
//

#include "lang.h"
#include "util.h"
#include <fstream>
#include <iostream>
using namespace std;

//*******************************
// string _(string input)
//*******************************
string _(string input) {
    shared_ptr<Lang> lang(Lang::getInstance());

    return lang->translate(input);
}

//*******************************
// Lang::translate
//*******************************
string Lang::translate(string input){
    if (currentLang == "English") return input;
    trim(input);
    if (input.empty()) return "";
    string translated = langData[input];
    if (translated == "") {
        langData[input] = input;
        translated = input;
        newData.push_back(input);
    }
    return translated;
}

//*******************************
// Lang::load
//*******************************
void Lang::load(string lang) {
    string path = Util::getWorkingPath() + Util::separator() + "lang" + Util::separator() + lang + ".txt";
    langData.clear();
    newData.clear();
    currentLang = lang;
    if (lang == "English") return;

    ifstream is(path);
    string line;
    std::vector<std::string> lines;
    while (std::getline(is, line)) {
        trim(line);
        lines.push_back(line);
    }
    for (int i = 0; i < lines.size(); i += 2) {
        if (i+1<lines.size()) {
            langData[lines[i]] = lines[i + 1];
        }
    }
    is.close();
}

//*******************************
// Lang::dump
//*******************************
void Lang::dump(string fileName) {

    string fileSave = Util::getWorkingPath() + Util::separator() + fileName;
    map<string, string>::iterator it;

    ofstream os(fileSave);
    for (string data:newData) {
        cout << data << endl;
        os << data << endl << data << endl;
    }
    os.flush();
    os.close();
}

//*******************************
// Lang::listLanguages
//*******************************
vector<string> Lang::listLanguages()
{
    vector<string> languages;
    languages.push_back("English");
    for (DirEntry entry:Util::diru(Util::getWorkingPath() + Util::separator() + "lang"))
    {
        if (Util::matchExtension(entry.name,".txt"))
        {
            languages.push_back(entry.name.substr(0,entry.name.size()-4));
        }
    }
    return languages;
}
