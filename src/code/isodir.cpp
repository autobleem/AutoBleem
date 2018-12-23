//
// Created by screemer on 23.12.18.
//

#include "isodir.h"

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

int Isodir::getSectorAddress(int sector) {
    return sector * SECTOR_SIZE + offset;
}

char Isodir::readChar() {
    char c;
    stream.read(&c, 1);
    return c;
}

string Isodir::readString(int size) {
    char str[size + 1];
    str[size] = 0;
    stream.read(str, size);
    return str;
}

int Isodir::readDword() {
    int res = 0;
    unsigned int c;
    c = readChar();
    res += c;
    c = readChar();
    res += c << 1 * 8;
    c = readChar();
    res += c << 2 * 8;
    c = readChar();
    res += c << 3 * 8;
    return res;

}

int Isodir::findPrimaryDescriptor(int maxOffset) {
    int addressStart = getSectorAddress(16);
    int address = addressStart;
    stream.seekg(addressStart, ios::beg);
    for (int i = 0; i < maxOffset; i++) {
        char c = readChar();
        if (c != 1) {
            address++;
            c = readChar();
        } else {
            int pos = stream.tellg();
            string str = readString(5);
            address = pos;
            stream.seekg(pos, ios::beg);
            if (str == "CD001") {
                int addr = stream.tellg();
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

IsoDirectory Isodir::getEmptyDir()
{
    IsoDirectory dir;
    dir.systemName="UNKNOWN";
    dir.volumeName = "UNKNOWN";
    return dir;
}


 IsoDirectory Isodir::getDir(string binPath) {
    stream.open(binPath, ios::binary);
    if (!stream.good()) {
        return getEmptyDir();
    }
    int pdvAddress = findPrimaryDescriptor(50);
    if (pdvAddress==-1)
    {
        return getEmptyDir();
    }
    int sectorAddr = getSectorAddress(16);
    stream.seekg(sectorAddr, ios::beg);
    stream.seekg(8, ios::cur);
    string system = readString(32);
    string volname = readString(32);
    stream.seekg(86, ios::cur);
    int sector = readDword();
    stream.seekg(getSectorAddress(sector), ios::beg);
    IsoDirectory result;
    result.systemName = trim(system);
    result.volumeName = trim(volname);
    for (int i = 0; i < 300; i++) { // max 300 entries - it will be less and it will just break the loop
        int start = stream.tellg();
        int len = readChar();
        stream.seekg(31, ios::cur);

        int fileNameLen = readChar();
        if (fileNameLen < 2) {
            stream.seekg(start + len);
            if (fileNameLen == 0) break;
            continue;
        }
        string fileName = readString(fileNameLen);
        result.rootDir.push_back(removeVersion(fileName));
        stream.seekg(start + len);
    }
    if (result.rootDir.empty())
    {
        return getEmptyDir();
    }
    return result;
}