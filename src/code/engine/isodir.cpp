//
// Created by screemer on 23.12.18.
//

#include "isodir.h"
#include <fstream>

using namespace std;

//*******************************
// Isodir::removeVersion
//*******************************
string Isodir::removeVersion(string input)
{
    int len=input.length();
    if (len>2)
    {
        int semiColon = input.find(';',0);
        if (semiColon!=-1)
        {
            input = input.substr(0,input.find(';',0));
        }
    }
    return input;
}

//*******************************
// Isodir::getSectorAddress
//*******************************
int Isodir::getSectorAddress(int sector) {
    return sector * SECTOR_SIZE + offset;
}

//*******************************
// Isodir::readChar
//*******************************
unsigned char Isodir::readChar() {
    unsigned char x;
    stream->read(reinterpret_cast<char *>(&x), 1);

    return x;
}

//*******************************
// Isodir::readString
//*******************************
string Isodir::readString(int size) {
    char str[size + 1];
    str[size] = 0;
    stream->read(str, size);
    return str;
}

//*******************************
// Isodir::readDword
//*******************************
unsigned long Isodir::readDword() {
    unsigned long res = 0;
    unsigned long c;
    c = readChar();
    res += c;
    c = readChar();
    res += c << (1 * 8);
    c = readChar();
    res += c << (2 * 8);
    c = readChar();
    res += c << (3 * 8);
    return res;
}

//*******************************
// Isodir::findPrimaryDescriptor
//*******************************
int Isodir::findPrimaryDescriptor(int maxOffset) {
    unsigned char c=0;
    int addressStart = getSectorAddress(16);
    int address = addressStart;
    stream->seekg(addressStart);
    for (int i = 0; i < maxOffset; i++) {
        c = readChar();
        if (c != 1) {
            address++;
            c = readChar();
        } else {
            int pos = stream->tellg();
            string str = readString(5);
            address = pos;
            stream->seekg(pos, ios::beg);
            if (str == "CD001") {
                int addr = stream->tellg();
                offset = pos - 1 - addressStart;
                return addr - 1;
            } else {
                address++;
                c = readChar();
            }
        }

    }
    return -1;
}

//*******************************
// Isodir::getEmptyDir
//*******************************
IsoDirectory Isodir::getEmptyDir()
{
    IsoDirectory dir;
    dir.systemName="UNKNOWN";
    dir.volumeName = "UNKNOWN";
    return dir;
}

//*******************************
// Isodir::readDir
//*******************************
void Isodir::readDir(vector<string> * data, unsigned int sector, int maxlevel, int level)
{
    if (level>=maxlevel)
    {
        return;
    }
    unsigned int currSector=sector;
    long currentPos = stream->tellg();
    long sectorAddr = getSectorAddress(sector);
    stream->seekg(sectorAddr, ios::beg);

    for (int i=0;i<200;i++){ // max 200 entries - it will be less and it will just break the loop
        long start = stream->tellg();
        if (start==-1) break;
        long readInSector = start-sectorAddr;
        if (readInSector>=2048)
        {
            currSector++;
            sectorAddr = getSectorAddress(currSector);
            stream->seekg(sectorAddr, ios::beg);
            continue;
        }
        long len = readChar();
        if (len==0)
        {
            currSector++;
            sectorAddr = getSectorAddress(currSector);
            stream->seekg(sectorAddr, ios::beg);
            if (readInSector<2048-62)
            {
                break;
            }
            continue;
        }

        int ealen = readChar();
        unsigned int loc = readDword();
        //stream->seekg(26, ios::cur);
        stream->seekg(4, ios::cur);
        stream->seekg(8, ios::cur);
        stream->seekg(7, ios::cur);
        int attr = readChar();
        stream->seekg(6, ios::cur);

        int fileNameLen = readChar();
        if (fileNameLen < 2) {
            stream->seekg(start + len);
            if (fileNameLen == 0) break;
            continue;
        }
        string fileName = readString(fileNameLen);
        data->push_back(removeVersion(fileName));
        if ((attr>>1) & 1)
        {
            readDir(data,loc, maxlevel,level+1);
        }
        stream->seekg(start + len);
    }

    stream->seekg(currentPos, ios::beg);
}

//*******************************
// Isodir::getDir
//*******************************
 IsoDirectory Isodir::getDir(string binPath,int maxlevel) {
    offset=0;
    stream = new ifstream(binPath,  ios::binary | ios::in);
    if (!stream->good()) {
        return getEmptyDir();
    }
     if (!stream->is_open()) {
         delete stream;
         return getEmptyDir();
     }
    stream->seekg(0);
    int pdvAddress = findPrimaryDescriptor(50);
    if (pdvAddress==-1)
    {
        stream->close();
        delete stream;
        return getEmptyDir();
    }
    int sectorAddr = getSectorAddress(16);
    stream->seekg(sectorAddr, ios::beg);
    stream->seekg(8, ios::cur);
    string system = readString(32);
    string volname = readString(32);
    stream->seekg(86, ios::cur);
    int sector = readDword();
    stream->seekg(getSectorAddress(sector), ios::beg);
    IsoDirectory result;
    result.systemName = trim(system);
    result.volumeName = trim(volname);
    readDir(&result.rootDir,sector,maxlevel,0);
    stream->close();

    if (result.rootDir.empty())
    {
        stream->close();
        delete stream;
        return getEmptyDir();
    }
     stream->close();
     delete stream;
    return result;
}