#pragma once

#include "environment.h"
#include "util.h"
#include "enums.h"
#include "stringinline.h"

#include <tuple>
#include <string>
#include <vector>





//*******************************
// separator
//*******************************
#ifdef _WIN32
    static const char separator = '\\';
#else
    static const char separator = '/';
#endif

//*******************************
// append separator helper function
//*******************************
struct Sep { };
const Sep sep;
// to use "operator +" below, "path + sep" will append the separator only if it's not already on the end of path
std::string operator + (const std::string &leftside, Sep);
// to use "operator +=" "path += sep;"
void operator += (std::string &leftside, Sep);

//******************
// DirEntry
//******************
using DirEntries = std::vector<class DirEntry>;

class DirEntry {
public:
    std::string name;
    bool isDir;

    DirEntry(std::string _name, bool dir) : name(_name), isDir(dir) { };
    static bool sortDirEntryByName(const DirEntry & i, const DirEntry & j) { return SortByCaseInsensitive(i.name, j.name); }

    static std::string fixPath(std::string path);   // removes leading and trailing spaces and removes any trailing '/'
    static std::string removeSeparatorFromEndOfPath(const std::string& path);   // return the path without a separator at the end

    static std::string removeGamesPathFromFrontOfPath(const std::string& path);

    static std::string replaceTheseCharsWithThisChar(std::string str, const std::string& charsToReplace, char replacementChar);
    static bool fixCommaInDirOrFileName(const std::string &path, DirEntry *entry);

    static DirEntries dir(std::string path);   // returns directory contents including . and ..
    static DirEntries diru(std::string path);  // returns directory contents except . and ..
    static DirEntries diru_DirsOnly(std::string path);  // diru but only returns directories
    static DirEntries diru_FilesOnly(std::string path);  // diru but only returns files

    static bool copy(const std::string& source, const std::string& dest); // copies a file
    static bool exists(const std::string &name);
    static bool createDir(const std::string &name);
    static int rmDir(std::string path);
    static bool removeDirAndContents(const std::string path);
    static std::string removeDotFromExtension(const std::string & ext); // if it has it
    static std::string addDotToExtension(const std::string & ext); // if it doesn't start with a "."
    static bool matchExtension(std::string path, std::string ext);  // this does a case insensitive compare
    static bool isDirectory(const std::string& path);
    static DirEntries getFilesWithExtension(const std::string& path, const DirEntries & entries,
                                            const std::vector<std::string> & extensions);    // pass file extensions in lower case
    static std::string getFileNameFromPath(const std::string& path);
    static std::string getDirNameFromPath(const std::string& path);
    static std::string getFileExtension(const std::string & fileName);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    static std::string findFirstFile(std::string ext, std::string path);
    static std::vector<std::string> cueToBinList(std::string cueFile);
    static bool isPBPFile(std::string path);
    static void generateM3UForDirectory(std::string path, std::string basename);

    void print() const;
    static void print(const DirEntries &entries);

    static bool isAGameFile(const std::string &filename);
    static ImageType getGameFileImageType(const std::string &filename);
    static bool imageTypeUsesACueFile(ImageType imageType);

    static bool thereIsAGameFile(const DirEntries &entries);
    static bool thereIsAGameFile(const std::string &dirpath) {
        return thereIsAGameFile(diru(dirpath)); }

    static bool thereIsASubDir(const DirEntries &entries);
    static bool thereIsASubDir(const std::string &dirpath) {
        return thereIsASubDir(diru(dirpath)); }

    // Note: you must know that the game file exists in the directory before calling these functions
    static std::tuple<ImageType, std::string> getGameFile(const DirEntries &entries);
    static std::tuple<ImageType, std::string> getGameFile(const std::string &dirpath) {
        return getGameFile(diru(dirpath)); }
};
