#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Function prototypes
void manage_appointments();
void book_appointment();
void view_appointments();
void cancel_appointment();

// Manage appointments (menu options)
void manage_appointments() {
    int choice;
    do {
        printf("\n=== Manage Appointments ===\n");
        printf("1. Book Appointment\n");
        printf("2. View Appointments\n");
        printf("3. Cancel Appointment\n");
        printf("4. Back to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                book_appointment();
                break;
            case 2:
                view_appointments();
                break;
            case 3:
                cancel_appointment();
                break;
            case 4:
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}

// Book a new appointment record
void book_appointment() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Pass database filename here
    if (rc != 1) return;

    int patient_id;
    char appointment_date[20];

    printf("Enter patient ID: ");
    scanf("%d", &patient_id);
    printf("Enter appointment date (YYYY-MM-DD): ");
    scanf(" %[^\n]%*c", appointment_date);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO appointments (patient_id, appointment_date) VALUES (?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, patient_id);
    sqlite3_bind_text(stmt, 2, appointment_date, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Error inserting appointment: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Appointment booked successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// View all appointments
void view_appointments() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Pass database filename here
    if (rc != 1) return;

    sqlite3_stmt *stmt;
    const char *sql = "SELECT a.appointment_id, p.name, a.appointment_date FROM appointments a "
                      "JOIN patients p ON a.patient_id = p.patient_id;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    printf("\nAppointments:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int appointment_id = sqlite3_column_int(stmt, 0);
        const char *patient_name = (const char *)sqlite3_column_text(stmt, 1);
        const char *appointment_date = (const char *)sqlite3_column_text(stmt, 2);

        printf("Appointment ID: %d, Patient: %s, Date: %s\n", appointment_id, patient_name, appointment_date);
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

// Cancel an appointment
void cancel_appointment() {
    sqlite3 *db;
    int rc = open_database(&db, "pharmacy.db");  // Pass database filename here
    if (rc != 1) return;

    int appointment_id;
    printf("Enter appointment ID to cancel: ");
    scanf("%d", &appointment_id);

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM appointments WHERE appointment_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, appointment_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Error cancelling appointment: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Appointment cancelled successfully!\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}
