#ifndef MEDICINES_H
#define MEDICINES_H

// Function prototypes for managing medicines
void add_medicine();
void view_medicines();  // Corrected here to match function name
void update_medicine();
void delete_medicine();
void buy_medicine(int authenticated_user_id, int allow_user_selection);

#endif
