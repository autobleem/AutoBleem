#include <stdio.h>
#include "database.h"
#include "filesystem.h"



static const char create_games_sql[] = "CREATE TABLE IF NOT EXISTS GAME  \
     ( [GAME_ID] integer NOT NULL UNIQUE, \
       [GAME_TITLE_STRING] text, \
       [PUBLISHER_NAME] text, \
       [RELEASE_YEAR] integer, \
       [PLAYERS] integer, \
       [RATING_IMAGE] text, \
       [GAME_MANUAL_QR_IMAGE] text, \
       [LINK_GAME_ID] integer,\
         PRIMARY KEY ([GAME_ID]) )";
static const char create_disc_sql[] = "CREATE TABLE IF NOT EXISTS DISC \
     ( [GAME_ID] integer, \
       [DISC_NUMBER] integer, \
       [BASENAME] text, \
          UNIQUE ([GAME_ID], [DISC_NUMBER]) )";

static const char create_language_specific_sql[] = "CREATE TABLE IF NOT EXISTS LANGUAGE_SPECIFIC \
      ( [DEFAULT_VALUE] text, \
        [LANGUAGE_ID] integer, \
        [VALUE] text, \
           UNIQUE ([DEFAULT_VALUE], [LANGUAGE_ID]) )";
static const char truncate_all[] = "DELETE FROM GAME;DELETE FROM DISC;DELETE FROM LANGUAGE_SPECIFIC";

static const char insert_game[] = "INSERT INTO GAME ([GAME_ID],[GAME_TITLE_STRING],[PUBLISHER_NAME],[RELEASE_YEAR],[PLAYERS],[RATING_IMAGE],[GAME_MANUAL_QR_IMAGE],[LINK_GAME_ID]) \
                values (?,?,?,?,?,'CERO_A','QR_Code_GM','')";

static const char insert_disc[] = "INSERT INTO DISC ([GAME_ID],[DISC_NUMBER],[BASENAME]) \
                values (?,?,?)";

int insert_game_record(char * fileName, t_game_data game_data) {
    sqlite3 *db;
    sqlite3_stmt *res;

    // open new one and create all required structure
    int rc = sqlite3_open(fileName, &db);

    fprintf(stderr, "Connecting to SQLITE file: %s\n", fileName);

    rc = sqlite3_prepare_v2(db, insert_game, -1, &res, 0);

    if (rc == SQLITE_OK) {

        sqlite3_bind_int(res, 1, game_data.folder_id);
        sqlite3_bind_text(res, 2, game_data.title, -1, 0);
        sqlite3_bind_text(res, 3, game_data.publisher, -1, 0);
        sqlite3_bind_int(res, 4, game_data.year);
        sqlite3_bind_int(res, 5, game_data.players);
        sqlite3_step(res);

    } else {

        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

    }

    for (int i = 0;i < game_data.total_discs; i++) {
        rc = sqlite3_prepare_v2(db, insert_disc, -1, &res, 0);
        if (rc == SQLITE_OK) {

            sqlite3_bind_int(res, 1, game_data.folder_id);
            sqlite3_bind_int(res, 2, i+1);
            sqlite3_bind_text(res, 3, game_data.discs[i].diskname, -1, 0);
      
            sqlite3_step(res);

        } else {

            fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

        }
    }
    sqlite3_finalize(res);
    sqlite3_close(db);

    return 0;
}

int create_database(char * fileName) {


    sqlite3 *db;
    sqlite3_stmt *res;



    // open new one and create all required structure
    int rc = sqlite3_open(fileName, &db);

    fprintf(stderr, "Connecting to SQLITE file: %s\n", fileName);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Creating GAME table (if not exists)\n");

    rc = sqlite3_exec(db, create_disc_sql, 0, 0, &res);
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to create GAME: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Creating DISC table (if not exists)\n");

    rc = sqlite3_exec(db, create_games_sql, 0, 0, &res);
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to create DISC: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Creating LANGUAGE_SPECIFIC table (if not exists)\n");

    rc = sqlite3_exec(db, create_language_specific_sql, 0, 0, &res);
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to create LANGUAGE_SPECIFIC: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Removing all data \n");
    rc = sqlite3_exec(db, truncate_all, 0, 0, &res);
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to truncate tables: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    fprintf(stderr, "Finalizing Create Step\n");

    sqlite3_finalize(res);
    sqlite3_close(db);

    return 0;

};