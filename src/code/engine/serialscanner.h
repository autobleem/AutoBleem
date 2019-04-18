//
// Created by screemer on 2/3/19.
//

#ifndef AUTOBLEEM_GUI_SERIALSCANNER_H
#define AUTOBLEEM_GUI_SERIALSCANNER_H

#include "string"
using namespace std;

// The SerialScanner class reads the serial number in a CDROM BIN file which is an ISO 9660 image of a CDROM.
// https://en.wikipedia.org/wiki/ISO_9660

class SerialScanner {
public:
    string fixSerial(string serial);
    string scanSerial(int imageType, string path, string firstBinPath="");
    string scanSerialInternal(int imageType, string path, string firstBinPath="");
    string workarounds(int imageType, string path, string firstBinPath);
    string serialByMd5(string scanFile);
};


#endif //AUTOBLEEM_GUI_SERIALSCANNER_H
