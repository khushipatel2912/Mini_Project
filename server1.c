#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#define TRANSACTION_LOG "transactions.log"
#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_USERS 100

// Structure to hold client data
typedef struct {
    int socket;
    char username[50];
    char role[20];
} client_data;

// Structure to hold user credentials
typedef struct {
    char username[50];
    char password[50];
    char role[20]; // Role of the user
    float balance; // User account balance
} user;

// User database
user user_db[MAX_USERS];
int user_count = 0;


#include <stdio.h>
#include <string.h>

int check_user_exists(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;  // File doesn't exist, assume user doesn't exist
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character from the line
        line[strcspn(line, "\n")] = 0;
        
        // Compare the line with the provided username
        if (strcmp(line, username) == 0) {
            fclose(file);
            return 1;  // User exists
        }
    }

    fclose(file);
    return 0;  // User does not exist
}


// Function to load user credentials from a file
void load_users(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open user database file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%49[^:]:%49[^:]:%19s\n", user_db[user_count].username, user_db[user_count].password, user_db[user_count].role) == 3) {
        user_count++;
    }
    fclose(file);
}

void save_users(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open user database file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s:%s:%s\n", user_db[i].username, user_db[i].password, user_db[i].role);
    }
    fclose(file);
}

// Function to check user credentials
int check_credentials(const char *username, const char *password, char *role) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(username, user_db[i].username) == 0 &&
            strcmp(password, user_db[i].password) == 0 && 
            strcmp(role, user_db[i].role) == 0)
            return 1; // Valid user
        }
    
    return 0; // Invalid user
}


void add_user(const char *username, const char *password, const char *role) {
    if (user_count < MAX_USERS) {
        strcpy(user_db[user_count].username, username);
        strcpy(user_db[user_count].password, password);
        strcpy(user_db[user_count].role, role);
        user_count++;
        save_users("users.txt"); 
    }
}

void modify_user(const char *username, const char *new_username, const char *new_password, const char *new_role) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0) {
            if (new_username != NULL) {
                strcpy(user_db[i].username, new_username);
            }
            if (new_password != NULL) {
                strcpy(user_db[i].password, new_password);
            }
            if (new_role != NULL) {
                strcpy(user_db[i].role, new_role);
            }
            break;
        }
        save_users("users.txt");
    }
    
}

void change_password(const char *username, const char *new_password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0) {
            strcpy(user_db[i].password, new_password);
            break;
        }
    }
}


void delete_user(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0) {
            for (int j = i; j < user_count - 1; j++) {
                user_db[j] = user_db[j + 1];
            }
            user_count--;
            break;
        }
    }
}

void manage_user_role(const char *username, const char *new_role) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0) {
            strcpy(user_db[i].role, new_role);
            break;
        }
    }
}


void list_users(int sock) {
    char response[BUFFER_SIZE] = "User List:\n";
    for (int i = 0; i < user_count; i++) {
        strcat(response, user_db[i].username);
        strcat(response, " (");
        strcat(response, user_db[i].role);
        strcat(response, ")\n");
    }
    send(sock, response, strlen(response) + 1, 0);
}

void log_admin_action(const char *action) {
    FILE *log_file = fopen("admin_actions.log", "a");
    if (log_file != NULL) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] %s\n", ctime(&now), action);
        fclose(log_file);
    }
}


void logout_user(int sock) {
    const char *msg = "Logout successful.";
    send(sock, msg, strlen(msg) + 1, 0);
}


// Function to activate or deactivate a customer account
void activate_deactivate_account(const char *username, int activate) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0 && strcmp(user_db[i].role, "customer") == 0) {
            if (activate) {
                strcpy(user_db[i].role, "active_customer"); // Activated account
            } else {
                strcpy(user_db[i].role, "deactivated_customer"); // Deactivated account
            }
            save_users("users.txt");
            break;
        }
    }
}

// Function to assign loan application process to an employee
void assign_loan_application(const char *employee_username) {
    // Simply logging the assignment for now
    FILE *log_file = fopen("loan_assignments.log", "a");
    if (log_file != NULL) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] Loan application assigned to employee: %s\n", ctime(&now), employee_username);
        fclose(log_file);
    }
}

// Function to review customer feedback
void review_feedback(int sock) {
    FILE *feedback_file = fopen("customer_feedback.txt", "r");
    char feedback[BUFFER_SIZE];
    if (feedback_file != NULL) {
        while (fgets(feedback, sizeof(feedback), feedback_file) != NULL) {
            send(sock, feedback, strlen(feedback) + 1, 0);
        }
        fclose(feedback_file);
    } else {
        const char *msg = "No feedback available.";
        send(sock, msg, strlen(msg) + 1, 0);
    }
}

// Function to add a new customer
void add_customer(int sock) {
    char cust_username[50], cust_password[50];
    send(sock, "Enter new customer username:", 29, 0);
    read(sock, cust_username, sizeof(cust_username));
    send(sock, "Enter new customer password:", 29, 0);
    read(sock, cust_password, sizeof(cust_password));
    add_user(cust_username, cust_password, "customer");
    save_users("users.txt");
    const char *msg = "Customer added successfully.";
    send(sock, msg, strlen(msg) + 1, 0);
}

// Function to modify customer details
void modify_customer(int sock) {
    char cust_username[50], new_username[50], new_password[50], new_role[20];
    send(sock, "Enter customer username to modify:", 34, 0);
    read(sock, cust_username, sizeof(cust_username));
    send(sock, "Enter new username (leave blank to keep unchanged):", 53, 0);
    read(sock, new_username, sizeof(new_username));
    send(sock, "Enter new password (leave blank to keep unchanged):", 53, 0);
    read(sock, new_password, sizeof(new_password));
    modify_user(cust_username, strcmp(new_username, "") == 0 ? NULL : new_username, 
                 strcmp(new_password, "") == 0 ? NULL : new_password, "customer");
    save_users("users.txt");
    const char *msg = "Customer details modified successfully.";
    send(sock, msg, strlen(msg) + 1, 0);
}

// Function to process loan applications
void process_loan(int sock) {
    char loan_status[20];
    send(sock, "Enter loan status (approved/rejected):", 38, 0);
    read(sock, loan_status, sizeof(loan_status));
    const char *msg = "Loan status updated.";
    send(sock, msg, strlen(msg) + 1, 0);
}

// Function to view assigned loan applications
void view_assigned_loans(int sock) {
    // For simplicity, we log the assignment
    FILE *log_file = fopen("loan_assignments.log", "r");
    char loan_info[BUFFER_SIZE];
    if (log_file != NULL) {
        while (fgets(loan_info, sizeof(loan_info), log_file) != NULL) {
            send(sock, loan_info, strlen(loan_info) + 1, 0);
        }
        fclose(log_file);
    } else {
        const char *msg = "No loan applications assigned.";
        send(sock, msg, strlen(msg) + 1, 0);
    }
}

// Function to view customer transactions
void view_customer_transactions(int sock) {
    FILE *transaction_file = fopen("transactions.log", "r");
    char transaction_info[BUFFER_SIZE];
    if (transaction_file != NULL) {
        while (fgets(transaction_info, sizeof(transaction_info), transaction_file) != NULL) {
            send(sock, transaction_info, strlen(transaction_info) + 1, 0);
        }
        fclose(transaction_file);
    } else {
        const char *msg = "No transactions found.";
        send(sock, msg, strlen(msg) + 1, 0);
    }
}


// Utility function to generate transaction file name
void get_transaction_file_name(const char *username, char *filename) {
    snprintf(filename, BUFFER_SIZE, "%s_transactions.txt", username);
}

// Function to fetch the account balance from the user's transaction file
float fetch_balance(const char *username) {
    char filename[BUFFER_SIZE];
    get_transaction_file_name(username, filename);
    FILE *file = fopen(filename, "r");
    float balance = 20000;

    if (file) {
        // Read the last recorded balance from the file
      //fscanf(file, "%*[^:]: %f\n", &balance);
      //fclose(file);
        char line[BUFFER_SIZE];
        while (fgets(line, sizeof(line), file) != NULL) {
            // Parse the last line to get the last balance
            sscanf(line, "%*[^:]: %f", &balance);
        }
        fclose(file);
    }
    printf("Fetched balance for %s: %.2f\n", username, balance); // Debug print
    return balance;
}

// Function to update balance and log the transaction in the transaction file
void update_balance_and_log(const char *username, const char *transaction, float new_balance) {
    char filename[BUFFER_SIZE];
    get_transaction_file_name(username, filename);
    FILE *file = fopen(filename, "w");

    if (file) {
        fprintf(file, "%s: %.2f\n", transaction, new_balance);
        fclose(file);
    }
}

void deposit_money(int sock, const char *username, float amount) {
   //  char filename[BUFFER_SIZE];
   // get_transaction_file_name(username, filename);
    float current_balance = fetch_balance(username); // Fetch current balance
    printf("Current balance before deposit: %.2f\n", current_balance); // Debug print

    current_balance += amount;  // Update balance
    printf("New balance after deposit: %.2f\n", current_balance); // Debug print

    // Log the transaction
    char log_message[BUFFER_SIZE];
    snprintf(log_message, sizeof(log_message), "Deposited: %.2f", amount);
    log_transaction(username, "Deposited", amount, current_balance);

    update_balance_and_log(username, "Deposit", current_balance); // Log transaction
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "Deposit successful. New balance is: %.2f", current_balance);
    send(sock, response, strlen(response) + 1, 0); // Send response back to the client

}

void withdraw_money(int sock, const char *username, float amount) {
    //char filename[BUFFER_SIZE];
    //get_transaction_file_name(username, filename);
    float current_balance = fetch_balance(username);
    if (amount <= 0 || amount > current_balance) {
        const char *msg = "Withdrawal amount must be positive and cannot exceed your balance.";
        send(sock, msg, strlen(msg) + 1, 0);
        return;
    }

    current_balance -= amount;
    //char log_message[BUFFER_SIZE];
    //snprintf(log_message, sizeof(log_message), "Withdrew: %.2f", amount);
    //log_transaction(username, log_message);
    update_balance_and_log(username, "Withdraw", current_balance);

    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "Withdrawal successful. New balance: %.2f", current_balance);
    send(sock, response, strlen(response) + 1, 0);
}

void transfer_money(const char *from_username, const char *to_username, float amount) {
    float from_balance = fetch_balance(from_username);
    
    if (!check_user_exists(to_username)) {
        printf("Recipient username '%s' does not exist.\n", to_username);
        log_transaction(from_username, "Transfer Out Failed", amount, from_balance);  // Log the transfer out for the sender
      
        return;
    }

    float to_balance = fetch_balance(to_username);

    if (from_balance >= amount) {
        from_balance -= amount;  // Deduct from sender's balance
        to_balance += amount;  // Add to recipient's balance

        update_balance_and_log(from_username, "Transfer Out", from_balance); // Log transfer out
        update_balance_and_log(to_username, "Transfer In", to_balance); // Log transfer in
        // Log transaction details for both users
        log_transaction(from_username, "Transfer Out", amount, from_balance);  // Log the transfer out for the sender
        log_transaction(to_username, "Transfer In", amount, to_balance);  // Log the transfer in for the recipient
        printf("Transfer of %.2f from '%s' to '%s' successful.\n", amount, from_username, to_username);
      } else {
          printf("Insufficient funds in account '%s'. Transfer failed.\n", from_username);
          log_transaction(to_username, "Transfer Out Failed", amount, to_balance);  
    }
}

void log_transaction(const char *username, const char *transaction, float amount, float balance) {
    char filename[BUFFER_SIZE];
    get_transaction_file_name(username, filename);  // Generate the transaction log file name

    FILE *log_file = fopen(filename, "a");  // Open the transaction log file in append mode
    if (log_file != NULL) {
        // Log transaction with details: type of transaction, amount, and resulting balance
        fprintf(log_file, "%s: %.2f, Balance: %.2f\n", transaction, amount, balance);
        fclose(log_file);
    } else {
        printf("Error: Unable to open transaction log for user '%s'.\n", username);
    }   
}


void view_account_balance(int sock, const char *username) {
    float balance = fetch_balance(username); // Fetch balance from transaction file
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "%.2f", balance);
    send(sock, response, strlen(response) + 1, 0);
}

void apply_for_loan(int sock, const char *username) {
    char loan_amount[20];
    send(sock, "Enter loan amount:", 18, 0);
    read(sock, loan_amount, sizeof(loan_amount));
    float amount = atof(loan_amount);
    char loan_transaction[BUFFER_SIZE];
    snprintf(loan_transaction, sizeof(loan_transaction), "Loan Application: %.2f", amount);
    log_transaction(username, "Loan Application", amount, fetch_balance(username));  // Log the loan application
    const char *msg = "Loan application submitted.";
    send(sock, msg, strlen(msg) + 1, 0);
}

void add_feedback(int sock, const char *username) {
    char feedback[BUFFER_SIZE];
    send(sock, "Enter your feedback:", 20, 0);
    read(sock, feedback, sizeof(feedback));
    char feedback_log[BUFFER_SIZE];
    snprintf(feedback_log, sizeof(feedback_log), "Feedback: %.1023s", feedback);
    log_transaction(username, feedback_log, 0.0, fetch_balance(username));  // Log feedback
    const char *msg = "Feedback submitted.";
    send(sock, msg, strlen(msg) + 1, 0);
}

void view_transaction_history(int sock, const char *username) {
    // Fetch the transaction history from the user's separate transaction file
    char filename[BUFFER_SIZE];
    get_transaction_file_name(username, filename);
    FILE *transaction_file = fopen(filename, "r");
    char transaction_info[BUFFER_SIZE];
    
    if (transaction_file != NULL) {
        while (fgets(transaction_info, sizeof(transaction_info), transaction_file) != NULL) {
            send(sock, transaction_info, strlen(transaction_info) + 1, 0);  // Send each line to the client
        }
        fclose(transaction_file);
    } else {
        const char *msg = "No transactions found.";
        send(sock, msg, strlen(msg) + 1, 0);
    }
}



// Function to handle client requests
void *handle_client(void *arg) {
    client_data *data = (client_data *)arg;
    int sock = data->socket;
    char buffer[BUFFER_SIZE];
    char username[50], password[50], role[20];
    char log_action[BUFFER_SIZE]; // Declare a buffer
  
    // Receive username
    read(sock, username, sizeof(username));
    // Receive password
    read(sock, password, sizeof(password));
    // Receive role
    read(sock, role, sizeof(role));

    // Check credentials
    if (check_credentials(username, password, role)) {
        const char *response = "Login successful!";
        send(sock, response, strlen(response) + 1, 0);
        // Here you can perform role-based actions
        printf("User %s logged in as: %s\n", username, role);
        
        strcpy(data->username, username);
        strcpy(data->role, role);
        
        
        if (strcmp(role, "admin") == 0) {
            int admin_choice;
            while (1) {
                read(sock, &admin_choice, sizeof(admin_choice));

                if (admin_choice == 1) {
                    // Add new employee
                    char emp_username[50], emp_password[50];
                    send(sock, "Enter new employee username:", 29, 0);
                    read(sock, emp_username, sizeof(emp_username));
                    send(sock, "Enter new employee password:", 29, 0);
                    read(sock, emp_password, sizeof(emp_password));
                    add_user(emp_username, emp_password, "employee");
                    save_users("users.txt");
                    snprintf(log_action, sizeof(log_action), "Added new employee: %s", emp_username);
                    log_admin_action(log_action);
                    const char *msg = "Employee added successfully.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (admin_choice == 2) {
                    // Modify user details
                    char mod_username[50], new_username[50], new_password[50], new_role[20];
                    send(sock, "Enter username to modify:", 25, 0);
                    read(sock, mod_username, sizeof(mod_username));
                    send(sock, "Enter new username (leave blank to keep unchanged):", 53, 0);
                    read(sock, new_username, sizeof(new_username));
                    send(sock, "Enter new password (leave blank to keep unchanged):", 53, 0);
                    read(sock, new_password, sizeof(new_password));
                    send(sock, "Enter new role (leave blank to keep unchanged):", 50, 0);
                    read(sock, new_role, sizeof(new_role));
                    modify_user(mod_username, strcmp(new_username, "") == 0 ? NULL : new_username,
                                 strcmp(new_password, "") == 0 ? NULL : new_password,
                                 strcmp(new_role, "") == 0 ? NULL : new_role);
                    save_users("users.txt");
                    snprintf(log_action, sizeof(log_action), "Modified user details for: %s", mod_username);
                    log_admin_action(log_action); 
                    const char *msg = "User details modified successfully.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (admin_choice == 3) {
                    // Change password
                    char new_password[50];
                    send(sock, "Enter new password:", 20, 0);
                    read(sock, new_password, sizeof(new_password));
                    change_password(username, new_password);
                    save_users("users.txt");
                    snprintf(log_action, sizeof(log_action), "Changed password for user: %s", username);
                    log_admin_action(log_action);
                    const char *msg = "Password changed successfully.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (admin_choice == 4) {
                    char username[50], new_role[20];
                    send(sock, "Enter username to change role:", 31, 0);
                    read(sock, username, sizeof(username));
                    send(sock, "Enter new role:", 16, 0);
                    read(sock, new_role, sizeof(new_role));
                    manage_user_role(username, new_role);
                    save_users("users.txt");
                    snprintf(log_action, sizeof(log_action), "Changed role for user: %s to %s", username, new_role);
                    log_admin_action(log_action);
                    const char *msg = "User role changed successfully.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (admin_choice == 5) {
                    char del_username[50];
                    send(sock, "Enter username to delete:", 25, 0);
                    read(sock, del_username, sizeof(del_username));
                    delete_user(del_username);
                    snprintf(log_action, sizeof(log_action), "Deleted user: %s", del_username);
                    log_admin_action(log_action);
                    save_users("users.txt");
                    const char *msg = "User deleted successfully.";
                    send(sock, msg, strlen(msg) + 1, 0);
                }else if (admin_choice == 6) {
                    // Logout
                    logout_user(sock);
                    break;
                } else if (admin_choice == 7) {
                    // Exit
                    close(sock);
                    free(data);
                    pthread_exit(NULL);
                }
            }
        } 
        // Example role-based response
        else if (strcmp(role, "manager") == 0) {
        int manager_choice;
        while (1) {
            read(sock, &manager_choice, sizeof(manager_choice));

            if (manager_choice == 1) {
              // Activate/Deactivate Customer Accounts
              char cust_username[50];
              int activate;
              send(sock, "Enter customer username:", 25, 0);
              read(sock, cust_username, sizeof(cust_username));
              send(sock, "Enter 1 to activate, 0 to deactivate:", 39, 0);
              read(sock, &activate, sizeof(activate));
              activate_deactivate_account(cust_username, activate);
              const char *msg = "Customer account status updated.";
              send(sock, msg, strlen(msg) + 1, 0);
          }else if (manager_choice == 2) {
            // Assign Loan Application Processes to Employees
            char emp_username[50];
            send(sock, "Enter employee username to assign loan application:", 53, 0);
            read(sock, emp_username, sizeof(emp_username));
            assign_loan_application(emp_username);
            const char *msg = "Loan application assigned.";
            send(sock, msg, strlen(msg) + 1, 0);
          }else if (manager_choice == 3) {
            // Review Customer Feedback
            review_feedback(sock);
          }else if (manager_choice == 4) {
            // Change Password
            char new_password[50];
            send(sock, "Enter new password:", 20, 0);
            read(sock, new_password, sizeof(new_password));
            change_password(username, new_password);
            save_users("users.txt");
            const char *msg = "Password changed successfully.";
            send(sock, msg, strlen(msg) + 1, 0);
          }else if (manager_choice == 5) {
            // Logout
            logout_user(sock);
            break;
          }else if (manager_choice == 6) {
            // Exit
            close(sock);
            free(data);
            pthread_exit(NULL);
          }
        }
      }
      else if (strcmp(role, "customer") == 0) {
            int customer_choice;
            while (1) {
                read(sock, &customer_choice, sizeof(customer_choice));

                if (customer_choice == 1) {
                    view_account_balance(sock, username);
                } else if (customer_choice == 2) {
                    float amount;
                    //send(sock, "Enter amount to deposit:", 25, 0);
                    read(sock, &amount, sizeof(amount));
                   if (amount <= 0) {
                      const char *msg = "Deposit amount must be positive.";
                      send(sock, msg, strlen(msg) + 1, 0);
                  } else {
                    // Perform the deposit operation
                    deposit_money(sock, username, amount);
                  // Send a success message back to the client
                  const char *msg = "Deposit successful.";
                  send(sock, msg, strlen(msg) + 1, 0);
                }
              }
               else if (customer_choice == 3) {
                    float amount;
                    send(sock, "Enter amount to withdraw:", 25, 0);
                    read(sock, &amount, sizeof(amount));
                    withdraw_money(sock, username, amount);
                    log_transaction(username, "Withdrawal", amount, fetch_balance(username));
                    const char *msg = "Withdrawal successful.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (customer_choice == 4) {
                    char to_username[50];
                    float amount;
                    send(sock, "Enter recipient username:", 25, 0);
                    read(sock, to_username, sizeof(to_username));
                    send(sock, "Enter amount to transfer:", 25, 0);
                    read(sock, &amount, sizeof(amount));
                    transfer_money(username, to_username, amount);
                    log_transaction(username, "Transfer to", amount, fetch_balance(username));
                    const char *msg = "Transfer successful.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (customer_choice == 5) {
                    apply_for_loan(sock, username);
                } else if (customer_choice == 6) {
                    char new_password[50];
                    send(sock, "Enter new password:", 20, 0);
                    read(sock, new_password, sizeof(new_password));
                    change_password(username, new_password);
                    const char *msg = "Password changed successfully.";
                    send(sock, msg, strlen(msg) + 1, 0);
                } else if (customer_choice == 7) {
                    add_feedback(sock, username);
                } else if (customer_choice == 8) {
                    view_transaction_history(sock, username);
                } else if (customer_choice == 9) {
                    logout_user(sock);
                    break; // Exit the loop
                } else if (customer_choice == 10) {
                    close(sock);
                    exit(0);
                }
            }
        }else if (strcmp(role, "employee") == 0) {
       int employee_choice;
       while (1) {
        read(sock, &employee_choice, sizeof(employee_choice));
        
        if (employee_choice == 1) {
            // Add new customer
            add_customer(sock);
        } else if (employee_choice == 2) {
            // Modify customer details
            modify_customer(sock);
        } else if (employee_choice == 3) {
            // Process loan applications
            process_loan(sock);
        } else if (employee_choice == 4) {
            // Approve/Reject loans
            process_loan(sock);
        } else if (employee_choice == 5) {
            // View assigned loan applications
            view_assigned_loans(sock);
        } else if (employee_choice == 6) {
            // View customer transactions (passbook)
            view_customer_transactions(sock);
        } else if (employee_choice == 7) {
            // Change password
            char new_password[50];
            send(sock, "Enter new password:", 20, 0);
            read(sock, new_password, sizeof(new_password));
            change_password(username, new_password);
            save_users("users.txt");
            const char *msg = "Password changed successfully.";
            send(sock, msg, strlen(msg) + 1, 0);
        } else if (employee_choice == 8) {
            // Logout
            logout_user(sock);
            break;
        } else if (employee_choice == 9) {
            // Exit
            close(sock);
            free(data);
            pthread_exit(NULL);
        }
    }
}
    }

    close(sock);
    free(data);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Load users from file
    load_users("users.txt");

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Set socket options failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        client_data *data = malloc(sizeof(client_data));
        if (!data) {
            perror("Failed to allocate memory");
            continue;
        }

        // Accept a new connection
        if ((data->socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            free(data);
            continue;
        }

        // Create a new thread to handle the client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, data) != 0) {
            perror("Thread creation failed");
            close(data->socket);
            free(data);
        } else {
            pthread_detach(thread_id); // Detach thread to free resources automatically
        }
    }

    close(server_fd);
    return 0;
}

