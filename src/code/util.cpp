#include "util.h"
#include "main.h"

#include <fstream>
#include <sys/wait.h>
#include <unistd.h>
#include <iomanip>
#include <string.h>
#include <sstream>
#include <iostream>
#include "main.h"
#include <stdio.h>

using namespace std;

//*******************************
// Util::powerOff
//*******************************
void Util::powerOff()
{
#if defined(__x86_64__) || defined(_M_X64)
    exit(0);
#else
    Util::execUnixCommad("shutdown -h now");
    exit(0);
#endif
}

//*******************************
// Util::replaceAll
//*******************************
void Util::replaceAll(string &str, const string &from, const string &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

//*******************************
// Util::escape
//*******************************
string Util::escape(string input) {
    replaceAll(input, "|", "||");
    replaceAll(input, ",", "|@");
    return input;
}

//*******************************
// Util::decode
//*******************************
string Util::decode(string input) {
    replaceAll(input, "|@", ",");
    replaceAll(input, "||", "|");
    return input;
}

//*******************************
// Util::isInteger
//*******************************
bool Util::isInteger(const char *input) {
    size_t ln = strlen(input);
    for (size_t i = 0; i < ln; i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

//*******************************
// Util::compareCaseInsensitive
//*******************************
bool Util::compareCaseInsensitive(string first, string second) {
    return lcase(first) == lcase(second);
}


//*******************************
// Util::readChar
//*******************************
unsigned char Util::readChar(ifstream *stream) {
    unsigned char c;
    stream->read((char *) &c, 1);
    return c;
}

//*******************************
// Util::readString
//*******************************
string Util::readString(int size, ifstream *stream) {
    char str[size + 1];
    str[size] = 0;
    stream->read(str, size);
    return str;
}

//*******************************
// Util::skipZeros
//*******************************
void Util::skipZeros(ifstream *stream) {
    char c = readChar(stream);
    while (c == 00) {
        c = readChar(stream);
    }
    stream->seekg(-1, ios::cur);
}

//*******************************
// Util::readString
//*******************************
string Util::readString(ifstream *stream) {
    string str;
    char c = readChar(stream);
    while (c != 00) {
        str = str + c;
        c = readChar(stream);
    }
    return str;
}

//*******************************
// Util::readDword
//*******************************
unsigned long Util::readDword(ifstream *stream) {
    unsigned long res = 0;
    unsigned long c;
    c = readChar(stream);
    res += c;
    c = readChar(stream);
    res += c << (1 * 8);
    c = readChar(stream);
    res += c << (2 * 8);
    c = readChar(stream);
    res += c << (3 * 8);
    return res;
}

//*******************************
// Util::getAvailableSpace
//*******************************
/*
 * Return the available space of a usb device
 */
string Util::getAvailableSpace(){
#if defined(__x86_64__) || defined(_M_X64)
    return "x86 - does not care about free space - Does not work on mac";
    #else
    string str;
    int gb = 1024 * 1024;
    string dfResult;
    float freeSpace;
    float totalSpace;
    int freeSpacePerc;
    freeSpace = ((float)(stoi(execUnixCommad("df | grep \"media\" | head -1 | awk '{print $4}'"))))/gb;
    totalSpace = ((float)(stoi(execUnixCommad("df | grep \"media\" | head -1 | awk '{print $2}'"))))/gb;
    freeSpacePerc = (freeSpace / totalSpace) * 100;
    str = floatToString(freeSpace, 2) + " GB / " + floatToString(totalSpace,2)+ " GB (" + to_string(freeSpacePerc)+"%)";
    return str;
#endif
}

//*******************************
// Util::floatToString
//*******************************
/*
 * Convert a float f to a string with precision of n
 */
string Util::floatToString(float f, int n){
    ostringstream stringStream;
    stringStream << fixed << setprecision(n) << f;
    return stringStream.str();
}

//*******************************
// Util::commaSep
//*******************************
string Util::commaSep(const string& s, int pos) {
    vector<string> v;
    v.clear();
    char c = ',';
    int i = 0;
    int j = s.find(c);

    while (j >= 0) {
        v.push_back(s.substr(i, j - i));
        i = ++j;
        j = s.find(c, j);

        if (j < 0) {
            v.push_back(s.substr(i, s.length()));
        }
    }
    if (pos<v.size())
    {
        return v[pos];
    }
    return "";
}

//*******************************
// Util::execUnixCommad
//*******************************
/*
 * Execute a shell command and return output
 */
string Util::execUnixCommand(const char* cmd){
    array<char, 128> buffer;
    string result;
    cout << "Exec:" << cmd << endl;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    if (!result.empty()) {
        result.erase(remove(result.begin(), result.end(), '\n'));
    }
    return result;
}

//*******************************
// Util::execFork
//*******************************
void Util::execFork(const char *cmd,  vector<const char *> argvNew)
{
    string link = cmd;

    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }

    waitpid(pid, NULL, 0);
}

//*******************************
// Util::ltrim
//*******************************
// Left trimming
string Util::ltrim(const string& s){
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == string::npos) ? "" : s.substr(start);
}

//*******************************
// Util::rtrim
//*******************************
// Right trimming
string Util::rtrim(const string& s){
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}

//*******************************
// Util::trim
//*******************************
// Trimming both left and right
string Util::trim(const string &s) {
    return rtrim(ltrim(s));
}

//*******************************
// Util::getStringWithinChar
//*******************************
/*
 * Return a char between separator like :
 * Super "GAMENAME" baby
 * Will return
 * GAMENAME
 */
string Util::getStringWithinChar(string s, char del) {
    int first = s.find(del);
    int last = s.find_last_of(del);
    return s.substr(first+1, last-first-1);
}

//*******************************
// Util::cleanPublisherString
// remove any trailing "." or space or " ."
//*******************************
void Util::cleanPublisherString(std::string & pub)
{
    if (pub.size() > 0 && pub.back() == '.')
        pub.pop_back();
    if (pub.size() > 0 && pub.back() == ' ')
        pub.pop_back();
}

//*******************************
// Util::dumpMemory
//*******************************
void Util::dumpMemory(const  char *p, int count) {
    for (int i=0; i < count; ++i) {
        printf("%x, ", (unsigned int) *p++);
        if (i %16 == 15 || i == count-1)
            cout << endl;
    }
}