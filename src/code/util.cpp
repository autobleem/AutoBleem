#include "util.h"

#include <fstream>
#include <sys/stat.h>
#include <algorithm>

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
    return i.name < j.name;
}

std::string Util::getWorkingPath() {
    char temp[2048];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

vector<DirEntry> Util::dir(string path) {
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

bool Util::exists(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

bool Util::createDir(const std::string name) {
    const int dir_err = mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return (-1 != dir_err);

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


bool Util::is_integer_name(const char *input) {
    size_t ln = strlen(input);
    for (size_t i = 0; i < ln; i++) {
        if (!isdigit(input[i])) {
            return false;
        }

    }
    return true;
}



int Util::strcicmp(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower((unsigned char) *a) - tolower((unsigned char) *b);
        if (d != 0 || !*a)
            return d;
    }
}

