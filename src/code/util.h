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

//******************
// Util
//******************
class Util {
public:
    static std::string escape(std::string input);
    static std::string decode(std::string input);
    static void replaceAll(std::string &str, const std::string &from, const std::string &to);

    static bool isInteger(const char *input);
    static bool compareCaseInsensitive(std::string first, std::string second);

    static unsigned char readChar(std::ifstream * stream);
    static unsigned long readDword(std::ifstream * stream);
    static std::string readString(int size, std::ifstream * stream);
    static std::string readString(std::ifstream * stream);
    static void skipZeros(std::ifstream * stream);
    static std::string commaSep(const std::string& input, int pos);
    static std::string ltrim(const std::string& s);
    static std::string rtrim(const std::string& s);
    static std::string trim(const std::string& s);
    static std::string getStringWithinChar(std::string s, char del);
    static void removeCharsFromString(std::string& str, std::string charsToRemove);
    static void removeCRLFFromString(std::string& str) { removeCharsFromString(str, "\r\n"); }

    static std::string getAvailableSpace();
    static std::string floatToString(float f, int n);
    static std::string execUnixCommand(const char* cmd);
    static void execFork(const char* cmd,  std::vector<const char *> argvNew);

    static void powerOff();

    static void cleanPublisherString(std::string & pub);  // remove any trailing "." or spaces
    static void dumpMemory(const char *p, int count);
};

