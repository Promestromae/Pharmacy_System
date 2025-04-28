#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"

// Function prototype
int authenticate_user();

int authenticate_user() {
    char username[50], password[50];
    int role = 0; // 1 = Admin, 2 = Patient, 0 = Invalid

    printf("\n=== Login ===\n");
    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    // Open database connection
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("pharmacy.db", &db);

    if (rc != SQLITE_OK) {
        printf("Error: Cannot connect to database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Query to validate user credentials
    const char *sql = "SELECT role FROM users WHERE username = ? AND password = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        role = sqlite3_column_int(stmt, 0); // Get role (1 = Admin, 2 = Patient)
        printf("Login successful! Welcome, %s.\n", username);
    } else {
        printf("Invalid username or password.\n");
    }

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return role;
}
