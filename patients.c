#include <stdio.h>
#include "patients.h"
#include "input.h"
#include "sqlite3.h"
#include "database.h"

static void bind_optional_text(sqlite3_stmt *stmt, int index, const char *value) {
    if (value[0] == '\0') {
        sqlite3_bind_null(stmt, index);
    } else {
        sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
    }
}

void manage_patients() {
    int choice;
    do {
        printf("\n=== Manage Patients ===\n");
        printf("1. Add Patient\n");
        printf("2. View Patients\n");
        printf("3. Update Patient\n");
        printf("4. Delete Patient\n");
        printf("5. Back to Admin Menu\n");
        if (!read_integer("Enter your choice: ", &choice)) {
            return;
        }

        switch (choice) {
            case 1:
                add_patient();
                break;
            case 2:
                view_patient_record();
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

void add_patient() {
    char name[100];
    char gender[32];
    char contact[64];
    char address[200];
    int age;

    if (!read_line("Enter patient's name: ", name, sizeof(name)) ||
        !read_integer("Enter patient's age: ", &age) ||
        !read_line("Enter patient's gender (optional): ", gender, sizeof(gender)) ||
        !read_line("Enter patient's contact (optional): ", contact, sizeof(contact)) ||
        !read_line("Enter patient's address (optional): ", address, sizeof(address))) {
        return;
    }
    if (name[0] == '\0' || age <= 0) {
        printf("Patient name and a positive age are required.\n");
        return;
    }

    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "INSERT INTO patients (name, age, gender, contact, address) "
        "VALUES (?, ?, ?, ?, ?);",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare patient record: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, age);
    bind_optional_text(stmt, 3, gender);
    bind_optional_text(stmt, 4, contact);
    bind_optional_text(stmt, 5, address);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to add patient: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Patient added successfully. Patient ID: %d\n",
               (int)sqlite3_last_insert_rowid(db));
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

void view_patient_record() {
    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "SELECT id, name, age, COALESCE(gender, 'Not provided'), "
        "COALESCE(contact, 'Not provided'), "
        "COALESCE(address, 'Not provided') "
        "FROM patients ORDER BY id;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load patient records: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    int count = 0;
    printf("\nPatient Records:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        char age[32];
        if (sqlite3_column_type(stmt, 2) == SQLITE_NULL) {
            snprintf(age, sizeof(age), "Not provided");
        } else {
            snprintf(age, sizeof(age), "%d", sqlite3_column_int(stmt, 2));
        }
        printf("ID: %d, Name: %s, Age: %s, Gender: %s, Phone: %s, Address: %s\n",
               sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 1),
               age,
               (const char *)sqlite3_column_text(stmt, 3),
               (const char *)sqlite3_column_text(stmt, 4),
               (const char *)sqlite3_column_text(stmt, 5));
        count++;
    }

    if (rc != SQLITE_DONE) {
        printf("Failed while reading patient records: %s\n", sqlite3_errmsg(db));
    } else if (count == 0) {
        printf("No patient records found.\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

void update_patient() {
    int patient_id;
    if (!read_integer("Enter patient ID to update: ", &patient_id) || patient_id <= 0) {
        printf("A valid patient ID is required.\n");
        return;
    }

    char name[100];
    char gender[32];
    char contact[64];
    char address[200];
    int age;
    if (!read_line("Enter new patient's name: ", name, sizeof(name)) ||
        !read_integer("Enter new patient's age: ", &age) ||
        !read_line("Enter new patient's gender (optional): ", gender, sizeof(gender)) ||
        !read_line("Enter new patient's contact (optional): ", contact, sizeof(contact)) ||
        !read_line("Enter new patient's address (optional): ", address, sizeof(address))) {
        return;
    }
    if (name[0] == '\0' || age <= 0) {
        printf("Patient name and a positive age are required.\n");
        return;
    }

    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "UPDATE patients SET name = ?, age = ?, gender = ?, contact = ?, address = ? "
        "WHERE id = ?;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare patient update: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, age);
    bind_optional_text(stmt, 3, gender);
    bind_optional_text(stmt, 4, contact);
    bind_optional_text(stmt, 5, address);
    sqlite3_bind_int(stmt, 6, patient_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to update patient: %s\n", sqlite3_errmsg(db));
    } else if (sqlite3_changes(db) == 0) {
        printf("Patient ID %d was not found.\n", patient_id);
    } else {
        printf("Patient updated successfully.\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

void delete_patient() {
    int patient_id;
    if (!read_integer("Enter patient ID to delete: ", &patient_id) || patient_id <= 0) {
        printf("A valid patient ID is required.\n");
        return;
    }

    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, "DELETE FROM patients WHERE id = ?;",
                                -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare patient deletion: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, patient_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to delete patient: %s\n", sqlite3_errmsg(db));
    } else if (sqlite3_changes(db) == 0) {
        printf("Patient ID %d was not found.\n", patient_id);
    } else {
        printf("Patient deleted successfully.\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}
