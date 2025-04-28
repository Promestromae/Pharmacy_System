#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

// Function to open a database connection
int open_database(sqlite3 **db, const char *filename) {
    int rc = sqlite3_open(filename, db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return 0;  // Return 0 for failure
    }
    return 1;  // Return 1 for success
}

// Function to close the database connection
void close_database(sqlite3 *db) {
    sqlite3_close(db);
}

// Function to create the necessary tables
int create_tables(sqlite3 *db) {
    const char *create_patients = "CREATE TABLE IF NOT EXISTS patients ("
                                  "patient_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "name TEXT NOT NULL, "
                                  "age INTEGER NOT NULL, "
                                  "contact TEXT NOT NULL);";

    const char *create_medicines = "CREATE TABLE IF NOT EXISTS medicines ("
                                   "medicine_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "name TEXT NOT NULL, "
                                   "price REAL NOT NULL, "
                                   "stock INTEGER NOT NULL);";

    const char *create_sales = "CREATE TABLE IF NOT EXISTS sales ("
                               "sale_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "medicine_id INTEGER NOT NULL, "
                               "quantity INTEGER NOT NULL, "
                               "total_price REAL NOT NULL, "
                               "date TEXT NOT NULL, "
                               "FOREIGN KEY(medicine_id) REFERENCES medicines(medicine_id));";

    const char *create_appointments = "CREATE TABLE IF NOT EXISTS appointments ("
                                      "appointment_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                      "patient_id INTEGER NOT NULL, "
                                      "date TEXT NOT NULL, "
                                      "doctor TEXT NOT NULL, "
                                      "FOREIGN KEY(patient_id) REFERENCES patients(patient_id));";

    const char *create_suppliers = "CREATE TABLE IF NOT EXISTS suppliers ("
                                   "supplier_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "name TEXT NOT NULL, "
                                   "contact TEXT NOT NULL);";

    char *error_message = 0;
    int rc;

    // Execute table creation commands
    rc = sqlite3_exec(db, create_patients, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        return 0;
    }

    rc = sqlite3_exec(db, create_medicines, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        return 0;
    }

    rc = sqlite3_exec(db, create_sales, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        return 0;
    }

    rc = sqlite3_exec(db, create_appointments, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        return 0;
    }

    rc = sqlite3_exec(db, create_suppliers, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        return 0;
    }

    return 1;  // Return 1 for success
}

// Insert a new patient
int insert_patient(sqlite3 *db, const char *name, int age, const char *contact) {
    const char *sql = "INSERT INTO patients (name, age, contact) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;  // Return 0 for failure
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, age);
    sqlite3_bind_text(stmt, 3, contact, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert patient: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;  // Return 0 for failure
    }

    sqlite3_finalize(stmt);
    return 1;  // Return 1 for success
}

// Fetch all patients
int fetch_all_patients(sqlite3 *db) {
    const char *sql = "SELECT * FROM patients;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;  // Return 0 for failure
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int patient_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        const char *contact = (const char *)sqlite3_column_text(stmt, 3);

        printf("ID: %d, Name: %s, Age: %d, Contact: %s\n", patient_id, name, age, contact);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to fetch patients: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;  // Return 0 for failure
    }

    sqlite3_finalize(stmt);
    return 1;  // Return 1 for success
}

// Insert a new medicine
int insert_medicine(sqlite3 *db, const char *name, double price, int stock) {
    const char *sql = "INSERT INTO medicines (name, price, stock) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;  // Return 0 for failure
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, price);
    sqlite3_bind_int(stmt, 3, stock);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert medicine: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;  // Return 0 for failure
    }

    sqlite3_finalize(stmt);
    return 1;  // Return 1 for success
}

// Fetch all medicines
int fetch_all_medicines(sqlite3 *db) {
    const char *sql = "SELECT * FROM medicines;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;  // Return 0 for failure
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int medicine_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);

        printf("ID: %d, Name: %s, Price: %.2f, Stock: %d\n", medicine_id, name, price, stock);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to fetch medicines: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;  // Return 0 for failure
    }

    sqlite3_finalize(stmt);
    return 1;  // Return 1 for success
}

// Add more CRUD functions for sales, appointments, suppliers, etc.
