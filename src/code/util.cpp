#include "util.h"
#include "main.h"

#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <iomanip>

using namespace std;
#define FILE_BUFFER 524288

const char *Util::separator() {
#ifdef _WIN32
    return "\\";
#else
    return (char *) "/";
#endif
}

bool wayToSort(DirEntry i, DirEntry j) {
    string name1 = i.name;
    string name2 = j.name;
    name1 = lcase(name1);
    name2 = lcase(name2);
    return name1 < name2;
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

void Util::replaceAll(std::string &str, const std::string &from, const std::string &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
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

std::string Util::getWorkingPath() {
    char temp[2048];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

bool Util::isDirectory(string path)
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
            DirEntry obj(entry->d_name, entry->d_type);
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

bool Util::exists(const std::string &name) {
    fixPath(name);
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);

}

bool Util::createDir(const std::string name) {
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

bool Util::copy(string source, string dest) {
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
string Util::getAvailableSpace(){
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
}


string Util::floatToString(float f, int n){
    std::ostringstream stringStream;
    stringStream << std::fixed << std::setprecision(n) << f;
    return stringStream.str();
}

string Util::commaSep(string s, int pos) {
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

string Util::execUnixCommad(const char* cmd){
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(remove(result.begin(),result.end(),'\n'));
    return result;
}
