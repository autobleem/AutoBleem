#include "util.h"
#include "main.h"

#include <fstream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <iomanip>
#include <libgen.h>

using namespace std;
#define FILE_BUFFER 524288

const char *Util::separator() {
#ifdef _WIN32
    return "\\";
#else
    return (char *) "/";
#endif
}

// return the path with a separator at the end
string Util::pathWithSeparatorAtEnd(const string& path)
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

// return the path without a separator at the end
string Util::pathWithOutSeparatorAtEnd(const string& path)
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

bool wayToSort(const DirEntry & i, const DirEntry & j) { return SortByCaseInsensitive(i.name, j.name); }

void Util::powerOff()
{
#if defined(__x86_64__) || defined(_M_X64)
    exit(0);
#else
    Util::execUnixCommad("shutdown -h now");
    exit(0);
#endif


}

string Util::getFileNameFromPath(const string& path)
{
    string result = "";
    char *cstr = new char[path.length() + 1];
    strcpy(cstr, path.c_str());
    char * base = basename(cstr);
    result += base;
    delete [] cstr;
    return result;
}

string fixPath(string path)
{
    trim(path);
    if (path.back()==Util::separator()[0])
    {
        path = path.substr(0,path.size()-1);
    }
    return path;
}

void Util::replaceAll(string &str, const string &from, const string &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string Util::escape(string input) {
    replaceAll(input, "|", "||");
    replaceAll(input, ",", "|@");
    return input;
}

string Util::decode(string input) {
    replaceAll(input, "|@", ",");
    replaceAll(input, "||", "|");
    return input;
}

string Util::getWorkingPath() {
    char temp[2048];
    return (getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
}

bool Util::isDirectory(const string& path)
{
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

vector<DirEntry> Util::dir(string path) {
    fixPath(path);
    vector<DirEntry> result;
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
    sort(result.begin(), result.end(), wayToSort);
    return result;
}

vector<DirEntry> Util::diru(string path) {
    fixPath(path);
    vector<DirEntry> result;
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
    sort(result.begin(), result.end(), wayToSort);
    return result;
}

vector<DirEntry> Util::diru_DirsOnly(string path) {
    auto temp = diru(path); // get all dirs and files
    vector<DirEntry> ret;
    copy_if(begin(temp), end(temp), back_inserter(ret), [](const DirEntry & dir) { return dir.isDir; });    // copy only dirs

    return ret; // return only the dirs
}

vector<DirEntry> Util::diru_FilesOnly(string path) {
    auto temp = diru(path); // get all dirs and files
    vector<DirEntry> ret;
    copy_if(begin(temp), end(temp), back_inserter(ret), [](const DirEntry & dir) { return !dir.isDir; });   //copy only files

    return ret; // return only the files
}

bool Util::exists(const string &name) {
    fixPath(name);
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);

}

bool Util::createDir(const string name) {
    fixPath(name);
    const int dir_err = mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return (-1 != dir_err);
}

int Util::rmDir(string path) {
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

bool Util::copy(const string& source, const string& dest) {
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

string Util::findFirstFile(string ext, string path) {
    fixPath(path);
    vector<DirEntry> entries = diru(path);
    for (DirEntry entry:entries) {
        if (matchExtension(entry.name, ext)) {
            return entry.name;
        }
    }
    return "";
}

bool Util::matchExtension(string path, string ext) {
    fixPath(path);
    if (path.length() >= 4) {
        string fileExt = path.substr(path.length() - 4, path.length());
        if (fileExt[0] != '.') {
            return false;
        } else {
            return lcase(fileExt) == lcase(ext);

        }
    } else {
        return false;
    };

}


bool Util::isInteger(const char *input) {
    size_t ln = strlen(input);
    for (size_t i = 0; i < ln; i++) {
        if (!isdigit(input[i])) {
            return false;
        }

    }
    return true;
}

bool Util::matchesLowercase(string first, string second) {
    return lcase(first) == lcase(second);
}


unsigned char Util::readChar(ifstream *stream) {
    unsigned char c;
    stream->read((char *) &c, 1);
    return c;
}

string Util::readString(int size, ifstream *stream) {
    char str[size + 1];
    str[size] = 0;
    stream->read(str, size);
    return str;
}

void Util::skipZeros(ifstream *stream) {
    char c = readChar(stream);
    while (c == 00) {
        c = readChar(stream);
    }
    stream->seekg(-1, ios::cur);
}

string Util::readString(ifstream *stream) {
    string str;
    char c = readChar(stream);
    while (c != 00) {
        str = str + c;
        c = readChar(stream);
    }
    return str;
}

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

/*
 * Convert a float f to a string with precision of n
 */
string Util::floatToString(float f, int n){
    ostringstream stringStream;
    stringStream << fixed << setprecision(n) << f;
    return stringStream.str();
}

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

void Util::execFork(const char *cmd,  vector<const char *> argvNew)
{
    string link = cmd;

    int pid = fork();
    if (!pid) {
        execvp(link.c_str(), (char **) argvNew.data());
    }

    waitpid(pid, NULL, 0);
}

/*
 * Return the extension of a filename out of string
 * myfile.txt will return txt
 */
string Util::getFileExtension(const string & fileName) {
    size_t i = fileName.rfind('.', fileName.length());
    if (i != string::npos) {
        return(fileName.substr(i+1, fileName.length() - i));
    }
    return "";
}

/*
 * Return the name of a file without extension
 */
string Util::getFileNameWithoutExtension(const string& filename) {
    size_t indexBeforeDot = filename.find_last_of(".");
    return filename.substr(0, indexBeforeDot);
}

/*
 * Return the bin list declared in a cue file
 */
vector<string> Util::cueToBinList(string cueFile) {
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
            binList.push_back(getStringWithinChar(line,'"').c_str());
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

/*
 * Left trimming
 */
string Util::ltrim(const string& s){
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == string::npos) ? "" : s.substr(start);
}

/*
 * Right trimming
 */
string Util::rtrim(const string& s){
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}

/*
 * Trimming both left and right
 */
string Util::trim(const string &s) {
    return rtrim(ltrim(s));
}

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

vector<DirEntry> Util::getFilesWithExtension(const string& path, const vector<DirEntry>& entries, const vector<string>& extensions) {
    vector<DirEntry> fileList;
    string fileExt;
    for (auto const &entry : entries){
        if(Util::isDirectory(path + "/" + entry.name)) continue;
        fileExt = Util::getFileExtension(entry.name);
        if(find(extensions.begin(),extensions.end(),fileExt) != extensions.end()){
            fileList.push_back(entry);
        }
    }
    return fileList;
}