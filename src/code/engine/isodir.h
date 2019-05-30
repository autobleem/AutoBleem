//
// Created by screemer on 23.12.18.
//
#pragma once

#include "../main.h"
#include <string>
#include <vector>

#define SECTOR_SIZE 2352

//******************
// IsoDirectory
//******************
class IsoDirectory
{
public:
    std::string systemName;
    std::string volumeName;
    std::vector<std::string> rootDir;
};

//******************
// Isodir
//******************
class Isodir {
public:
    IsoDirectory getDir(std::string binPath,int maxlevel );
    void readDir(std::vector<std::string> * data, unsigned int sector, int maxlevel, int level);
private:
    IsoDirectory getEmptyDir();
    std::string removeVersion(std::string input);
    int getSectorAddress(int sector);
    unsigned char readChar();
    std::string readString(int size);
    unsigned long readDword();
    int findPrimaryDescriptor(int maxOffset);
    int offset = 0;
    std::ifstream * stream;
};
