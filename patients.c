#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Function prototypes
void manage_patients();
void add_patient();
void view_patient_record();  // Renamed function to match the header
void update_patient();
void delete_patient();

// Manage patient records (menu options)
void manage_patients() {
    int choice;
    do {
        printf("\n=== Manage Patients ===\n");
        printf("1. Add Patient\n");
        printf("2. View Patients\n");
        printf("3. Update Patient\n");
        printf("4. Delete Patient\n");
        printf("5. Back to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_patient();
                break;
            case 2:
                view_patient_record();  // Call updated function
                break;
            case 3:
                update_patient();
                break;
            case 4:
                delete_patient();
                break;
            case 5:
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);
}

// Add a new patient record
void add_patient() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Specify the database filename
    if (rc != SQLITE_OK) return;

    char name[100], address[200], phone[15];
    int age;

    printf("Enter patient's name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter patient's age: ");
    scanf("%d", &age);
    printf("Enter patient's address: ");
    scanf(" %[^\n]%*c", address);
    printf("Enter patient's phone: ");
    scanf(" %[^\n]%*c", phone);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO patients (name, age, address, phone) VALUES (?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, age);
    sqlite3_bind_text(stmt, 3, address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, phone, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to insert patient record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Patient added successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// View all patients' records
void view_patient_record() {  // Updated function name
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Specify the database filename
    if (rc != SQLITE_OK) return;

    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM patients;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    printf("\nPatient Records:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int patient_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        const char *address = (const char *)sqlite3_column_text(stmt, 3);
        const char *phone = (const char *)sqlite3_column_text(stmt, 4);

        printf("ID: %d, Name: %s, Age: %d, Address: %s, Phone: %s\n", patient_id, name, age, address, phone);
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Update an existing patient record
void update_patient() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Specify the database filename
    if (rc != SQLITE_OK) return;

    int patient_id;
    char name[100], address[200], phone[15];
    int age;

    printf("Enter patient ID to update: ");
    scanf("%d", &patient_id);
    printf("Enter new patient's name: ");
    scanf(" %[^\n]%*c", name);
    printf("Enter new patient's age: ");
    scanf("%d", &age);
    printf("Enter new patient's address: ");
    scanf(" %[^\n]%*c", address);
    printf("Enter new patient's phone: ");
    scanf(" %[^\n]%*c", phone);

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE patients SET name = ?, age = ?, address = ?, phone = ? WHERE patient_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, age);
    sqlite3_bind_text(stmt, 3, address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, phone, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, patient_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to update patient record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Patient updated successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Delete a patient record
void delete_patient() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Specify the database filename
    if (rc != SQLITE_OK) return;

    int patient_id;
    printf("Enter patient ID to delete: ");
    scanf("%d", &patient_id);

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM patients WHERE patient_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, patient_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to delete patient record: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Patient deleted successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}
