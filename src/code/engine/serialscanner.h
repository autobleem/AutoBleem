//
// Created by screemer on 2/3/19.
//

#ifndef AUTOBLEEM_GUI_SERIALSCANNER_H
#define AUTOBLEEM_GUI_SERIALSCANNER_H

#include "string"
using namespace std;
class SerialScanner {
public:
    string fixSerial(string serial);
    string scanSerial(int imageType, string path, string firstBinPath="");
    string scanSerialInternal(int imageType, string path, string firstBinPath="");
    string workarounds(int imageType, string path, string firstBinPath);
    string serialByMd5(string scanFile);
};


#endif //AUTOBLEEM_GUI_SERIALSCANNER_H
