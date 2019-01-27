//
// Created by screemer on 2019-01-27.
//

#include "ver_migration.h"
#include "util.h"
#include "config.h"
#include "inifile.h"
#include <fstream>

string verFile = "/media/System/Logs/ver.txt";

string VerMigration::getLastRunVersion()
{
    string version="";

    if (!Util::exists(verFile))
    {
        version="v0.4.0";
    } else
    {
        ifstream is(verFile);
        string ver;
        getline(is,ver);
        version = ver;
        is.close();
    }
    trim(version);
    return version;

}
void split(const string& s, char c,
           vector<string>& v) {
    int i = 0;
    int j = s.find(c);

    while (j >= 0) {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j < 0) {
            v.push_back(s.substr(i, s.length()));
        }
    }
}

void VerMigration::migrate04_05(Database * db)
{
    cout << "Migrating 0.4.0 to 0.5.0" << endl;
    // update game ini's with correct year (by title)
    int newYear = 2000; // initial value
    ifstream is(Util::getWorkingPath()+Util::separator()+"autobleem.list");
    string line;
    while (getline(is, line)) {
        std::vector<string> vect;

        split(line,',',vect);

        if (vect.size()>0) {
            string id = vect[0];
            string folder = vect[1];

            cout << id << endl;
            cout << folder << endl;

            // try to open game.ini
            string gameIniLoc = folder+Util::separator()+"GameData"+Util::separator()+"Game.ini";
            if (!Util::exists(gameIniLoc))
            {
                gameIniLoc = folder+Util::separator()+"Game.ini";
            }
            if (!Util::exists(gameIniLoc))
            {
                continue;
            }
            Inifile ini;
            ini.load(gameIniLoc);
            string title = ini.values["title"];
            cout << title << endl;

            Metadata * md = new Metadata();
            md->lookup2(title);
            {
                if(md->valid)
                {
                    cout << to_string(md->year) << endl;
                    db->updateYear(atoi(id.c_str()),md->year);
                    ini.values["year"]=to_string(md->year);
                    ini.save(gameIniLoc);
                }
            }
            delete (md);

        }
    }
    is.close();


}
void VerMigration::migrate(Database * db)
{
    string last=getLastRunVersion();
    Config cfg;
    string current=cfg.inifile.values["version"];
    bool autobleemEverRun = Util::exists("/media/System/Logs/autobleem.log");
    if ((autobleemEverRun) && (current!=last))
    {
        // we need to migrate from 0.4 to 0.5.0
        if (last=="v0.4.0")
        {
            migrate04_05(db);
        }



    }
    ofstream os(verFile);
    os << current << endl;
    os.close();

}