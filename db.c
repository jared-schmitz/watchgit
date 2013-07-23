/*
 * db.c
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef DB_LOCATION
#define DB_LOCATION "~/.watchgit.db"
#endif

#include "db.h"
#include <errno.h>
#include <limits.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wordexp.h>

/* void* != funcptr */
/* So just wrap it! */
struct callback_container {
  db_iter_func_t callback;
};

static sqlite3 *create_new_db(const char *path);
static int foreach_repo_callback(void *container,
  int argc, char **argv, char **col_name);

/*
 * add_repo_to_db()
 *
 * Adds a repository to track to the
 * database. Returns the insert status,
 * or SQLITE_ERROR.
 */
int add_repo_to_db(sqlite3 *dbh, const char *alias, const char *path) {
  char abspath[PATH_MAX], *fullquery;
  int status = SQLITE_ERROR;

  static const char *query = "INSERT INTO "
    "repos_table (aliases, paths) VALUES("
    "\"%s\", \"%s\")";

  if ((fullquery = malloc(strlen(query) +
    sizeof(abspath) + strlen(alias) + 1)) == NULL)
    return status;

  if (realpath(path, abspath) == NULL) {
    perror("realpath");
    return status;
  }

  sprintf(fullquery, query, alias, abspath);
  status = sqlite3_exec(dbh, fullquery, NULL, NULL, NULL);
  free(fullquery);

  return status;
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
    "AUTOINCREMENT, aliases TEXT UNIQUE, paths TEXT UNIQUE);", NULL, NULL,
    NULL) != SQLITE_OK) {
    sqlite3_close(dbh);

    unlink(DB_LOCATION);
    return NULL;
  }

  return dbh;
}

/*
 * foreach_repo()
 *
 * Executes a callback function for
 * every repository in the database.
 *
 * TODO: Don't use sqlite3_exec so
 * we don't have to do illegal casts.
 */
int foreach_repo(sqlite3 *dbh, db_iter_func_t function) {
  struct callback_container container;
  container.callback = function;

  if (sqlite3_exec(dbh, "SELECT aliases, paths FROM repos_table",
    foreach_repo_callback, &container, NULL) != SQLITE_OK)
    return -1;

  return 0;
}

/*
 * foreach_repo_callback()
 *
 * Indirectly invoked by foreach_repo
 * when retrieving database results.
 */
static int foreach_repo_callback(void *container_,
  int argc, char **argv, char **col_name) {
  struct callback_container *container;
  int i = argc, status = 0;
  (void) col_name;

  container = (struct callback_container*) container_;

  for (i = 0; i < argc; i++)
    status |= container->callback(col_name[i], argv[i]);

  return status;
}

/*
 * get_db_handle()
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

/*
 * remove_repo_from_db()
 *
 * Removes a repository from the
 * database. Returns the removal status,
 * or SQLITE_ERROR.
 */
int remove_repo_from_db(sqlite3 *dbh, const char *alias) {
  int status = SQLITE_ERROR;
  char *fullquery;

  static const char *query = "DELETE FROM "
    "repos_table WHERE aliases=\"%s\"";

  if ((fullquery = malloc(strlen(query)
    + strlen(alias) )) == NULL)
    return status;

  sprintf(fullquery, query, alias);
  status = sqlite3_exec(dbh, fullquery, NULL, NULL, NULL);
  free(fullquery);

  return status;
}

