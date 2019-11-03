#include "util.h"
#include "DirEntry.h"
#include "main.h"

#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "main.h"
#include <dirent.h>
#include <libgen.h>
#include <iostream>
#include <algorithm>

using namespace std;

// 0.5MB
#define FILE_BUFFER_SIZE 524288

//*******************************
// append separator helper function
//*******************************
// to use "operator +" below, "path + sep" will append the separator only if it's not already on the end of path
std::string operator + (const std::string &leftside, Sep) {
    string ret = leftside;
    ret += sep;

    return ret;
}

//*******************************
// append separator helper function
//*******************************
// to use "operator +" below, "path + sep" will append the separator only if it's not already on the end of path
void operator += (std::string &leftside, Sep) {
    if (leftside.size() > 0)
    {
        char lastChar = leftside.back();
        if (lastChar != separator)
            leftside += separator; // add slash at end
    }
}

//*******************************
// DirEntry::isPBPFile
//*******************************
bool DirEntry::isPBPFile(std::string path) {
    if (path.length() < 4) return false;
    string last_four = path.substr(path.length() - 4);
    lcase(last_four);
    return last_four == ".pbp";
}

void DirEntry::generateM3UForDirectory(std::string path, std::string basename) {
    if (DirEntry::isPBPFile(basename)) {
        basename = basename.substr(basename.length() - 4);
    }
    cout << basename << endl;
    vector<string> files;
    DirEntries filesInPath = DirEntry::diru_FilesOnly(path);
    for (const DirEntry &entry:filesInPath) {
        string ext = DirEntry::getFileExtension(entry.name);
        if (Util::compareCaseInsensitive(ext, "pbp")
            || Util::compareCaseInsensitive(ext, "cue"))
            files.push_back(entry.name);
    }
    string m3uName = DirEntry::fixPath(path) + sep + basename + ".m3u";
    if (files.size() > 1) {
        ofstream os(m3uName);
        for (const string &file:files) {
            os   << file << endl;
        }
        os.close();
    }
}

//*******************************
// DirEntry::fixPath
// removes leading and trailing spaces and removes any '/' from the end
//*******************************
string DirEntry::fixPath(string path)
{
    trim(path);
    if (path.size() > 0 && path.back() == separator)
        path.pop_back();

    return path;
}

//*******************************
// DirEntry::removeSeparatorFromEndOfPath
//*******************************
// return the path without a separator at the end
string DirEntry::removeSeparatorFromEndOfPath(const string& path)
{
    string ret = path;
    if (ret.length() > 0) {
        char & lastChar = ret.back();
        if (lastChar == separator)
            ret.pop_back();     // remove slash at end
    }

    return ret;
}

//*******************************
// DirEntry::removeGamesPathFromFrontOfPath
//*******************************
string DirEntry::removeGamesPathFromFrontOfPath(const std::string& path) {
    string gamesDir = Env::getPathToGamesDir() + sep;
    int len = gamesDir.size();
    if (path.compare(0, len, gamesDir) == 0)
        return string(path).erase(0, len);
    else
        return path;
}

//*******************************
// DirEntry::getFileNameFromPath
//*******************************
string DirEntry::getFileNameFromPath(const string &path) {
    string result = "";
    char *cstr = new char[path.length() + 1];
    strcpy(cstr, path.c_str());
    char *base = basename(cstr);
    result += base;
    delete[] cstr;
    return result;
}

//*******************************
// DirEntry::getDirNameFromPath
//*******************************
string DirEntry::getDirNameFromPath(const string& path)
{
    string result = "";
    char *cstr = new char[path.length() + 1];
    strcpy(cstr, path.c_str());
    char * base = dirname(cstr);
    result += base;
    delete [] cstr;
    return result;
}

//*******************************
// DirEntry::isDirectory
//*******************************
bool DirEntry::isDirectory(const string &path) {
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

//*******************************
// DirEntry::replaceTheseCharsWithThisChar
// replaces all the chars of a selection of chars with a single replacement char
// returns a string with those chars replaced
//*******************************
string DirEntry::replaceTheseCharsWithThisChar(string str, const string& charsToReplace, char replacementChar) {
    auto isBadChar = [&](char c) {
        return charsToReplace.find(c) != string::npos; // return if the char is a bad char
    };

    replace_if(begin(str), end(str), isBadChar, replacementChar);

    return str;
}

//*******************************
// DirEntry::fixCommaInDirName
// returns true if dir entry modified
//*******************************
bool DirEntry::fixCommaInDirOrFileName(const std::string &path, DirEntry *entry) {
    // fix for comma in dirname
    if (entry->name.find(",") != string::npos) {
        string newName = entry->name;
        Util::replaceAll(newName, ",", "-");
        rename((path + sep + entry->name).c_str(), (path + sep + newName).c_str());
        entry->name = newName;
        return true;
    } else
        return false;
}

//*******************************
// DirEntry::dir
//*******************************
DirEntries DirEntry::dir(string path) {
    path = fixPath(removeSeparatorFromEndOfPath(path));
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
    path = fixPath(removeSeparatorFromEndOfPath(path));
    DirEntries result;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent *entry = readdir(dir);
        while (entry != NULL) {
            DirEntry obj(entry->d_name, isDirectory(path + sep + entry->d_name));
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
    copy_if(begin(temp), end(temp), back_inserter(ret),
            [](const DirEntry &dir) { return dir.isDir; });    // copy only dirs

    return ret; // return only the dirs
}

//*******************************
// DirEntry::diru_FilesOnly
//*******************************
DirEntries DirEntry::diru_FilesOnly(string path) {
    auto temp = diru(path); // get all dirs and files
    DirEntries ret;
    copy_if(begin(temp), end(temp), back_inserter(ret),
            [](const DirEntry &dir) { return !dir.isDir; });   //copy only files

    return ret; // return only the files
}

//*******************************
// DirEntry::exists
//*******************************
bool DirEntry::exists(const string &_name) {
    auto name = _name;
    fixPath(name);
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

//*******************************
// DirEntry::createDir
//*******************************
bool DirEntry::createDir(const string &_name) {
    auto name = fixPath(_name);
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
// DirEntry::removeDirAndContents
//*******************************
bool  DirEntry::removeDirAndContents(const std::string path) {
    // remove the files in the dir
    auto files = diru_FilesOnly(path);
    for (auto & file : files)
        { remove((path + sep + file.name).c_str()); }

    // recursively remove the subdirs
    auto dirs = diru_DirsOnly(path);
    for (auto & dir : dirs)
        { removeDirAndContents(path + sep + dir.name); }

    // remove the passed (now empty) dir
    return (rmdir(path.c_str()) == 0);
}

//*******************************
// DirEntry::copy file
//*******************************
bool DirEntry::copy(const string &source, const string &dest) {
    ifstream infile;
    ofstream outfile;

    char *buffer;
    buffer = new char[FILE_BUFFER_SIZE];

    infile.open(source, ios::binary);
    outfile.open(dest, ios::binary);

    if (!infile.good()) return false;
    if (!outfile.good()) return false;

    while (true) {
        int read = infile.readsome(buffer, FILE_BUFFER_SIZE);
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
string DirEntry::removeDotFromExtension(const std::string &ext) {
    if (ext.c_str()[0] == '.')
        return ext.c_str() + 1;
    else
        return ext;
}

//*******************************
// DirEntry::addDotToExtension
//*******************************
// if it doesn't start with a ".", add it to the front
string DirEntry::addDotToExtension(const std::string &ext) {
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
// DirEntry::getFileExtension
//*******************************
// Return the extension of a filename without the "."
string DirEntry::getFileExtension(const string &fileName) {
    size_t i = fileName.rfind('.', fileName.length());
    if (i != string::npos) {
        return (fileName.substr(i + 1, fileName.length() - i));
    }
    return "";
}

//*******************************
// DirEntry::getFileNameWithoutExtension
//*******************************
// Return the name of a file without extension
string DirEntry::getFileNameWithoutExtension(const string &filename) {
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
    fp = fopen(cueFile.c_str(), "r");
    if (fp == NULL) {
        printf("Error opening cue file");
        return binList;
    }

    //Reading line by line
    while ((read = getline(&cline, &length, fp)) != -1) {
        line = cline;
        line = trim(line);
        if (line.substr(0, 4) == "FILE") {
            binList.push_back(Util::getStringWithinChar(line, '"').c_str());
        }
    }

    //Closing file pointer
    fclose(fp);

    //Freeing line pointer
    if (cline) {
        free(cline);
    }

    return binList;
}

//*******************************
// DirEntry::getFilesWithExtension
//*******************************
DirEntries
DirEntry::getFilesWithExtension(const string &path, const DirEntries &entries, const vector<string> &extensions) {
    DirEntries fileList;
    string fileExt;
    for (const auto &entry : entries) {
        if(isDirectory(path + sep + entry.name))
            continue;
        // make it case insensitive compare (find .bin and .BIN)
        fileExt = ReturnLowerCase(getFileExtension(entry.name));
        if (find(extensions.begin(), extensions.end(), fileExt) != extensions.end()) {
            fileList.push_back(entry);
        }
    }
    return fileList;
}

//*******************************
// DirEntry::print
//*******************************
void DirEntry::print() const {
    cout << (isDir ? "Dir: " : "File: ") << name << std::endl;
}

//*******************************
// DirEntries::print(const DirEntries &entries)
//*******************************
static void print(const DirEntries &entries) {
    for (auto & entry : entries)
        entry.print();
}

//*******************************
// DirEntry::isAGameFile
//*******************************
bool DirEntry::isAGameFile(const std::string &filename) {
    if (matchExtension(filename, EXT_BIN))
        return true;
    if (matchExtension(filename, EXT_PBP))
        return true;
    if (matchExtension(filename, EXT_IMG))
        return true;

    return false;
}

//*******************************
// DirEntry::getGameFileImageType
//*******************************
ImageType DirEntry::getGameFileImageType(const std::string &filename) {
    if (matchExtension(filename, EXT_BIN))
        return IMAGE_BIN;
    if (matchExtension(filename, EXT_PBP))
        return IMAGE_PBP;
    if (matchExtension(filename, EXT_IMG))
        return IMAGE_IMG;

    return IMAGE_NO_GAME_FOUND;
}

//*******************************
// DirEntry::imageTypeUsesACueFile
//*******************************
bool DirEntry::imageTypeUsesACueFile(ImageType imageType) {
    if (imageType == IMAGE_BIN || imageType == IMAGE_IMG)
        return true;
    else
        return false;
}

//*******************************
// DirEntry::thereIsAGameFile
//*******************************
bool DirEntry::thereIsAGameFile(const DirEntries &entries) {
    return any_of(begin(entries), end(entries), [] (const DirEntry &entry) { return !entry.isDir && isAGameFile(entry.name); } );
}

//*******************************
// DirEntry::thereIsASubDir
//*******************************
bool DirEntry::thereIsASubDir(const DirEntries &entries) {
    return any_of(begin(entries), end(entries), [] (const DirEntry &entry) { return entry.isDir; } );
}

//*******************************
// DirEntry::getGameFile
// Note: you must know that the game file exists in the directory before calling this function
//*******************************
tuple<ImageType, string> DirEntry::getGameFile(const DirEntries &entries) {
    auto iter = find_if(begin(entries), end(entries), [] (const DirEntry &entry) { return isAGameFile(entry.name); } );
    if (iter != end(entries))
        return make_tuple(getGameFileImageType(iter->name), iter->name);
    else
        return make_tuple(IMAGE_NO_GAME_FOUND, "");
}