#include "database.h"
#include "inifile.h"
#include "../util.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include "serialscanner.h"
#include "../DirEntry.h"

using namespace std;

                                  //*******************************
                                  // DATABASE SQL
                                  //*******************************

//*******************************
// covers?.db
//*******************************

static const char SELECT_META[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS,  COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE SERIAL=? OR SERIAL LIKE ?";

static const char SELECT_TITLE[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS, COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE TITLE=?";

//*******************************
// RELEASE_YEAR is found in both internal.db and output.db
// used by Database::updateYear() which is only called by VerMigration::migrate04_05()
// VerMigration appears to be no longer used
//*******************************
static const char UPDATE_YEAR[] = "UPDATE GAME SET RELEASE_YEAR=? WHERE GAME_ID=?";

//*******************************
// output.db
//*******************************

static const char UPDATE_MEMCARD[] = "UPDATE GAME SET MEMCARD=? WHERE GAME_ID=?";

static const char GAMES_DATA[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, PATH, SSPATH, MEMCARD, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char GAMES_DATA_SINGLE[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, PATH, SSPATH, MEMCARD, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                    WHERE g.GAME_ID=?  \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char INSERT_GAME[] = "INSERT INTO GAME ([GAME_ID],[GAME_TITLE_STRING],[PUBLISHER_NAME],[RELEASE_YEAR],[PLAYERS],[RATING_IMAGE],[GAME_MANUAL_QR_IMAGE],[LINK_GAME_ID],\
                [PATH],[SSPATH],[MEMCARD]) \
                values (?,?,?,?,?,'CERO_A','QR_Code_GM','',?,?,?)";

static const char CREATE_GAME_SQL[] = " CREATE TABLE IF NOT EXISTS GAME  \
     ( GAME_ID integer NOT NULL UNIQUE, \
       GAME_TITLE_STRING text, \
       PUBLISHER_NAME text, \
       RELEASE_YEAR integer,\
       PLAYERS integer,     \
       RATING_IMAGE text,   \
       GAME_MANUAL_QR_IMAGE text, \
       LINK_GAME_ID integer,\
       PATH    text null,   \
       SSPATH  text null,   \
       MEMCARD text null,   \
         PRIMARY KEY ( GAME_ID ) )";

static const char CREATE_DISC_SQL[] = " CREATE TABLE IF NOT EXISTS DISC \
     ( [GAME_ID] integer, \
       [DISC_NUMBER] integer, \
       [BASENAME] text, \
          UNIQUE ([GAME_ID], [DISC_NUMBER]) )";

static const char CREATE_SUBDIR_ROW_SQL[] = " CREATE TABLE IF NOT EXISTS SUBDIR_ROWS  \
     ( SUBDIR_ROW_INDEX integer NOT NULL UNIQUE, \
       SUBDIR_ROW_NAME text, \
       INDENT_LEVEL integer,    \
       NUM_GAMES integer,    \
       PRIMARY KEY ( SUBDIR_ROW_INDEX ) )";

static const char IS_SUBDIR_ROWS_TABLE_EMPTY[] = "SELECT count(*) FROM SUBDIR_ROWS";

static const char INSERT_SUBDIR_ROW[] = "INSERT INTO SUBDIR_ROWS \
        ([SUBDIR_ROW_INDEX],[SUBDIR_ROW_NAME],[INDENT_LEVEL],[NUM_GAMES]) \
        values (?,?,?,?)";

static const char GET_SUBDIR_ROW[] = "SELECT SUBDIR_ROW_INDEX, SUBDIR_ROW_NAME, INDENT_LEVEL, NUM_GAMES FROM \
        SUBDIR_ROWS ORDER BY SUBDIR_ROW_INDEX";

static const char CREATE_SUBDIR_GAMES_TO_DISPLAY_ON_ROW_SQL[] = " CREATE TABLE IF NOT EXISTS SUBDIR_GAMES_TO_DISPLAY_ON_ROW  \
     ( SUBDIR_ROW_INDEX integer, GAME_ID integer )";

static const char INSERT_SUBDIR_GAME[] = "INSERT INTO SUBDIR_GAMES_TO_DISPLAY_ON_ROW \
        ([SUBDIR_ROW_INDEX],[GAME_ID]) values (?,?)";

static const char GET_SUBDIR_GAME[] = "SELECT SUBDIR_ROW_INDEX, GAME_ID FROM \
        SUBDIR_GAMES_TO_DISPLAY_ON_ROW ORDER BY SUBDIR_ROW_INDEX";

static const char GET_SUBDIR_GAME_ON_ROW[] = "SELECT GAME_ID FROM \
        SUBDIR_GAMES_TO_DISPLAY_ON_ROW WHERE SUBDIR_ROW_INDEX =?";

//*******************************
// internal.db
//*******************************

static const char GAMES_DATA_SINGLE_INTERNAL[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                    WHERE g.GAME_ID=?  \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

static const char GAMES_DATA_INTERNAL[] = "SELECT g.GAME_ID, GAME_TITLE_STRING, PUBLISHER_NAME, RELEASE_YEAR, PLAYERS, d.BASENAME,  COUNT(d.GAME_ID) as NUMD \
                                  FROM GAME G JOIN DISC d ON g.GAME_ID=d.GAME_ID \
                                     GROUP BY g.GAME_ID HAVING MIN(d.DISC_NUMBER) \
                                     ORDER BY g.GAME_TITLE_STRING asc,d.DISC_NUMBER ASC";

//*******************************
// ????.db
//*******************************

static const char UPDATE_TITLE[] = "UPDATE GAME SET GAME_TITLE_STRING=? WHERE GAME_ID=?";

static const char NUM_GAMES[] = "SELECT COUNT(*) as ctn FROM GAME";

static const char UPDATE_GAME_DB[] = "ALTER TABLE GAME ADD COLUMN FAV INT DEFAULT 0";

static const char CREATE_LANGUAGE_SPECIFIC_SQL[] = "CREATE TABLE IF NOT EXISTS LANGUAGE_SPECIFIC \
      ( [DEFAULT_VALUE] text, \
        [LANGUAGE_ID] integer, \
        [VALUE] text, \
           UNIQUE ([DEFAULT_VALUE], [LANGUAGE_ID]) )";

static const char BEGIN_TRANSACTION[] = "BEGIN TRANSACTION";
static const char COMMIT[] = "COMMIT";

static const char DELETE_GAME_DATA[] = "DELETE FROM GAME";
static const char DELETE_DISC_DATA[] = "DELETE FROM DISC";
static const char DELETE_LANGUAGE_DATA[] = "DELETE FROM LANGUAGE_SPECIFIC";
static const char DELETE_SUBDIR_ROW_DATA[] = "DELETE FROM SUBDIR_ROWS";
static const char DELETE_SUBDIR_GAME_DATA[] = "DELETE FROM SUBDIR_GAMES_TO_DISPLAY_ON_ROW";

static const char INSERT_DISC[] = "INSERT INTO DISC ([GAME_ID],[DISC_NUMBER],[BASENAME]) \
                values (?,?,?)";

//*******************************
// DATABASE code
//*******************************

//*******************************
// Database::getNumGames
//*******************************
int Database::getNumGames() {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, NUM_GAMES, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const int number = sqlite3_column_int(res, 0);
            sqlite3_finalize(res);
            return number;
        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return 0;
    }
    sqlite3_finalize(res);
    return 0;
}

//*******************************
// Database::updateYear
// called by VerMigration::migrate04_05()
//*******************************
bool Database::updateYear(int id, int year) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_YEAR, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_int(res, 1, year);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateMemcard
//*******************************
bool Database::updateMemcard(int id, string memcard) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_MEMCARD, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_text(res, 1, memcard.c_str(), -1, nullptr);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::updateTitle
//*******************************
bool Database::updateTitle(int id, string title) {
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_TITLE, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_text(res, 1, title.c_str(), -1, nullptr);
    sqlite3_bind_int(res, 2, id);
    sqlite3_step(res);
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::queryTitle
//*******************************
bool Database::queryTitle(string title, Metadata *md) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, SELECT_TITLE, -1, &res, nullptr);
    if (rc == SQLITE_OK) {

        sqlite3_bind_text(res, 1, title.c_str(), -1, nullptr);
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            const int year = sqlite3_column_int(res, 3);
            const int players = sqlite3_column_int(res, 4);
            const void *bytes = sqlite3_column_blob(res, 5);
            size_t size = sqlite3_column_bytes(res, 5);
            if (size != 0) {
                md->dataSize = size;
                md->bytes = new char[size];
                memcpy(md->bytes, bytes, size);
            }
            md->title = string(reinterpret_cast<const char *>(title));
            md->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(md->publisher);
            md->year = year;
            md->players = players;
            md->valid = true;
            return true;

        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return false;
}

//*******************************
// Database::getInternalGames
//*******************************
bool Database::getInternalGames(PsGames *result) {
    result->clear();
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_INTERNAL, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *base = sqlite3_column_text(res, 5);
            int discs = sqlite3_column_int(res, 6);

            PsGamePtr psGame{new PsGame};
            psGame->gameId = id;
            psGame->title = string(reinterpret_cast<const char *>(title));
            psGame->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(psGame->publisher);
            psGame->year = year;
            psGame->players = players;
            psGame->folder = "/gaadata/" + to_string(id) + "/";
            psGame->ssFolder = "/media/Games/!SaveStates/" + to_string(id) + "/";
            psGame->base = string(reinterpret_cast<const char *>(base));
            psGame->serial = psGame->base;
            psGame->region = SerialScanner::serialToRegion(psGame->serial);
            psGame->memcard = "SONY";
            psGame->internal = true;
            psGame->cds = discs;
            result->push_back(psGame);
            //cout << "getInternalGames: " << game->serial << ", " << game->title << endl;
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::refreshGameInternal
//*******************************
bool Database::refreshGameInternal(PsGamePtr &psGame) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_SINGLE_INTERNAL, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, psGame->gameId);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *base = sqlite3_column_text(res, 5);
            int discs = sqlite3_column_int(res, 6);

            psGame->gameId = id;
            psGame->title = string(reinterpret_cast<const char *>(title));
            psGame->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(psGame->publisher);
            psGame->year = year;
            psGame->players = players;
            psGame->folder = "/gaadata/" + to_string(id) + "/";
            psGame->ssFolder = "/media/Games/!SaveStates/" + to_string(id) + "/";
            psGame->base = string(reinterpret_cast<const char *>(base));
            psGame->serial = psGame->base;
            psGame->region = SerialScanner::serialToRegion(psGame->serial);
            psGame->memcard = "SONY";
            psGame->internal = true;
            psGame->cds = discs;

            string gameIniPath = psGame->folder + "/Game.ini";
            if (DirEntry::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                psGame->locked =  !(ini.values["automation"]=="1");
                psGame->hd =       (ini.values["highres"]=="1");
                psGame->favorite = (ini.values["favorite"] == "1");
            }
        }
    } else {

        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::refreshGame
//*******************************
bool Database::refreshGame(PsGamePtr &game) {

    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA_SINGLE, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, game->gameId);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *path = sqlite3_column_text(res, 5);
            const unsigned char *sspath = sqlite3_column_text(res, 6);
            const unsigned char *memcard = sqlite3_column_text(res, 7);
            const unsigned char *base = sqlite3_column_text(res, 8);
            int discs = sqlite3_column_int(res, 9);

            game->gameId = id;
            game->title = string(reinterpret_cast<const char *>(title));
            game->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(game->publisher);
            game->year = year;
            game->players = players;
            game->folder = string(reinterpret_cast<const char *>(path));
            game->ssFolder = string(reinterpret_cast<const char *>(sspath));
            game->base = string(reinterpret_cast<const char *>(base));
            game->memcard = string(reinterpret_cast<const char *>(memcard));
            game->cds = discs;

            string gameIniPath = game->folder + "/Game.ini";
            if (DirEntry::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                game->locked =  !(ini.values["automation"]=="1");
                game->hd =       (ini.values["highres"]=="1");
                game->favorite = (ini.values["favorite"] == "1");
            }
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getGames
//*******************************
bool Database::getGames(PsGames *result) {
    result->clear();
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GAMES_DATA, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            int id = sqlite3_column_int(res, 0);
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            int year = sqlite3_column_int(res, 3);
            int players = sqlite3_column_int(res, 4);
            const unsigned char *path = sqlite3_column_text(res, 5);
            const unsigned char *sspath = sqlite3_column_text(res, 6);
            const unsigned char *memcard = sqlite3_column_text(res, 7);
            const unsigned char *base = sqlite3_column_text(res, 8);
            int discs = sqlite3_column_int(res, 9);

            PsGamePtr game{new PsGame};
            game->gameId = id;
            game->title = string(reinterpret_cast<const char *>(title));
            game->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(game->publisher);
            game->year = year;
            game->players = players;
            game->folder = string(reinterpret_cast<const char *>(path));
            game->ssFolder = string(reinterpret_cast<const char *>(sspath));
            game->base = string(reinterpret_cast<const char *>(base));
            game->memcard = string(reinterpret_cast<const char *>(memcard));
            game->cds = discs;
            //cout << "getGames: " << game->serial << ", " << game->title << endl;

            string gameIniPath = game->folder + sep + "Game.ini";
            if (DirEntry::exists(gameIniPath)) {
                Inifile ini;
                ini.load(gameIniPath);
                game->locked =  !(ini.values["automation"]=="1");
                game->hd =       (ini.values["highres"]=="1");
                game->favorite = (ini.values["favorite"] == "1");
            }
            result->push_back(game);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getGameRowInfos
//*******************************
bool Database::getGameRowInfos(GameRowInfos *gameRowInfos) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GET_SUBDIR_ROW, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            SubDirRowInfo subDirRowInfo;
            subDirRowInfo.subDirRowIndex = sqlite3_column_int(res, 0);
            const unsigned char *name = sqlite3_column_text(res, 1);
            subDirRowInfo.rowName = (const char*) name;
            subDirRowInfo.indentLevel = sqlite3_column_int(res, 2);
            subDirRowInfo.numGames = sqlite3_column_int(res, 3);

            cout << "SubDirRowInfo: " << string(subDirRowInfo.indentLevel * 2, ' ') << subDirRowInfo.rowName
                    << ", index: " << subDirRowInfo.subDirRowIndex
                    << ", indent: " << subDirRowInfo.indentLevel
                    << ", numGames: " << subDirRowInfo.numGames << endl;

            gameRowInfos->emplace_back(subDirRowInfo);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getSubDirGames
//*******************************
bool Database::getGameRowGameInfos(GameRowGames *gameRowGames) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GET_SUBDIR_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(res) == SQLITE_ROW) {
            GameRowGame gameRowGame;
            gameRowGame.rowIndex = sqlite3_column_int(res, 0);
            gameRowGame.gameId = sqlite3_column_int(res, 1);

            cout << "GameRowGame: " << gameRowGame.rowIndex << ", " << gameRowGame.gameId << endl;

            gameRowGames->emplace_back(gameRowGame);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::getSubDirGames
//*******************************
bool Database::getGameIdsInRow(vector<int> *gameIdsInRow, int row) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, GET_SUBDIR_GAME_ON_ROW, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, row);
        while (sqlite3_step(res) == SQLITE_ROW) {
            int gameId = sqlite3_column_int(res, 0);
            //cout << "GameId in Row: " << row << ", " << gameId << endl;
            gameIdsInRow->emplace_back(gameId);
        }
    } else {
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::querySerial
//*******************************
bool Database::querySerial(string serial, Metadata *md) {
    string serialLike = serial + "-%";
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, SELECT_META, -1, &res, nullptr);
    if (rc == SQLITE_OK) {

        sqlite3_bind_text(res, 1, serial.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 2, serialLike.c_str(), -1, nullptr);
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const unsigned char *title = sqlite3_column_text(res, 1);
            const unsigned char *publisher = sqlite3_column_text(res, 2);
            const int year = sqlite3_column_int(res, 3);
            const int players = sqlite3_column_int(res, 4);
            const void *bytes = sqlite3_column_blob(res, 5);
            size_t size = sqlite3_column_bytes(res, 5);
            if (size != 0) {
                md->dataSize = size;
                md->bytes = new char[size];
                memcpy(md->bytes, bytes, size);
            }

            const unsigned char *path = sqlite3_column_text(res, 6);
            md->title = string(reinterpret_cast<const char *>(title));
            md->publisher = string(reinterpret_cast<const char *>(publisher));
            Util::cleanPublisherString(md->publisher);
            md->year = year;
            md->serial = serial;
            md->region = SerialScanner::serialToRegion(md->serial);
            md->players = players;
            md->valid = true;
            //cout << "querySerial: " << "serial " << serial << ", " << md->title << endl;

            return true;

        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return false;
}

//*******************************
// Database::insertDisc
//*******************************
bool Database::insertDisc(int id, int discNum, string discName) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_DISC, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, id);
        sqlite3_bind_int(res, 2, discNum);
        sqlite3_bind_text(res, 3, discName.c_str(), -1, nullptr);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::insertGame
//*******************************
bool Database::insertGame(int id, string title, string publisher, int players, int year, string path, string sspath,
                          string memcard) {
    sqlite3_stmt *res = nullptr;
    Util::cleanPublisherString(publisher);
    int rc = sqlite3_prepare_v2(db, INSERT_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, id);
        sqlite3_bind_text(res, 2, title.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 3, publisher.c_str(), -1, nullptr);
        sqlite3_bind_int(res, 4, year);
        sqlite3_bind_int(res, 5, players);
        sqlite3_bind_text(res, 6, path.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 7, sspath.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 8, memcard.c_str(), -1, nullptr);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::subDirRowsTableIsEmpty
// returns true if no rows in table or failure
// *******************************
bool Database::subDirRowsTableIsEmpty() {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, IS_SUBDIR_ROWS_TABLE_EMPTY, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        int result = sqlite3_step(res);
        if (result == SQLITE_ROW) {
            const int number = sqlite3_column_int(res, 0);
            sqlite3_finalize(res);
            return (number == 0);   // true if no rows in table
        }
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return true;
    }
    sqlite3_finalize(res);
    return true;
}

//*******************************
// Database::insertSubDirRow
//*******************************
bool Database::insertSubDirRow(int rowIndex, string rowName, int indentLevel, int numGames) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_SUBDIR_ROW, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, rowIndex);
        sqlite3_bind_text(res, 2, rowName.c_str(), -1, nullptr);
        sqlite3_bind_int(res, 3, indentLevel);
        sqlite3_bind_int(res, 4, numGames);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);

    return true;
}

//*******************************
// Database::insertSubDirGames
//*******************************
bool Database::insertSubDirGames(int rowIndex, int gameId) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_SUBDIR_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, rowIndex);
        sqlite3_bind_int(res, 2, gameId);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);

    return true;
}

//*******************************
// Database::executeCreateStatement
//*******************************
bool Database::executeCreateStatement(char *sql, string tableName) {
    char *errorReport = nullptr;
    cout << "Creating " << tableName << " table (if not exists)" << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << "Failed to create " << tableName << "  table  " << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    return true;
}

//*******************************
// Database::executeStatement
//*******************************
bool Database::executeStatement(char *sql, string outMsg, string errorMsg) {
    char *errorReport = nullptr;
    cout << outMsg << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << errorMsg << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    return true;
}

//*******************************
// Database::connect
//*******************************
bool Database::connect(string fileName) {
    int rc = sqlite3_open(fileName.c_str(), &db);
    cout << "Connected to DB" << fileName << endl;
    if (rc != SQLITE_OK) {
        cout << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return true;
}

//*******************************
// Database::disconnect
//*******************************
void Database::disconnect() {
    if (db != nullptr) {
        cout << "Disconnecting DBs" << endl;
        sqlite3_db_cacheflush(db);
        sqlite3_close(db);
        db = nullptr;
    }
}

//*******************************
// Database::beginTransaction
//*******************************
bool Database::beginTransaction() {
    executeStatement((char *) BEGIN_TRANSACTION, "Begin Transaction", "Error beginning  transaction");
    return true;
}

//*******************************
// Database::commit
//*******************************
bool Database::commit() {
    executeStatement((char *) COMMIT, "Commit", "Error on commit");
    return true;
}

//*******************************
// Database::truncate
//*******************************
bool Database::truncate() {
    executeStatement((char *) DELETE_GAME_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_DISC_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_LANGUAGE_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_SUBDIR_ROW_DATA, "Truncating all data", "Error truncating data");
    executeStatement((char *) DELETE_SUBDIR_GAME_DATA, "Truncating all data", "Error truncating data");
    return true;
}

bool Database::createInitialDatabase() {
    if (!executeCreateStatement((char *) CREATE_GAME_SQL, "GAME")) return false;
    if (!executeCreateStatement((char *) CREATE_DISC_SQL, "DISC")) return false;
    if (!executeCreateStatement((char *) CREATE_LANGUAGE_SPECIFIC_SQL, "LANGUAGE_SPECIFIC")) return false;
    if (!executeCreateStatement((char *) CREATE_SUBDIR_ROW_SQL, "SUBDIR_ROWS")) return false;
    if (!executeCreateStatement((char *) CREATE_SUBDIR_GAMES_TO_DISPLAY_ON_ROW_SQL, "SUBDIR_GAMES_TO_DISPLAY_ON_ROW")) return false;

    return true;
}

//*******************************
// Database::createFavColumn
//*******************************
void Database::createFavColumn() {
    //   executeCreateStatement((char*) UPDATE_GAME_DB, "FAV column" );
}
