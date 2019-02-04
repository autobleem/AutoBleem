//
// Created by screemer on 2/4/19.
//

#include "lang.h"
#include "util.h"


string _(string input)
{
    shared_ptr <Lang> lang(Lang::getInstance());

    return lang->translate(input);
}

string Lang::translate(string input)
{
    if (input.empty()) return "";
    string translated = langData[input];
    if (translated=="")
    {
        langData[input]=input;
        translated = input;
    }
    return translated;
}

void Lang::dump(string fileName)
{
    string fileSave = Util::getWorkingPath()+Util::separator()+fileName;
    map<string, string>::iterator it;

    ofstream os(fileSave);
    for ( it = langData.begin(); it != langData.end(); it++ )
    {
        std::cout << it->first  // string (key)
                  << ':'
                  << it->second   // string's value
                  << std::endl ;
        os << it->first << endl << it->second <<endl;
    }
    os.flush();
    os.close();
}

