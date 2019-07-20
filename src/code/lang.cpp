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
string _(const string & input) {
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
void Lang::load(string languageName) {
    string path = Util::getWorkingPath() + Util::separator() + "lang" + Util::separator() + languageName + ".txt";
    langData.clear();
    newData.clear();
    currentLang = languageName;
    if (languageName == "English") return;

    ifstream is(path);
    string line;
    std::vector<std::string> lines;
    int lineNum = 0;
    while (std::getline(is, line)) {
        // if this is the first line of the file and the beginning of the string contains the UTF-8 header
        // strip the UTF-8 header off
        if (lineNum == 0 && line.size() >= 3) {
            unsigned char* p = (unsigned char*) line.c_str();
            if ((p[0] == 0xEF) && (p[1] == 0xBB) && (p[2] == 0xBF)) {
                string skipUTF8Header =  (char*) p + 3;
                line = skipUTF8Header;
            }
        }
        trim(line);
        lines.push_back(line);
        ++lineNum;
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
// Lang::getListOfLanguages
//*******************************
vector<string> Lang::getListOfLanguages()
{
    vector<string> languages;
    languages.push_back("English");
    for (DirEntry entry:Util::diru(Util::getWorkingPath() + Util::separator() + "lang"))
    {
        // if it's a*.txt file but not English.txt, add it to the list of languages
        if (Util::matchExtension(entry.name,".txt") && ! Util::matchesLowercase(entry.name, "English.txt"))
        {
            languages.push_back(entry.name.substr(0,entry.name.size()-4));
        }
    }
    return languages;
}
