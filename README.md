# Pharmacy Management System

A C-based pharmacy management system designed to help manage patient records, medicine inventory, sales, appointments, suppliers, and staff operations using SQLite as the database backend.

## Project overview
This project is a practical desktop-style application that demonstrates structured C programming with modular files and a persistent SQLite database. It is suitable for learning database-driven application development and for small-scale pharmacy operations.

## What the system includes
- Patient management
  - Add, view, update, and delete patient records
- Medicine inventory management
  - Track medicine names, stock, and pricing
- Sales tracking
  - Record medicine purchases against a user account, update stock, and compute totals
- Appointment management
  - Schedule patient appointments with doctors
- Supplier management
  - Maintain supplier contact details and records
- Staff management
  - Manage staff-related data through the admin menu
- Authentication
  - Role-based login for admin and patient users

## Technologies used
- C programming language
- SQLite3 database
- Standard C libraries
- GCC / MinGW compatible compiler

## Project structure
- `main.c` — entry point and menu flow
- `input.h` — shared validation for numeric menu and purchase input
- `auth.c` / `auth.h` — authentication logic
- `database.c` / `database.h` — database setup and CRUD helpers
- `patients.c` / `patients.h` — patient management
- `medicines.c` / `medicines.h` — medicine operations
- `sales.c` / `sales.h` — sales handling
- `appointments.c` / `appointments.h` — appointment functions
- `suppliers.c` / `suppliers.h` — supplier data
- `staff.c` / `staff.h` — staff records
- `sqlite3.c` / `sqlite3.h` — SQLite implementation
- `pharmacy.db` — SQLite database file

## Evaluation
This is a well-structured beginner-to-intermediate project. It shows solid use of modular C code and database integration, especially for educational or prototype-level healthcare management systems. The project is easy to extend and can be improved further with:
- better validation and input handling
- more secure authentication
- clearer reporting and data exports
- richer admin dashboards
- improved error handling and user feedback

## Prerequisites
Before running the project, make sure you have:
- GCC or MinGW installed and available in PATH
- SQLite3 development support available
- A Windows command prompt, PowerShell, or terminal

## Commands to execute

### 1) Build the project with GCC
Open PowerShell or Command Prompt in the project folder and run:

```powershell
cd C:\Users\Administrator\Pharmacy_System-1
gcc -Wall -std=c99 main.c patients.c medicines.c sales.c appointments.c suppliers.c staff.c auth.c database.c sqlite3.c -o pharmacy_system.exe
```

### 2) Run the application
```powershell
cd C:\Users\Administrator\Pharmacy_System-1
.\pharmacy_system.exe
```

### 3) If you prefer the Makefile
The project includes a `Makefile.txt` file. Use:

```powershell
cd C:\Users\Administrator\Pharmacy_System-1
mingw32-make -f Makefile.txt
mingw32-make -f Makefile.txt run
```

If `mingw32-make` is not available, you can compile manually using the GCC command above.

## Notes
- The application uses `pharmacy.db` as its default SQLite database file.
- Make sure the database file is present in the same folder as the executable.
- Login credentials depend on the contents of the `users` table in the SQLite database; add or update records if needed before first use.
- Patient purchases are associated with the logged-in user. Admin medicine purchases prompt for a patient account.
- The first recorded purchase adds a `user_id` column to the existing `sales` table; earlier sales remain visible as legacy records without an associated user.

## Example workflow
1. Start the program.
2. Enter login credentials.
3. Choose the admin or patient menu based on the assigned role.
4. Admins can choose a patient account when recording a medicine purchase; patient logins are linked automatically.
5. Purchase records, totals, dates, and inventory changes are saved in `pharmacy.db`.

## License
This project is intended for educational and demonstration use.
