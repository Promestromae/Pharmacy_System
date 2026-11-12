#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

// Database operations; open_database returns 1 on success and 0 on failure.
int open_database(sqlite3 **db, const char *filename);
void close_database(sqlite3 *db);
int create_tables(sqlite3 *db);
int ensure_sales_user_column(sqlite3 *db);
int execute_query(sqlite3 *db, const char *sql);

#endif
