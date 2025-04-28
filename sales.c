#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Function prototypes
void manage_sales();
void add_sale();
void view_sales();
void delete_sale();

// Manage sales (menu options)
void manage_sales() {
    int choice;
    do {
        printf("\n=== Manage Sales ===\n");
        printf("1. Add Sale\n");
        printf("2. View Sales\n");
        printf("3. Delete Sale\n");
        printf("4. Back to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_sale();
                break;
            case 2:
                view_sales();
                break;
            case 3:
                delete_sale();
                break;
            case 4:
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}

// Add a new sale record
void add_sale() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Updated to pass filename
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    int patient_id, medicine_id, quantity;
    float total_price;
    char sale_date[20];

    printf("Enter patient ID: ");
    scanf("%d", &patient_id);
    printf("Enter medicine ID: ");
    scanf("%d", &medicine_id);
    printf("Enter quantity sold: ");
    scanf("%d", &quantity);
    printf("Enter total price: ");
    scanf("%f", &total_price);
    printf("Enter sale date (YYYY-MM-DD): ");
    scanf(" %[^\n]%*c", sale_date);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO sales (patient_id, medicine_id, quantity, total_price, sale_date) VALUES (?, ?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, patient_id);
    sqlite3_bind_int(stmt, 2, medicine_id);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_double(stmt, 4, total_price);
    sqlite3_bind_text(stmt, 5, sale_date, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to insert sale record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Sale added successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// View all sales records
void view_sales() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Updated to pass filename
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT s.sale_id, p.name, m.name, s.quantity, s.total_price, s.sale_date FROM sales s "
                      "JOIN patients p ON s.patient_id = p.patient_id "
                      "JOIN medicines m ON s.medicine_id = m.medicine_id;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    printf("\nSales Records:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int sale_id = sqlite3_column_int(stmt, 0);
        const char *patient_name = (const char *)sqlite3_column_text(stmt, 1);
        const char *medicine_name = (const char *)sqlite3_column_text(stmt, 2);
        int quantity = sqlite3_column_int(stmt, 3);
        double total_price = sqlite3_column_double(stmt, 4);
        const char *sale_date = (const char *)sqlite3_column_text(stmt, 5);

        printf("Sale ID: %d, Patient: %s, Medicine: %s, Quantity: %d, Total Price: %.2f, Date: %s\n",
               sale_id, patient_name, medicine_name, quantity, total_price, sale_date);
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Delete a sale record
void delete_sale() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Updated to pass filename
    if (rc != SQLITE_OK) {
        printf("Failed to open database\n");
        return;
    }

    int sale_id;
    printf("Enter Sale ID to delete: ");
    scanf("%d", &sale_id);

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM sales WHERE sale_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, sale_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to delete sale record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Sale deleted successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}
