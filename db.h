/*
 * db.h
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef __DB_H__
#define __DB_H__
#include <sqlite3.h>

int add_repo_to_db(sqlite3 *dbh, const char *path);
void close_db_handle(sqlite3 *dbh);
sqlite3 *get_db_handle(void);

#endif

