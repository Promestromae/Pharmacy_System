#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include "database.h"

int authenticate_user(const char *username, const char *password, int *user_id) {
    int role = 0; // 1 = Admin, 2 = Patient, 0 = Invalid
    *user_id = 0;

    // Open database connection
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("pharmacy.db", &db);

    if (rc != SQLITE_OK) {
        printf("Error: Cannot connect to database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Query to validate user credentials
    const char *sql =
        "SELECT id, CASE "
        "WHEN lower(CAST(role AS TEXT)) IN ('1', 'admin') THEN 1 "
        "WHEN lower(CAST(role AS TEXT)) IN ('2', 'patient') THEN 2 "
        "ELSE 0 END "
        "FROM users WHERE username = ? AND password = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *user_id = sqlite3_column_int(stmt, 0);
        role = sqlite3_column_int(stmt, 1);
        printf("Login successful! Welcome, %s.\n", username);
    } else {
        printf("Invalid username or password.\n");
    }

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return role;
}
