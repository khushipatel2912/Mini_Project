#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024


void display_admin_menu(int sock) {
    int choice;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nAdmin Menu:\n");
        printf("1. Add new employee\n");
        printf("2. Modify user details\n");
        printf("3. Change password\n");
        printf("4. Manage User Roles\n");
        printf("5. Delete User\n");
        printf("6. Logout\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        send(sock, &choice, sizeof(choice), 0);

        if (choice == 1) {
            char username[50], password[50];
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", username);
            send(sock, username, strlen(username) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", password);
            send(sock, password, strlen(password) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s\n", buffer);
        } else if (choice == 2) {
            char username[50], new_username[50], new_password[50], new_role[20];
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", username);
            send(sock, username, strlen(username) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", new_username);
            send(sock, new_username, strlen(new_username) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", new_password);
            send(sock, new_password, strlen(new_password) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", new_role);
            send(sock, new_role, strlen(new_role) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s\n", buffer);
        } else if (choice == 3) {
            char new_password[50];
            read(sock, buffer, BUFFER_SIZE);
            printf("%s", buffer);
            scanf("%s", new_password);
            send(sock, new_password, strlen(new_password) + 1, 0);
            read(sock, buffer, BUFFER_SIZE);
            printf("%s\n", buffer);
        } else if (choice == 4) {
          char username[50], new_role[20];
          read(sock, buffer, BUFFER_SIZE);
          printf("%s", buffer);
          scanf("%s", username);
          send(sock, username, strlen(username) + 1, 0);
          read(sock, buffer, BUFFER_SIZE);
          printf("%s", buffer);
          scanf("%s", new_role);
          send(sock, new_role, strlen(new_role) + 1, 0);
          read(sock, buffer, BUFFER_SIZE);
          printf("%s\n", buffer);
        } else if (choice == 5) {
          char username[50];
          read(sock, buffer, BUFFER_SIZE);
          printf("%s", buffer);
          scanf("%49s", username);
          send(sock, username, strlen(username) + 1, 0);
          read(sock, buffer, BUFFER_SIZE);
          printf("%s\n", buffer);
        } else if (choice == 6 || choice == 7) {
            read(sock, buffer, BUFFER_SIZE);
            printf("%s\n", buffer);
            break;
        }else {
            printf("Invalid choice. Please try again.\n");
            display_admin_menu(sock);
        }
    }
}


void display_manager_menu(int sock) {
    int choice;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nManager Menu:\n");
        printf("1. Activate/Deactivate Customer Accounts\n");
        printf("2. Assign Loan Application to Employees\n");
        printf("3. Review Customer Feedback\n");
        printf("4. Change Password\n");
        printf("5. Logout\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        send(sock, &choice, sizeof(choice), 0);

        if (choice == 1) {
            // Activate/Deactivate Customer Accounts
            char cust_acc_no[50];
            int activate;
            printf("Enter customer username: ");
            scanf("%s", cust_acc_no);
            send(sock, cust_acc_no, sizeof(cust_acc_no), 0);
            printf("Enter 1 to activate, 0 to deactivate: ");
            scanf("%d", &activate);
            send(sock, &activate, sizeof(activate), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 2) {
            // Assign Loan Application to Employees
            char emp_username[50];
            printf("Enter employee username to assign loan application: ");
            scanf("%s", emp_username);
            send(sock, emp_username, sizeof(emp_username), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 3) {
            // Review Customer Feedback
            printf("Customer Feedback:\n");
            while (read(sock, buffer, sizeof(buffer)) > 0) {
                printf("%s\n", buffer);
            }
        } else if (choice == 4) {
            // Change Password
            char new_password[50];
            printf("Enter new password: ");
            scanf("%s", new_password);
            send(sock, new_password, sizeof(new_password), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 5) {
            // Logout
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            break;
        } else if (choice == 6) {
            // Exit
            close(sock);
            exit(0);
        }else {
            printf("Invalid choice. Please try again.\n");
            display_manager_menu(sock);
        }
    }
}

void display_employee_menu(int sock) {
    int choice;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nEmployee Menu:\n");
        printf("1. Add new customer\n");
        printf("2. Modify customer details\n");
        printf("3. Process loan applications\n");
        printf("4. Approve/Reject loans\n");
        printf("5. View assigned loan applications\n");
        printf("6. View customer transactions (passbook)\n");
        printf("7. Change password\n");
        printf("8. Logout\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        send(sock, &choice, sizeof(choice), 0);

        
        if (choice == 1) {
            char customer_username[50], customer_details[BUFFER_SIZE];
            printf("Enter new customer username: ");
            scanf("%s", customer_username);
            printf("Enter customer details: ");
            scanf(" %[^\n]", customer_details);
            send(sock, customer_username, strlen(customer_username) + 1, 0);
            send(sock, customer_details, strlen(customer_details) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 2) {
            char customer_username[50], updated_details[BUFFER_SIZE];
            printf("Enter customer username to modify: ");
            scanf("%s", customer_username);
            printf("Enter updated customer details: ");
            scanf(" %[^\n]", updated_details);
            send(sock, customer_username, strlen(customer_username) + 1, 0);
            send(sock, updated_details, strlen(updated_details) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 3) {
            char loan_request[BUFFER_SIZE];
            printf("Enter loan application details: ");
            scanf(" %[^\n]", loan_request);
            send(sock, loan_request, strlen(loan_request) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 4) {
            char loan_decision[BUFFER_SIZE];
            printf("Enter loan approval/rejection details: ");
            scanf(" %[^\n]", loan_decision);
            send(sock, loan_decision, strlen(loan_decision) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 5) {
            // View assigned loan applications
            read(sock, buffer, sizeof(buffer));
            printf("Assigned Loan Applications:\n%s\n", buffer);
        } else if (choice == 6) {
            // View customer transactions (passbook)
            char customer_username[50];
            printf("Enter customer username to view transactions: ");
            scanf("%s", customer_username);
            send(sock, customer_username, strlen(customer_username) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("Customer Transactions:\n%s\n", buffer);
        } else if (choice == 7) {
            char new_password[50];
            printf("Enter new password: ");
            scanf("%s", new_password);
            send(sock, new_password, strlen(new_password) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 8) {
            // Logout
            const char *msg = "Logging out...";
            send(sock, msg, strlen(msg) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            break;
        } else if (choice == 9) {
            close(sock);
            exit(0);
        } else {
            printf("Invalid choice. Please try again.\n");
            display_employee_menu(sock);
        }
    }
}

void display_customer_menu(int sock) {
    int choice;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nCustomer Menu:\n");
        printf("1. View Account Balance\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Transfer Funds\n");
        printf("5. Apply for a Loan\n");
        printf("6. Change Password\n");
        printf("7. Add Feedback\n");
        printf("8. View Transaction History\n");
        printf("9. Logout\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        send(sock, &choice, sizeof(choice), 0);

        if (choice == 1) {
            read(sock, buffer, sizeof(buffer));
            printf("Account Balance: %s\n", buffer);
        } else if (choice == 2) {
            float amount;
            printf("Enter amount to deposit: ");
            scanf("%f", &amount);
            send(sock, &amount, sizeof(amount), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            int balance_choice = 1; // Code for fetching balance
            send(sock, &balance_choice, sizeof(balance_choice), 0); // Re-send the request for balance
            read(sock, buffer, sizeof(buffer));
            printf("Updated Account Balance: %s\n", buffer);
        } else if (choice == 3) {
            double amount;
            printf("Enter amount to withdraw: ");
            scanf("%lf", &amount);  // Check scanf return value
            send(sock, &amount, sizeof(amount), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            int balance_choice = 1;  // Code for fetching balance
            send(sock, &balance_choice, sizeof(balance_choice), 0);  // Re-send the request for balance
            read(sock, buffer, sizeof(buffer));
            printf("Updated Account Balance: %s\n", buffer);

        } else if (choice == 4) {
            double amount;
            char recipient[50];
            printf("Enter recipient username: ");
            scanf("%s", recipient);
            send(sock, recipient, sizeof(recipient), 0);
            printf("Enter amount to transfer: ");
            scanf("%lf", &amount);
            send(sock, &amount, sizeof(amount), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 5) {
            double loan_amount;
            printf("Enter loan amount: ");
            scanf("%lf", &loan_amount);
            send(sock, &loan_amount, sizeof(loan_amount), 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 6) {
            char new_password[50];
            printf("Enter new password: ");
            scanf("%s", new_password);
            send(sock, new_password, strlen(new_password) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 7) {
            char feedback[BUFFER_SIZE];
            printf("Enter your feedback: ");
            scanf(" %[^\n]", feedback);
            send(sock, feedback, strlen(feedback) + 1, 0);
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else if (choice == 8) {
            // View transaction history
            read(sock, buffer, sizeof(buffer));
            printf("Transaction History:\n%s\n", buffer);
        } else if (choice == 9) {
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            break;
        } else if (choice == 10) {
            close(sock);
            exit(0);
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
}



int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char username[50], password[50], role[20];
    int role_choice;

    while (1) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    
    printf("Enter username: ");
    scanf("%s", username);
    send(sock, username, strlen(username) + 1, 0);

    printf("Enter password: ");
    scanf("%s", password);
    send(sock, password, strlen(password) + 1, 0);

    printf("Enter role: ");
    scanf("%s", role);
    send(sock, role, strlen(role) + 1, 0);

    read(sock, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);

  if (strcmp(buffer, "Login successful!") == 0) {
    if (strcmp(role, "admin") == 0) {
        display_admin_menu(sock);
    } else if (strcmp(role, "customer") == 0) {
        display_customer_menu(sock);
    } else if (strcmp(role, "employee") == 0) {
        display_employee_menu(sock);
    } else if (strcmp(role, "manager") == 0) {
        display_manager_menu(sock);
    }
  } else {
            printf("Login failed. Try again.\n");
            close(sock);
        }
    }

    return 0;
}

