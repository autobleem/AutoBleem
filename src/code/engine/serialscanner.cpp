//
// Created by screemer on 2/3/19.
//

#include "../main.h"
#include "serialscanner.h"
#include "isodir.h"
#include <sstream>
#include "../util.h"

using namespace std;

// The SerialScanner class reads the serial number in a CDROM BIN file which is an ISO 9660 image of a CDROM.
// https://en.wikipedia.org/wiki/ISO_9660

//*******************************
// SerialScanner::fixSerial
//*******************************
string SerialScanner::fixSerial(string serial) {
    replace(serial.begin(), serial.end(), '_', '-');
    serial.erase(remove(serial.begin(), serial.end(), '.'), serial.end());
    string fixed = "";
    stringstream alpha;
    stringstream digits;
    bool digitsProcessing = false;
    for (int i = 0; i < serial.size(); i++) {
        int maxchars = serial[0] == 'L' ? 3 : 4;
        if (!isdigit(serial[i])) {

            if (digitsProcessing) continue;
            if (serial[i] == '-') continue;
            if (alpha.str().length() < maxchars) {
                alpha << serial[i];
            }
        } else {
            digitsProcessing = true;
            digits << serial[i];
        }
    }
    return alpha.str() + "-" + digits.str();
}

//*******************************
// SerialScanner::scanSerial
//*******************************
string SerialScanner::scanSerial(int imageType, string path, string firstBinPath)
{
    string serial = scanSerialInternal(imageType,path,firstBinPath);
    cout <<serial<<endl;
    if (serial.empty())
    {
        serial = workarounds(imageType,path,firstBinPath);
    }
    return serial;
}

//*******************************
// SerialScanner::scanSerialInternal
//*******************************
string SerialScanner::scanSerialInternal(int imageType, string path, string firstBinPath) {
    if (imageType == IMAGE_PBP) {
        string destinationDir = path ;
        string pbpFileName = Util::findFirstFile(EXT_PBP, destinationDir);
        if (pbpFileName != "") {
            ifstream is;
            is.open(destinationDir + pbpFileName);

            long magic = Util::readDword(&is);
            if (magic != 0x50425000) {
                return "";
            }
            long second = Util::readDword(&is);
            if (second != 0x10000) {
                return "";
            }
            long sfoStart = Util::readDword(&is);

            is.seekg(sfoStart, ios::beg);

            unsigned int signature = Util::readDword(&is);
            if (signature != 1179865088) {
                return "";
            }
            unsigned int version = Util::readDword(&is);;
            unsigned int fields_table_offs = Util::readDword(&is);
            unsigned int values_table_offs = Util::readDword(&is);
            int nitems = Util::readDword(&is);

            vector<string> fields;
            vector<string> values;
            fields.clear();
            values.clear();
            is.seekg(sfoStart, ios::beg);
            is.seekg(fields_table_offs, ios::cur);
            for (int i = 0; i < nitems; i++) {
                string fieldName = Util::readString(&is);
                Util::skipZeros(&is);
                fields.push_back(fieldName);
            }

            is.seekg(sfoStart, ios::beg);
            is.seekg(values_table_offs, ios::cur);
            for (int i = 0; i < nitems; i++) {
                string valueName = Util::readString(&is);
                Util::skipZeros(&is);
                values.push_back(valueName);
            }

            is.close();

            for (int i = 0; i < nitems; i++) {
                if (fields[i] == "DISC_ID") {
                    string potentialSerial = values[i];
                    return fixSerial(potentialSerial);
                }
            }
        }
    }
    if (imageType == IMAGE_CUE_BIN) {
        string prefixes[] = {
                "CPCS", "ESPM", "HPS", "LPS", "LSP", "SCAJ", "SCED", "SCES", "SCPS", "SCUS", "SIPS", "SLES", "SLKA",
                "SLPM", "SLPS", "SLUS"};
        if (firstBinPath == "") {
            return ""; // not at this stage
        }

        for (int level = 1; level < 4; level++) {
            Isodir *dirLoader = new Isodir();
            IsoDirectory dir = dirLoader->getDir(firstBinPath, level);
            delete dirLoader;
            string serialFound = "";
            if (!dir.rootDir.empty()) {
                for (string entry:dir.rootDir) {
                 //   cout << entry << endl;
                    string potentialSerial = fixSerial(entry);
                    for (string prefix:prefixes) {
                        int pos = potentialSerial.find(prefix.c_str(), 0);
                        if (pos == 0) {
                            serialFound = potentialSerial;
                            cout << "Serial number: " << serialFound << endl;
                            return serialFound;
                        }
                    }
                }
                string volume = fixSerial(dir.volumeName);
                for (string prefix:prefixes) {
                    int pos = volume.find(prefix.c_str(), 0);
                    if (pos == 0) {
                        serialFound = volume;
                        cout << "Serial number: " << serialFound << endl;
                        return serialFound;
                    }
                }

            } else {
                return "";
            }
        }
    }
    return "";
}

//*******************************
// SerialScanner::workarounds
//*******************************
string SerialScanner::workarounds(int imageType, string path, string firstBinPath)
{
    string fileToScan = "";
    if (imageType == IMAGE_CUE_BIN)
    {
        fileToScan = firstBinPath;
    }
    if (imageType == IMAGE_PBP)
    {
        fileToScan = Util::findFirstFile(EXT_PBP, path);
    }

    // BH2 - Resident Evil 1.5
    if (fileToScan.find("BH2")!=string::npos)
    {
        return serialByMd5(fileToScan);
    }
    return "";
}


//*******************************
// SerialScanner::serialByMd5
//*******************************
string SerialScanner::serialByMd5(string scanFile)
{
    string head=Util::execUnixCommad(("head -c 1M \""+scanFile+"\" | md5sum | awk '{print $1}'").c_str());
    string tail=Util::execUnixCommad(("tail -c 1M \""+scanFile+"\" | md5sum | awk '{print $1}'").c_str());

    return head+tail;
}