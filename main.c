#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "auth.h"
#include "patients.h"
#include "medicines.h"
#include "sales.h"
#include "appointments.h"
#include "suppliers.h"
#include "staff.h"

// Function prototypes
void admin_menu();
void patient_menu();

// Declare functions used in main.c
void manage_patients();
void manage_medicines();
void manage_appointments();
void view_sales();
void manage_suppliers();
void manage_staff();
void book_appointment();
void buy_medicine();
void view_patient_record();  // Ensure that this function is declared correctly

int main() {
    char username[100], password[100];
    
    // Prompt user for credentials
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Authenticate user and get role
    int role = authenticate_user(username, password); // Pass the arguments
    if (role == 1) {
        admin_menu();
    } else if (role == 2) {
        patient_menu();
    } else {
        printf("Invalid credentials. Exiting system.\n");
        return 0;
    }
    return 0;
}

void admin_menu() {
    int choice;
    do {
        printf("\n=== Admin Menu ===\n");
        printf("1. Manage Patients\n");
        printf("2. Manage Medicines\n");
        printf("3. Manage Appointments\n");
        printf("4. View Sales Records\n");
        printf("5. Manage Suppliers\n");
        printf("6. Manage Staff\n");
        printf("7. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                manage_patients();
                break;
            case 2:
                manage_medicines();
                break;
            case 3:
                manage_appointments();
                break;
            case 4:
                view_sales();
                break;
            case 5:
                manage_suppliers();
                break;
            case 6:
                manage_staff();
                break;
            case 7:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 7);
}

void patient_menu() {
    int choice;
    do {
        printf("\n=== Patient Menu ===\n");
        printf("1. Book Appointment\n");
        printf("2. Buy Medicine\n");
        printf("3. View Your Record\n");
        printf("4. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                book_appointment();
                break;
            case 2:
                buy_medicine();
                break;
            case 3:
                view_patient_record();  // This calls the correct function
                break;
            case 4:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}

