//
// Created by screemer on 2/3/19.
//
#pragma once

#include <string>

//******************
// SerialScanner
//******************
// The SerialScanner class reads the serial number in a CDROM BIN file which is an ISO 9660 image of a CDROM.
// https://en.wikipedia.org/wiki/ISO_9660
class SerialScanner {
public:
    static std::string fixSerial(std::string serial);
    static std::string scanSerial(int imageType, std::string path, std::string firstBinPath="");
    static std::string scanSerialInternal(int imageType, std::string path, std::string firstBinPath="");
    static std::string workarounds(int imageType, std::string path, std::string firstBinPath);
    static std::string serialByMd5(std::string scanFile);
};
