#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Function prototypes
void manage_suppliers();
void add_supplier();
void view_suppliers();
void update_supplier();
void delete_supplier(); // New function prototype

// Manage suppliers (menu options)
void manage_suppliers() {
    int choice;
    do {
        printf("\n=== Manage Suppliers ===\n");
        printf("1. Add Supplier\n");
        printf("2. View Suppliers\n");
        printf("3. Update Supplier\n");
        printf("4. Delete Supplier\n"); // Added menu option
        printf("5. Back to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_supplier();
                break;
            case 2:
                view_suppliers();
                break;
            case 3:
                update_supplier();
                break;
            case 4:
                delete_supplier();
                break;
            case 5:
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);
}

// Add a new supplier record
void add_supplier() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected line
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    char name[100], contact[50], address[150];
    printf("Enter supplier name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter supplier contact: ");
    scanf(" %[^\n]%*c", contact);
    printf("Enter supplier address: ");
    scanf(" %[^\n]%*c", address);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO suppliers (name, contact, address) VALUES (?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contact, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, address, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to insert supplier record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Supplier added successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// View all suppliers
void view_suppliers() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected line
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT supplier_id, name, contact, address FROM suppliers;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    printf("\nSuppliers List:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int supplier_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *contact = (const char *)sqlite3_column_text(stmt, 2);
        const char *address = (const char *)sqlite3_column_text(stmt, 3);

        printf("Supplier ID: %d, Name: %s, Contact: %s, Address: %s\n",
               supplier_id, name, contact, address);
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Update supplier details
void update_supplier() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected line
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    int supplier_id;
    char name[100], contact[50], address[150];
    printf("Enter supplier ID to update: ");
    scanf("%d", &supplier_id);
    printf("Enter new supplier name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter new supplier contact: ");
    scanf(" %[^\n]%*c", contact);
    printf("Enter new supplier address: ");
    scanf(" %[^\n]%*c", address);

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE suppliers SET name = ?, contact = ?, address = ? WHERE supplier_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contact, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, address, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, supplier_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to update supplier record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Supplier updated successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Delete supplier
void delete_supplier() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected line
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    int supplier_id;
    printf("Enter supplier ID to delete: ");
    scanf("%d", &supplier_id);

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM suppliers WHERE supplier_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, supplier_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to delete supplier record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Supplier deleted successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}
