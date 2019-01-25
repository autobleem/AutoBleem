//
// Created by screemer on 2018-12-19.
//

#include "inifile.h"


void Inifile::load(string path) {
    this->path = path;
    ifstream file;
    string iniLine;
    file.open(path);

    if (!file.good()) {
        return;

    }
    while (getline(file, iniLine)) {
        iniLine = trim(iniLine);
        if (iniLine.length() == 0) continue;
        if (iniLine[0]=='[')
        {
            iniLine = ltrim(iniLine);
            iniLine = iniLine.substr(1,iniLine.length()-1);
            section = iniLine;
        }
        if (iniLine.find('=') != string::npos) {
            iniLine = lcase(iniLine, iniLine.find('='));
            string paramName = iniLine.substr(0, iniLine.find('='));
            string paramVal = iniLine.substr(iniLine.find('=') + 1, string::npos);
            values[paramName] = paramVal;
        }

        if (file.eof()) break;
    };
    file.close();
}

void Inifile::save(string path) {
    cout << "Writting ini file" << path << endl;
    ofstream os;
    os.open(path);
    os << "[" << section <<"]" << endl;
    for(std::map<string,string>::iterator iter = values.begin(); iter != values.end(); ++iter)
    {
        string k =  iter->first;
        string v = iter->second;
        k=lcase(k);
        k[0]=toupper(k[0]);

        os << k << "=" << v << endl;
    }
    os.flush();
    os.close();

}