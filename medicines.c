#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "sqlite3.h"
#include "database.h"

// Function prototypes
void manage_medicines();
void add_medicine();
void view_medicines();
void update_medicine();
void delete_medicine();
void buy_medicine(int authenticated_user_id, int allow_user_selection);

// Manage medicines (menu options)
void manage_medicines() {
    int choice;
    do {
        printf("\n=== Manage Medicines ===\n");
        printf("1. Add Medicine\n");
        printf("2. View Medicines\n");
        printf("3. Update Medicine\n");
        printf("4. Delete Medicine\n");
        printf("5. Buy Medicine\n");
        printf("6. Back to Admin Menu\n");
        if (!read_integer("Enter your choice: ", &choice)) {
            return;
        }

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
                buy_medicine(0, 1);
                break;
            case 6:
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);
}

void add_medicine() {
    char name[100];
    char description[256];
    double price;
    int stock;
    if (!read_line("Enter medicine name: ", name, sizeof(name)) ||
        !read_line("Enter description (optional): ", description, sizeof(description)) ||
        !read_double("Enter medicine price: ", &price) ||
        !read_integer("Enter medicine stock quantity: ", &stock)) {
        return;
    }
    if (name[0] == '\0' || price <= 0 || stock < 0) {
        printf("A medicine name, positive price, and non-negative stock are required.\n");
        return;
    }

    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) return;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "INSERT INTO medicines (name, description, price, stock) VALUES (?, ?, ?, ?);",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare medicine record: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    if (description[0] == '\0') sqlite3_bind_null(stmt, 2);
    else sqlite3_bind_text(stmt, 2, description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, price);
    sqlite3_bind_int(stmt, 4, stock);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to add medicine: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Medicine added successfully. Medicine ID: %d\n",
               (int)sqlite3_last_insert_rowid(db));
    }
    sqlite3_finalize(stmt);
    close_database(db);
}

void view_medicines() {
    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) return;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "SELECT id, name, COALESCE(description, ''), price, stock "
        "FROM medicines ORDER BY name, id;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load medicines: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    printf("\nMedicine Records:\n");
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("ID: %d, Name: %s, Description: %s, Price: %.2f, Stock: %d\n",
               sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 1),
               (const char *)sqlite3_column_text(stmt, 2),
               sqlite3_column_double(stmt, 3),
               sqlite3_column_int(stmt, 4));
        count++;
    }
    if (rc != SQLITE_DONE) {
        printf("Failed while reading medicines: %s\n", sqlite3_errmsg(db));
    } else if (count == 0) {
        printf("No medicines found.\n");
    }
    sqlite3_finalize(stmt);
    close_database(db);
}

void update_medicine() {
    int medicine_id;
    if (!read_integer("Enter medicine ID to update: ", &medicine_id) || medicine_id <= 0) {
        printf("A valid medicine ID is required.\n");
        return;
    }

    char name[100];
    char description[256];
    double price;
    int stock;
    if (!read_line("Enter new medicine name: ", name, sizeof(name)) ||
        !read_line("Enter new description (optional): ", description, sizeof(description)) ||
        !read_double("Enter new medicine price: ", &price) ||
        !read_integer("Enter new medicine stock quantity: ", &stock)) {
        return;
    }
    if (name[0] == '\0' || price <= 0 || stock < 0) {
        printf("A medicine name, positive price, and non-negative stock are required.\n");
        return;
    }

    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) return;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "UPDATE medicines SET name = ?, description = ?, price = ?, stock = ? WHERE id = ?;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare medicine update: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    if (description[0] == '\0') sqlite3_bind_null(stmt, 2);
    else sqlite3_bind_text(stmt, 2, description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, price);
    sqlite3_bind_int(stmt, 4, stock);
    sqlite3_bind_int(stmt, 5, medicine_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to update medicine: %s\n", sqlite3_errmsg(db));
    } else if (sqlite3_changes(db) == 0) {
        printf("Medicine ID %d was not found.\n", medicine_id);
    } else {
        printf("Medicine updated successfully.\n");
    }
    sqlite3_finalize(stmt);
    close_database(db);
}

void delete_medicine() {
    int medicine_id;
    if (!read_integer("Enter medicine ID to delete: ", &medicine_id) || medicine_id <= 0) {
        printf("A valid medicine ID is required.\n");
        return;
    }

    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) return;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, "DELETE FROM medicines WHERE id = ?;",
                                -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare medicine deletion: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, medicine_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to delete medicine: %s\n", sqlite3_errmsg(db));
    } else if (sqlite3_changes(db) == 0) {
        printf("Medicine ID %d was not found.\n", medicine_id);
    } else {
        printf("Medicine deleted successfully.\n");
    }
    sqlite3_finalize(stmt);
    close_database(db);
}

static int select_patient_user(sqlite3 *db, int *user_id) {
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id, username FROM users "
        "WHERE lower(CAST(role AS TEXT)) IN ('2', 'patient') ORDER BY username;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load users: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int user_count = 0;
    printf("\nUsers:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("ID: %d, Username: %s\n",
               sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 1));
        user_count++;
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        printf("Failed to load users: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    if (user_count == 0) {
        printf("No patient users are available.\n");
        return 0;
    }

    if (!read_integer("Enter user ID for this purchase: ", user_id)) {
        return 0;
    }

    rc = sqlite3_prepare_v2(db,
        "SELECT 1 FROM users WHERE id = ? "
        "AND lower(CAST(role AS TEXT)) IN ('2', 'patient');",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to validate user: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, *user_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_ROW) {
        printf("That ID does not belong to a patient user.\n");
        return 0;
    }
    return 1;
}

static int list_available_medicines(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "SELECT id, name, price, stock FROM medicines ORDER BY name;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load medicines: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int medicine_count = 0;
    printf("\nAvailable Medicines:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("ID: %d, Name: %s, Price: %.2f, Stock: %d\n",
               sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 1),
               sqlite3_column_double(stmt, 2),
               sqlite3_column_int(stmt, 3));
        medicine_count++;
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        printf("Failed to load medicines: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    if (medicine_count == 0) {
        printf("No medicines are available.\n");
        return 0;
    }
    return 1;
}

void buy_medicine(int authenticated_user_id, int allow_user_selection) {
    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    if (!ensure_sales_user_column(db)) {
        close_database(db);
        return;
    }

    int user_id = authenticated_user_id;
    if (allow_user_selection && !select_patient_user(db, &user_id)) {
        close_database(db);
        return;
    }
    if (user_id <= 0) {
        fprintf(stderr, "Cannot record purchase without an authenticated user.\n");
        close_database(db);
        return;
    }

    if (!list_available_medicines(db)) {
        close_database(db);
        return;
    }

    int medicine_id;
    int quantity;
    if (!read_integer("Enter medicine ID to buy: ", &medicine_id) ||
        !read_integer("Enter quantity to buy: ", &quantity)) {
        close_database(db);
        return;
    }
    if (medicine_id <= 0 || quantity <= 0) {
        printf("Medicine ID and quantity must be greater than zero.\n");
        close_database(db);
        return;
    }

    int rc = sqlite3_exec(db, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        printf("Could not start purchase: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db,
        "SELECT name, price, stock FROM medicines WHERE id = ?;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load medicine: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }
    sqlite3_bind_int(stmt, 1, medicine_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        printf("Medicine ID %d was not found.\n", medicine_id);
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }

    char medicine_name[256];
    snprintf(medicine_name, sizeof(medicine_name), "%s",
             (const char *)sqlite3_column_text(stmt, 0));
    double price = sqlite3_column_double(stmt, 1);
    int stock = sqlite3_column_int(stmt, 2);
    sqlite3_finalize(stmt);

    if (quantity > stock) {
        printf("Insufficient stock. Available quantity: %d\n", stock);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }

    rc = sqlite3_prepare_v2(db,
        "UPDATE medicines SET stock = stock - ? WHERE id = ? AND stock >= ?;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to update inventory: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }
    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_int(stmt, 2, medicine_id);
    sqlite3_bind_int(stmt, 3, quantity);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE || sqlite3_changes(db) != 1) {
        printf("Could not update medicine stock: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }

    rc = sqlite3_prepare_v2(db,
        "INSERT INTO sales (user_id, medicine_id, quantity, total_price, date) "
        "VALUES (?, ?, ?, ?, date('now', 'localtime'));",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare purchase record: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, medicine_id);
    sqlite3_bind_int(stmt, 3, quantity);
    sqlite3_bind_double(stmt, 4, price * quantity);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to save purchase: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }

    rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to complete purchase: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        close_database(db);
        return;
    }

    printf("Purchase recorded for user ID %d: %s x%d, total %.2f.\n",
           user_id, medicine_name, quantity, price * quantity);
    close_database(db);
}
