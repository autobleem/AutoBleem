
#include <string>
#include <sys/wait.h>
#include "util.h"
#include "inifile.h"
#include "memcard.h"

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
    int pid = fork();
    if (!pid) {
        execvp(PCSX, argv);
    }
    waitpid(pid, NULL, 0);
}

int main (int argc, char *argv[])
{

    string path="/data/AppData/sony/title/";
    string sourceCard="/media/Games/!MemCards/";
    Inifile ini;
    ini.load(path+"Game.ini");
    string imageType=valueOrDefault("imagetype","0",ini.values);
    string memcard=valueOrDefault("memcard","SONY",ini.values);

    if (memcard!="SONY")
    {
        if (Util::exists(sourceCard+memcard))
        {
            Memcard * card = new Memcard("/media/Games/");
            if (!card->swapIn("./.pcsx",memcard))
            {
                memcard = "SONY";
                ini.values["memcard"]="SONY";
                ini.save(path+"Game.ini");
            };
            delete card;
        }
    }

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
                if (Util::matchExtension(image,".cue")) {
                    image = image.substr(0, image.size() - 4);
                }
                arguments[i+1] = image;
            }
        }
    }


    std::vector<char*> argvNew;
    for (const auto& arg : arguments)
        argvNew.push_back((char*)arg.data());


    argvNew.push_back(nullptr);
    execute(argvNew.size() - 1, argvNew.data());

    if (memcard!="SONY")
    {

            Memcard * card = new Memcard("/media/Games/");
            card->swapOut("./.pcsx",memcard);
            delete card;

    }

    return 0;
}
