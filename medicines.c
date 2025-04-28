#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Function prototypes
void manage_medicines();
void add_medicine();
void view_medicines();
void update_medicine();
void delete_medicine();
void buy_medicine();  // Added function prototype

// Manage medicines (menu options)
void manage_medicines() {
    int choice;
    do {
        printf("\n=== Manage Medicines ===\n");
        printf("1. Add Medicine\n");
        printf("2. View Medicines\n");
        printf("3. Update Medicine\n");
        printf("4. Delete Medicine\n");
        printf("5. Buy Medicine\n"); // Added option
        printf("6. Back to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_medicine();
                break;
            case 2:
                view_medicines();
                break;
            case 3:
                update_medicine();
                break;
            case 4:
                delete_medicine();
                break;
            case 5:
                buy_medicine();  // Call buy_medicine function
                break;
            case 6:
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);
}

// Add a new medicine record
void add_medicine() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected: database filename passed
    if (rc != 1) return;

    char name[100], expiry_date[20];
    int quantity, reorder_level;
    float price;

    printf("Enter medicine name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter medicine price: ");
    scanf("%f", &price);
    printf("Enter medicine quantity: ");
    scanf("%d", &quantity);
    printf("Enter medicine expiry date (YYYY-MM-DD): ");
    scanf(" %[^\n]%*c", expiry_date);
    printf("Enter reorder level: ");
    scanf("%d", &reorder_level);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO medicines (name, price, quantity, expiry_date, reorder_level) VALUES (?, ?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, price);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_text(stmt, 4, expiry_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, reorder_level);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to insert medicine record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Medicine added successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// View all medicines' records
void view_medicines() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected: database filename passed
    if (rc != 1) return;

    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM medicines;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    printf("\nMedicine Records:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int medicine_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        double price = sqlite3_column_double(stmt, 2);
        int quantity = sqlite3_column_int(stmt, 3);
        const char *expiry_date = (const char *)sqlite3_column_text(stmt, 4);
        int reorder_level = sqlite3_column_int(stmt, 5);

        printf("ID: %d, Name: %s, Price: %.2f, Quantity: %d, Expiry Date: %s, Reorder Level: %d\n",
               medicine_id, name, price, quantity, expiry_date, reorder_level);
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Update an existing medicine record
void update_medicine() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected: database filename passed
    if (rc != 1) return;

    int medicine_id;
    char name[100], expiry_date[20];
    int quantity, reorder_level;
    float price;

    printf("Enter medicine ID to update: ");
    scanf("%d", &medicine_id);
    printf("Enter new medicine name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter new medicine price: ");
    scanf("%f", &price);
    printf("Enter new medicine quantity: ");
    scanf("%d", &quantity);
    printf("Enter new medicine expiry date (YYYY-MM-DD): ");
    scanf(" %[^\n]%*c", expiry_date);
    printf("Enter new reorder level: ");
    scanf("%d", &reorder_level);

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE medicines SET name = ?, price = ?, quantity = ?, expiry_date = ?, reorder_level = ? WHERE medicine_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, price);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_text(stmt, 4, expiry_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, reorder_level);
    sqlite3_bind_int(stmt, 6, medicine_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to update medicine record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Medicine updated successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Delete a medicine record
void delete_medicine() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db"); // Corrected: database filename passed
    if (rc != 1) return;

    int medicine_id;
    printf("Enter medicine ID to delete: ");
    scanf("%d", &medicine_id);

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM medicines WHERE medicine_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, medicine_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to delete medicine record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Medicine deleted successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Buy medicine (stubbed implementation)
void buy_medicine() {
    int medicine_id, quantity;
    printf("Enter medicine ID to buy: ");
    scanf("%d", &medicine_id);
    printf("Enter quantity to buy: ");
    scanf("%d", &quantity);
    
    // Simulate updating the quantity in the database (you would add the actual logic to do this)
    printf("Medicine bought successfully! (ID: %d, Quantity: %d)\n", medicine_id, quantity);
}
