#include <stdio.h>
#include <sqlite3.h>  // Add this for SQLite3 functions
#include "database.h"  // Include the database functions

// Manage a new staff record
void manage_staff() {  // Renamed function from add_staff to manage_staff
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Use the open_database function from database.c
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    char name[100], role[50], contact[50];
    printf("Enter staff name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter staff role (e.g., pharmacist, cashier): ");
    scanf(" %[^\n]%*c", role);
    printf("Enter staff contact: ");
    scanf(" %[^\n]%*c", contact);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO staff (name, role, contact) VALUES (?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, contact, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to insert staff record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Staff added successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);  // Use the close_database function from database.c
}

// View all staff members
void view_staff() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Use the open_database function from database.c
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT staff_id, name, role, contact FROM staff;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    printf("\nStaff List:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int staff_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *role = (const char *)sqlite3_column_text(stmt, 2);
        const char *contact = (const char *)sqlite3_column_text(stmt, 3);

        printf("Staff ID: %d, Name: %s, Role: %s, Contact: %s\n",
               staff_id, name, role, contact);
    }

    sqlite3_finalize(stmt);
    close_database(db);  // Use the close_database function from database.c
}

// Update staff details
void update_staff() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Use the open_database function from database.c
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    int staff_id;
    char name[100], role[50], contact[50];
    printf("Enter staff ID to update: ");
    scanf("%d", &staff_id);
    printf("Enter new staff name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter new staff role: ");
    scanf(" %[^\n]%*c", role);
    printf("Enter new staff contact: ");
    scanf(" %[^\n]%*c", contact);

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE staff SET name = ?, role = ?, contact = ? WHERE staff_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, contact, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, staff_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to update staff record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Staff updated successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);  // Use the close_database function from database.c
}
