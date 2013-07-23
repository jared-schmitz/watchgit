/*
 * db.c
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef DB_LOCATION
#define DB_LOCATION "~/.watchgit.db"
#endif

#include <errno.h>
#include <limits.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wordexp.h>

static sqlite3 *create_new_db(const char *path);

/*
 * add_repo_to_db()
 *
 * Adds a repository to track to the
 * database. Returns the insert status,
 * or SQLITE_ERROR.
 */
int add_repo_to_db(sqlite3 *dbh, const char *path) {
  static const char *query = "INSERT INTO repos_table (paths) VALUES(\"%s\")";
  char abspath[PATH_MAX], fullquery[PATH_MAX + strlen(query)];

  if (realpath(path, abspath) == NULL) {
    perror("realpath");
    return SQLITE_ERROR;
  }

  sprintf(fullquery, query, abspath);
  return sqlite3_exec(dbh, fullquery, NULL, NULL, NULL);
}

/*
 * close_db_handle()
 *
 * Closes the connection to the database.
 * Call after get_db_handle() successful
 * returns.
 */
void close_db_handle(sqlite3 *dbh) {
  sqlite3_close(dbh);
}

/*
 * create_new_db()
 *
 * Creates a new SQLite database for
 * storing repositories and writes the
 * schema. Returns handle to file.
 */
static sqlite3 *create_new_db(const char *path) {
  sqlite3 *dbh;

  if (sqlite3_open(path, &dbh) != SQLITE_OK) {
    printf("Failed to create a new database.\n");
    return NULL;
  }

  if (sqlite3_exec(dbh, "CREATE TABLE repos_table(id INTEGER PRIMARY KEY "
    "AUTOINCREMENT, paths TEXT UNIQUE);", NULL, NULL, NULL) != SQLITE_OK) {
    sqlite3_close(dbh);

    unlink(DB_LOCATION);
    return NULL;
  }

  return dbh;
}

/*
 * get_db_handle():
 *
 * Gets a handle to the SQLite database.
 * Creates a new database if none exists.
 * Returns NULL on error.
 */
sqlite3 *get_db_handle(void) {
  struct stat statbuf;
  char path[PATH_MAX];
  sqlite3 *dbh;
  wordexp_t p;

  if (wordexp(DB_LOCATION, &p, 0) != 0)
    return NULL;

  strcpy(path, p.we_wordv[0]);
  wordfree(&p);

  if (stat(path, &statbuf)!= 0) {
    if (errno == ENOENT)
      return create_new_db(path);

    else {
      perror("stat");
      return NULL;
    }
  }

  if (sqlite3_open_v2(path, &dbh,
    SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
    return NULL;

  return dbh;
}

