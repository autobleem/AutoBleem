
#include <string>
#include "util.h"
#include "inifile.h"

using namespace std;

string valueOrDefault(string name, string def, map<string,string> iniValues) {
    string value;
    if (iniValues.find(name) != iniValues.end()) {
        value = trim(iniValues.find(name)->second);
        if (value.length() == 0) {

            return def;
        }
    } else {

        value = def;
    }
    return value;
}

#define PCSX "/tmp/pcsx"

void execute(int argc, char** argv)
{
    execvp(PCSX,argv);
}

int main (int argc, char *argv[])
{


    string path="/data/AppData/sony/title/";


    Inifile ini;
    ini.load(path+"Game.ini");
    string imageType=valueOrDefault("imagetype","0",ini.values);

    std::vector<std::string> arguments;
    for (int i=0;i<argc;i++)
    {
        arguments.push_back(argv[i]);
    }


    if (imageType!="0")
    {
        for (int i=0;i<arguments.size();i++)
        {
            if (arguments[i]=="-cdfile")
            {
                string image=arguments[i+1];
                image = image.substr(0,image.size()-4);
                arguments[i+1] = image;
            }
        }
    }


    std::vector<char*> argvNew;
    for (const auto& arg : arguments)
        argvNew.push_back((char*)arg.data());
    argvNew.push_back(nullptr);

    execute(argvNew.size() - 1, argvNew.data());

    return 0;
}
