/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   util.h
 * Author: screemer
 *
 * Created on 11 grudnia 2018, 23:12
 */

#pragma once

#include <string>
#include <vector>
#include "main.h"

//******************
// DirEntry
//******************
class DirEntry {
public:
    std::string name;
    bool isDir;

    DirEntry(std::string _name, bool dir) : name(_name), isDir(dir) { };
    static bool sortByName(const DirEntry & i, const DirEntry & j) { return SortByCaseInsensitive(i.name, j.name); }
};

//******************
// Util
//******************
class Util {
public:
    static const char *separator();
    static std::string pathWithSeparatorAtEnd(const std::string& path);  // return the path with a separator at the end
    static std::string pathWithOutSeparatorAtEnd(const std::string& path);   // return the path without a separator at the end

    static std::string escape(std::string input);
    static std::string decode(std::string input);
    static void replaceAll(std::string &str, const std::string &from, const std::string &to);

    // File System operations
    static std::vector<DirEntry> dir(std::string path);   // returns directory contents including . and ..
    static std::vector<DirEntry> diru(std::string path);  // returns directory contents except . and ..
    static std::vector<DirEntry> diru_DirsOnly(std::string path);  // diru but only returns directories
    static std::vector<DirEntry> diru_FilesOnly(std::string path);  // diru but only returns files

    static bool copy(const std::string& source, const std::string& dest); // copies a file
    static bool exists(const std::string &name);
    static bool createDir(const std::string name);
    static int rmDir(std::string path);
    static std::string getWorkingPath();
    static bool isInteger(const char *input);
    static bool matchExtension(std::string path, std::string ext);  // this does a case insensitive compare
    static bool matchesLowercase(std::string first, std::string second);
    static std::string findFirstFile(std::string ext, std::string path);
    static bool isDirectory(const std::string& path);
    static std::vector<DirEntry> getFilesWithExtension(const std::string& path, const std::vector<DirEntry>& entries,
            const std::vector<std::string>& extensions);    // pass file extensions in lower case
    static std::string getFileNameFromPath(const std::string& path);

    static unsigned char readChar(std::ifstream * stream);
    static unsigned long  readDword(std::ifstream * stream);
    static std::string readString(int size, std::ifstream * stream);
    static std::string readString(std::ifstream * stream);
    static void skipZeros(std::ifstream * stream);
    static std::string commaSep(const std::string& input, int pos);
    static std::string getFileExtension(const std::string & fileName);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    static std::string ltrim(const std::string& s);
    static std::string rtrim(const std::string& s);
    static std::string trim(const std::string& s);
    static std::string getStringWithinChar(std::string s, char del);

    static std::string getAvailableSpace();
    static std::string floatToString(float f, int n);
    static std::string execUnixCommad(const char* cmd);
    static void execFork(const char* cmd,  std::vector<const char *> argvNew);

    static void powerOff();

    static std::vector<std::string> cueToBinList(std::string cueFile);
};

