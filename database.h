#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Function declarations for database operations
int open_database(sqlite3 **db, const char *filename);  // Updated declaration
void close_database(sqlite3 *db);
int create_tables(sqlite3 *db);
int execute_query(sqlite3 *db, const char *sql);

#endif
