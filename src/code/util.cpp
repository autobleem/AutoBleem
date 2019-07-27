#include "util.h"
#include "main.h"

#include <fstream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <iomanip>
#include <string.h>
#include <libgen.h>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include "main.h"

using namespace std;

#define FILE_BUFFER 524288

//*******************************
// DirEntry::separator
//*******************************
const char *DirEntry::separator() {
#ifdef _WIN32
    return "\\";
#else
    return (char *) "/";
#endif
}

//*******************************
// DirEntry::pathWithSeparatorAtEnd
//*******************************
// return the path with a separator at the end
string DirEntry::pathWithSeparatorAtEnd(const string& path)
{
    string ret = path;
    if (ret.length() > 0)
    {
        char lastChar = ret[ret.length()-1];
        if (lastChar != separator()[0])
            ret += separator(); // add slash at end
    }

    return ret;
}

//*******************************
// DirEntry::pathWithOutSeparatorAtEnd
//*******************************
// return the path without a separator at the end
string DirEntry::pathWithOutSeparatorAtEnd(const string& path)
{
    string ret = path;
    if (ret.length() > 0)
    {
        char & lastChar = ret[ret.length()-1];
        if (lastChar == separator()[0])
            lastChar = 0;   // remove slash at end
    }

    return ret;
}

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
// DirEntry::getFileNameFromPath
//*******************************
string DirEntry::getFileNameFromPath(const string& path)
{
    string result = "";
    char *cstr = new char[path.length() + 1];
    strcpy(cstr, path.c_str());
    char * base = basename(cstr);
    result += base;
    delete [] cstr;
    return result;
}

//*******************************
// fixPath
//*******************************
string fixPath(string path)
{
    trim(path);
    if (path.back() == DirEntry::separator()[0])
    {
        path = path.substr(0,path.size()-1);
    }
    return path;
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
// DirEntry::getWorkingPath
//*******************************
string DirEntry::getWorkingPath() {
    char temp[2048];
    return (getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
}

//*******************************
// DirEntry::isDirectory
//*******************************
bool DirEntry::isDirectory(const string& path)
{
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

//*******************************
// DirEntry::dir
//*******************************
DirEntries DirEntry::dir(string path) {
    fixPath(path);
    DirEntries result;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent *entry = readdir(dir);
        while (entry != NULL) {
            DirEntry obj(entry->d_name, entry->d_type);
            result.push_back(obj);
            entry = readdir(dir);
        }

        closedir(dir);
    }
    sort(result.begin(), result.end(), DirEntry::sortDirEntryByName);
    return result;
}

//*******************************
// DirEntry::diru
//*******************************
DirEntries DirEntry::diru(string path) {
    fixPath(path);
    DirEntries result;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent *entry = readdir(dir);
        while (entry != NULL) {
            DirEntry obj(entry->d_name, isDirectory(path + entry->d_name));
            if (entry->d_name[0] != '.') {
                result.push_back(obj);
            }
            entry = readdir(dir);
        }

        closedir(dir);
    }
    sort(result.begin(), result.end(), DirEntry::sortDirEntryByName);
    return result;
}

//*******************************
// DirEntry::diru_DirsOnly
//*******************************
DirEntries DirEntry::diru_DirsOnly(string path) {
    auto temp = diru(path); // get all dirs and files
    DirEntries ret;
    copy_if(begin(temp), end(temp), back_inserter(ret), [](const DirEntry & dir) { return dir.isDir; });    // copy only dirs

    return ret; // return only the dirs
}

//*******************************
// DirEntry::diru_FilesOnly
//*******************************
DirEntries DirEntry::diru_FilesOnly(string path) {
    auto temp = diru(path); // get all dirs and files
    DirEntries ret;
    copy_if(begin(temp), end(temp), back_inserter(ret), [](const DirEntry & dir) { return !dir.isDir; });   //copy only files

    return ret; // return only the files
}

//*******************************
// DirEntry::exists
//*******************************
bool DirEntry::exists(const string &name) {
    fixPath(name);
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);

}

//*******************************
// DirEntry::createDir
//*******************************
bool DirEntry::createDir(const string name) {
    fixPath(name);
    const int dir_err = mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return (-1 != dir_err);
}

//*******************************
// DirEntry::rmDir
//*******************************
int DirEntry::rmDir(string path) {
    fixPath(path);
    DIR *d = opendir(path.c_str());
    size_t path_len = path.size();
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;

        while (!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = new char[len];

            if (buf) {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path.c_str(), p->d_name);

                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        r2 = rmDir(buf);
                    } else {
                        r2 = unlink(buf);
                    }
                }

                delete (buf);
            }

            r = r2;
        }

        closedir(d);
    }

    if (!r) {
        r = rmdir(path.c_str());
    }

    return r;

}

//*******************************
// DirEntry::copy
//*******************************
bool DirEntry::copy(const string& source, const string& dest) {
    ifstream infile;
    ofstream outfile;

    char *buffer;
    buffer = new char[FILE_BUFFER];

    infile.open(source, ios::binary);
    outfile.open(dest, ios::binary);

    if (!infile.good()) return false;
    if (!outfile.good()) return false;

    while (true) {
        int read = infile.readsome(buffer, FILE_BUFFER);
        if (read == 0) break;
        outfile.write(buffer, read);

    }
    infile.close();
    outfile.flush();
    outfile.close();
    delete buffer;

    return true;
}

//*******************************
// DirEntry::findFirstFile
//*******************************
string DirEntry::findFirstFile(string ext, string path) {
    fixPath(path);
    DirEntries entries = diru(path);
    for (DirEntry entry:entries) {
        if (matchExtension(entry.name, ext)) {
            return entry.name;
        }
    }
    return "";
}

//*******************************
// DirEntry::removeDotFromExtension
//*******************************
// if it begins with a ".", remove it
string DirEntry::removeDotFromExtension(const std::string & ext) {
    if (ext.c_str()[0] == '.')
        return ext.c_str() + 1;
    else
        return ext;
}

//*******************************
// DirEntry::addDotToExtension
//*******************************
// if it doesn't start with a ".", add it to the front
string DirEntry::addDotToExtension(const std::string & ext) {
    if (ext.c_str()[0] != '.')
        return string(".") + ext;
    else
        return ext;
}

//*******************************
// DirEntry::matchExtension
//*******************************
bool DirEntry::matchExtension(string path, string ext) {
    fixPath(path);
    if (path.length() >= 4) {
        string fileExt = path.substr(path.length() - 4, path.length()); // file extension includes the "."
        if (fileExt[0] != '.') {
            return false;
        } else {
            auto temp = addDotToExtension(ext);
            return lcase(fileExt) == lcase(temp);
        }
    } else {
        return false;
    };

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
// Util::matchesLowercase
//*******************************
bool Util::matchesLowercase(string first, string second) {
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
string Util::execUnixCommad(const char* cmd){
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
// DirEntry::getFileExtension
//*******************************
// Return the extension of a filename without the "."
string DirEntry::getFileExtension(const string & fileName) {
    size_t i = fileName.rfind('.', fileName.length());
    if (i != string::npos) {
        return(fileName.substr(i+1, fileName.length() - i));
    }
    return "";
}

//*******************************
// DirEntry::getFileNameWithoutExtension
//*******************************
// Return the name of a file without extension
string DirEntry::getFileNameWithoutExtension(const string& filename) {
    size_t indexBeforeDot = filename.find_last_of(".");
    return filename.substr(0, indexBeforeDot);
}

//*******************************
// DirEntry::cueToBinList
//*******************************
// Return the bin list declared in a cue file
vector<string> DirEntry::cueToBinList(string cueFile) {
    vector<string> binList;
    FILE *fp;
    char *cline = NULL;
    string line;
    size_t length = 0;
    ssize_t read;

    //Opening file
    fp = fopen(cueFile.c_str(),"r");
    if(fp == NULL){
        printf("Error opening cue file");
        return binList;
    }

    //Reading line by line
    while((read = getline(&cline, &length, fp)) != -1){
        line = cline;
        line = trim(line);
        if(line.substr(0,4) == "FILE"){
            binList.push_back(Util::getStringWithinChar(line,'"').c_str());
        }
    }

    //Closing file pointer
    fclose(fp);

    //Freeing line pointer
    if(cline){
        free(cline);
    }

    return binList;
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
// DirEntry::getFilesWithExtension
//*******************************
DirEntries DirEntry::getFilesWithExtension(const string& path, const DirEntries & entries, const vector<string>& extensions) {
    DirEntries fileList;
    string fileExt;
    for (const auto & entry : entries){
        if(isDirectory(path + "/" + entry.name))
            continue;
        // make it case insensitive compare (find .bin and .BIN)
        fileExt = ReturnLowerCase(getFileExtension(entry.name));
        if(find(extensions.begin(),extensions.end(),fileExt) != extensions.end()){
            fileList.push_back(entry);
        }
    }
    return fileList;
}

//*******************************
// Util::cleanPublisherString
//*******************************// remove any trailing "." or space or " ."
void Util::cleanPublisherString(std::string & pub)
{
    if (pub.size() > 0 && pub[pub.size()-1] == '.')
        pub.pop_back();
    if (pub.size() > 0 && pub[pub.size()-1] == ' ')
        pub.pop_back();
}