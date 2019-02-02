#include "database.h"


using namespace std;


static const char SELECT_META[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS, COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE SERIAL=? OR SERIAL LIKE ?";

static const char SELECT_TITLE[] = "SELECT SERIAL,TITLE, PUBLISHER, \
                                RELEASE,PLAYERS, COVER FROM SERIALS s \
                                JOIN GAME g on s.GAME=g.id \
                                WHERE TITLE=?";

static const char UPDATE_YEAR[] = "UPDATE GAME SET RELEASE_YEAR=? WHERE GAME_ID=?";

static const char NUM_GAMES[] = "SELECT COUNT(*) as ctn FROM GAME";
static const char CREATE_GAME_SQL[] = "CREATE TABLE IF NOT EXISTS GAME  \
     ( [GAME_ID] integer NOT NULL UNIQUE, \
       [GAME_TITLE_STRING] text, \
       [PUBLISHER_NAME] text, \
       [RELEASE_YEAR] integer, \
       [PLAYERS] integer, \
       [RATING_IMAGE] text, \
       [GAME_MANUAL_QR_IMAGE] text, \
       [LINK_GAME_ID] integer,\
         PRIMARY KEY ([GAME_ID]) )";
static const char CREATE_DISC_SQL[] = "CREATE TABLE IF NOT EXISTS DISC \
     ( [GAME_ID] integer, \
       [DISC_NUMBER] integer, \
       [BASENAME] text, \
          UNIQUE ([GAME_ID], [DISC_NUMBER]) )";

static const char CREATE_LANGUAGE_SPECIFIC_SQL[] = "CREATE TABLE IF NOT EXISTS LANGUAGE_SPECIFIC \
      ( [DEFAULT_VALUE] text, \
        [LANGUAGE_ID] integer, \
        [VALUE] text, \
           UNIQUE ([DEFAULT_VALUE], [LANGUAGE_ID]) )";
static const char DELETE_DATA[] = "DELETE FROM GAME;DELETE FROM DISC;DELETE FROM LANGUAGE_SPECIFIC";

static const char INSERT_GAME[] = "INSERT INTO GAME ([GAME_ID],[GAME_TITLE_STRING],[PUBLISHER_NAME],[RELEASE_YEAR],[PLAYERS],[RATING_IMAGE],[GAME_MANUAL_QR_IMAGE],[LINK_GAME_ID]) \
                values (?,?,?,?,?,'CERO_A','QR_Code_GM','')";

static const char INSERT_DISC[] = "INSERT INTO DISC ([GAME_ID],[DISC_NUMBER],[BASENAME]) \
                values (?,?,?)";

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

bool Database::updateYear(int id, int year)
{
    char *errorReport = nullptr;
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, UPDATE_YEAR, -1, &res, nullptr);
    if (rc != SQLITE_OK) {
        cerr <<  sqlite3_errmsg(db) << endl;
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
            md->title = std::string(reinterpret_cast<const char *>(title));
            md->publisher = std::string(reinterpret_cast<const char *>(publisher));
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

bool Database::querySerial(string serial, Metadata *md) {
    string serialLike = serial+"-%";
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
            md->title = std::string(reinterpret_cast<const char *>(title));
            md->publisher = std::string(reinterpret_cast<const char *>(publisher));
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

bool Database::insertGame(int id, string title, string publisher, int players, int year) {
    sqlite3_stmt *res = nullptr;
    int rc = sqlite3_prepare_v2(db, INSERT_GAME, -1, &res, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, id);
        sqlite3_bind_text(res, 2, title.c_str(), -1, nullptr);
        sqlite3_bind_text(res, 3, publisher.c_str(), -1, nullptr);
        sqlite3_bind_int(res, 4, year);
        sqlite3_bind_int(res, 5, players);
        sqlite3_step(res);
    } else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(res);
        return false;
    }
    sqlite3_finalize(res);
    return true;
}

bool Database::executeCreateStatement(char *sql, string tableName) {
    char *errorReport = nullptr;
    cout << "Creating " << tableName << " table (if not exists)" << endl;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorReport);
    if (rc != SQLITE_OK) {
        cerr << "Failed to create " << tableName << "  table" << sqlite3_errmsg(db) << endl;
        if (!errorReport) sqlite3_free(errorReport);
        sqlite3_close(db);
        return false;
    }
    return true;
}

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

void Database::disconnect() {
    if (db != nullptr) {
        cout << "Disconnecting DBs"  << endl;
        sqlite3_db_cacheflush(db);
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::createInitialDatabase() {
    if (!executeCreateStatement((char *) CREATE_GAME_SQL, "GAME")) return false;
    if (!executeCreateStatement((char *) CREATE_DISC_SQL, "DISC")) return false;
    if (!executeCreateStatement((char *) CREATE_LANGUAGE_SPECIFIC_SQL, "LANGUAGE_SPECIFIC")) return false;
    return executeStatement((char *) DELETE_DATA, "Truncating all data", "Error truncating data");


}

