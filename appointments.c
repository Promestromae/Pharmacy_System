#include <stdio.h>
#include "appointments.h"
#include "input.h"
#include "sqlite3.h"
#include "database.h"

static int list_patients(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "SELECT id, name FROM patients ORDER BY name;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load patients: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int count = 0;
    printf("\nPatients:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("ID: %d, Name: %s\n",
               sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 1));
        count++;
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        printf("Failed to load patients: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    if (count == 0) {
        printf("No patient records are available. Add a patient before booking.\n");
        return 0;
    }
    return 1;
}

void manage_appointments() {
    int choice;
    do {
        printf("\n=== Manage Appointments ===\n");
        printf("1. Book Appointment\n");
        printf("2. View Appointments\n");
        printf("3. Cancel Appointment\n");
        printf("4. Back to Admin Menu\n");
        if (!read_integer("Enter your choice: ", &choice)) {
            return;
        }

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

void book_appointment() {
    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }
    if (!list_patients(db)) {
        close_database(db);
        return;
    }

    int patient_id;
    if (!read_integer("Enter patient ID: ", &patient_id) || patient_id <= 0) {
        printf("A valid patient ID is required.\n");
        close_database(db);
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, "SELECT 1 FROM patients WHERE id = ?;",
                                -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to validate patient: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }
    sqlite3_bind_int(stmt, 1, patient_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_ROW) {
        printf("Patient ID %d was not found.\n", patient_id);
        close_database(db);
        return;
    }

    char appointment_date[32];
    char appointment_time[16];
    char reason[256];
    if (!read_line("Enter appointment date (YYYY-MM-DD): ",
                   appointment_date, sizeof(appointment_date)) ||
        !read_line("Enter appointment time (HH:MM): ",
                   appointment_time, sizeof(appointment_time)) ||
        !read_line("Enter appointment reason: ", reason, sizeof(reason))) {
        close_database(db);
        return;
    }
    if (appointment_date[0] == '\0' || appointment_time[0] == '\0' ||
        reason[0] == '\0') {
        printf("Date, time, and reason are required.\n");
        close_database(db);
        return;
    }

    rc = sqlite3_prepare_v2(db,
        "INSERT INTO appointments (patient_id, date, time, reason) "
        "VALUES (?, ?, ?, ?);",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare appointment: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, patient_id);
    sqlite3_bind_text(stmt, 2, appointment_date, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, appointment_time, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, reason, -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to book appointment: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Appointment booked successfully. Appointment ID: %d\n",
               (int)sqlite3_last_insert_rowid(db));
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

void view_appointments() {
    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT a.id, a.patient_id, COALESCE(p.name, 'Unknown patient'), "
        "a.date, a.time, COALESCE(a.reason, ''), COALESCE(s.name, '') "
        "FROM appointments a "
        "LEFT JOIN patients p ON p.id = a.patient_id "
        "LEFT JOIN staff s ON s.id = a.doctor_id "
        "ORDER BY a.date, a.time, a.id;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to load appointments: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    int count = 0;
    printf("\nAppointments:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("Appointment ID: %d, Patient: %s (ID: %d), Date: %s, "
               "Time: %s, Reason: %s",
               sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 2),
               sqlite3_column_int(stmt, 1),
               (const char *)sqlite3_column_text(stmt, 3),
               (const char *)sqlite3_column_text(stmt, 4),
               (const char *)sqlite3_column_text(stmt, 5));
        const char *doctor = (const char *)sqlite3_column_text(stmt, 6);
        if (doctor && doctor[0] != '\0') {
            printf(", Doctor: %s", doctor);
        }
        printf("\n");
        count++;
    }

    if (rc != SQLITE_DONE) {
        printf("Failed while reading appointments: %s\n", sqlite3_errmsg(db));
    } else if (count == 0) {
        printf("No appointments found.\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}

void cancel_appointment() {
    sqlite3 *db;
    if (!open_database(&db, "pharmacy.db")) {
        return;
    }

    int appointment_id;
    if (!read_integer("Enter appointment ID to cancel: ", &appointment_id) ||
        appointment_id <= 0) {
        printf("A valid appointment ID is required.\n");
        close_database(db);
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,
        "DELETE FROM appointments WHERE id = ?;",
        -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare cancellation: %s\n", sqlite3_errmsg(db));
        close_database(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, appointment_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to cancel appointment: %s\n", sqlite3_errmsg(db));
    } else if (sqlite3_changes(db) == 0) {
        printf("Appointment ID %d was not found.\n", appointment_id);
    } else {
        printf("Appointment cancelled successfully.\n");
    }

    sqlite3_finalize(stmt);
    close_database(db);
}
