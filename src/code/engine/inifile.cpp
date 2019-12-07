//
// Created by screemer on 2018-12-19.
//

#include "inifile.h"
#include "../main.h"
#include <iostream>
#include <fstream>
#include "../util.h"

using namespace std;

//*******************************
// Inifile::load
//*******************************
void Inifile::load(const string & _path) {
    this->path = _path;
    //cout << "Reading ini file: " << path << endl;
    ifstream file;
    string iniLine;
    file.open(path);
    values.clear(); // remove values from any prior load()

    if (!file.good()) {
        cout << "Error opening ini file: " << path << endl;
        return;
    }

    while (getline(file, iniLine)) {
        iniLine = trim(iniLine);
        if (iniLine.length() == 0) continue;    // blank line
        if (iniLine[0] == '#') continue;        // treat a line beginning with # as a comment
        if (iniLine[0]=='[')
        {
            iniLine = ltrim(iniLine);
            iniLine = iniLine.substr(1,iniLine.find(']')-1);
            section = iniLine;
        }
        if (iniLine.find('=') != string::npos) {
            iniLine = lcase(iniLine, iniLine.find('='));
            string paramName = iniLine.substr(0, iniLine.find('='));
            string paramVal = iniLine.substr(iniLine.find('=') + 1, string::npos);
            if (paramName == "publisher")
                Util::cleanPublisherString(paramVal);
            values[paramName] = paramVal;
        }

        if (file.eof()) break;
    };
    file.close();
}

//*******************************
// Inifile::save
//*******************************
void Inifile::save(const string & _path) {
    cout << "Writing ini file: " << _path << endl;
    ofstream os;
    os.open(_path);
    os << "[" << section <<"]" << endl;
    for (map<string,string>::iterator iter = values.begin(); iter != values.end(); ++iter)
    {
        string k =  iter->first;
        string v = iter->second;
        k=lcase(k);
        if (k == "publisher")
            Util::cleanPublisherString(v);
        k[0]=toupper(k[0]);

        os << k << "=" << v << endl;
    }
    os.flush();
    os.close();
}

//*******************************
// Inifile::print
//*******************************
void Inifile::print() {
    cout << "section = " << section << '\n';
    cout << "path = " << path << '\n';
    cout << "entry = " << entry << '\n';

    for (auto & item : values)
        cout << item.first << " = " << item.second << '\n';
    cout << flush;
}