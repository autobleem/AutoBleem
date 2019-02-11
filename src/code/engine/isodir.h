//
// Created by screemer on 23.12.18.
//

#ifndef CBLEEMSYNC_ISODIR_H
#define CBLEEMSYNC_ISODIR_H

#include "../main.h"

using namespace std;
#define SECTOR_SIZE 2352

class IsoDirectory
{
public:
    string systemName;
    string volumeName;
    vector<string> rootDir;
};

class Isodir {
public:
    IsoDirectory getDir(string binPath,int maxlevel );
    void readDir(vector<string> * data, unsigned int sector, int maxlevel, int level);
private:
    IsoDirectory getEmptyDir();
    string removeVersion(string input);
    int getSectorAddress(int sector);
    unsigned char readChar();
    string readString(int size);
    unsigned long readDword();
    int findPrimaryDescriptor(int maxOffset);
    int offset = 0;
    ifstream * stream;

};


#endif //CBLEEMSYNC_ISODIR_H
