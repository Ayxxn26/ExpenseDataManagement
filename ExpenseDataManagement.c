#include <stdio.h>
#include <string.h>

#define MAX_USERS 1000
#define MAX_EXPENSES 1000
#define MAX_SHARED_USERS 4

// User structure definition
struct User {
    int id;             
    char name[100];     
    double totalSpent;  
};

// Expense structure definition
struct Expense {
    int id;             
    double amount;     
    int paidBy;         
    int sharedWith[MAX_SHARED_USERS];  
    int numSharedUsers;
};

struct User users[MAX_USERS];
struct Expense expenses[MAX_EXPENSES];
int numUsers = 0;
int numExpenses = 0;

// Function to add a new user
void AddUser(int id, char name[100], double totalSpent) {
    if (numUsers >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return;
    }

    // Create a new user and insart it in a sorted manner according to User ID
    int insertIndex = 0;
    while (insertIndex < numUsers && users[insertIndex].id < id) {
        insertIndex++;
    }

    for (int i = numUsers; i > insertIndex; i--) {
        users[i] = users[i - 1];
    }

    users[insertIndex].id = id;
    strcpy(users[insertIndex].name, name);
    users[insertIndex].totalSpent = totalSpent;

    numUsers++;
}

// Function to add an expense
void AddExpense(int id, double amount, int paidBy, int sharedWith[MAX_SHARED_USERS], int numSharedUsers) {
    if (numExpenses >= MAX_EXPENSES) {
        printf("Maximum number of expenses reached.\n");
        return;
    }

    // Create a new expanse and insert in sorted manner based on Expense ID
    int insertIndex = 0;
    while (insertIndex < numExpenses && expenses[insertIndex].id < id) {
        insertIndex++;
    }

    for (int i = numExpenses; i > insertIndex; i--) {
        expenses[i] = expenses[i - 1];
    }

    expenses[insertIndex].id = id;
    expenses[insertIndex].amount = amount;
    expenses[insertIndex].paidBy = paidBy;
    expenses[insertIndex].numSharedUsers = numSharedUsers;

    for (int i = 0; i < numSharedUsers; i++) {
        expenses[insertIndex].sharedWith[i] = sharedWith[i];
    }

    numExpenses++;
}

// Function to calculate the amount owed to a user
void AmountOwed(int userID) {
    double totalAmountOwed = 0;
    printf("Amount owed to User %03d (%s):\n", userID, users[userID - 1].name);

    for (int i = 0; i < numExpenses; i++) {
        if (expenses[i].paidBy != userID) {
            for (int j = 0; j < expenses[i].numSharedUsers; j++) {
                if (expenses[i].sharedWith[j] == userID) {
                    double sharePerUser = expenses[i].amount / expenses[i].numSharedUsers;
                    totalAmountOwed += sharePerUser;

                    for (int k = 0; k < numUsers; k++) {
                        if (users[k].id == expenses[i].paidBy) {
                            printf("Expense ID: %03d, Amount: $%.2f (Owed by %s - User %03d)\n",
                                   expenses[i].id, sharePerUser, users[k].name, users[k].id);
                            break;
                        }
                    }
                }
            }
        }
    }

    printf("Total amount owed: $%.2f\n", totalAmountOwed);
}

// Function to calculate the amount to be paid by a user
void Amount_to_pay(int userID) {
    double totalAmountToPay = 0;
    printf("Amount to be paid by User %03d (%s):\n", userID, users[userID - 1].name);

    for (int i = 0; i < numExpenses; i++) {
        if (expenses[i].paidBy == userID) {
            for (int j = 0; j < expenses[i].numSharedUsers; j++) {
                int sharedUserID = expenses[i].sharedWith[j];
                double sharePerUser = expenses[i].amount / expenses[i].numSharedUsers;
                totalAmountToPay += sharePerUser;

                for (int k = 0; k < numUsers; k++) {
                    if (users[k].id == sharedUserID) {
                        printf("Expense ID: %03d, Amount: $%.2f (To be paid to %s - User %03d)\n",
                               expenses[i].id, sharePerUser, users[k].name, users[k].id);
                        break;
                    }
                }
            }
        }
    }

    printf("Total amount to be paid: $%.2f\n", totalAmountToPay);
}

// Function to display the final balances for each user
void User_balances() {
    printf("User Balances:\n");
    for (int i = 0; i < numUsers; i++) {
        double balance = users[i].totalSpent;
        for (int j = 0; j < numExpenses; j++) {
            if (expenses[j].paidBy == users[i].id) {
                double sharePerUser = expenses[j].amount / expenses[j].numSharedUsers;
                balance -= sharePerUser;
            } else {
                for (int k = 0; k < expenses[j].numSharedUsers; k++) {
                    if (expenses[j].sharedWith[k] == users[i].id) {
                        double sharePerUser = expenses[j].amount / expenses[j].numSharedUsers;
                        balance += sharePerUser;
                    }
                }
            }
        }
        printf("%s (User %03d) - Balance: $%.2f\n", users[i].name, users[i].id, balance);
    }
}

// Function to settle up an expense between two users
void Settle_up(int userID1, int userID2) {
    printf("Settling up expenses between User %03d and User %03d:\n", userID1, userID2);
    printf("Enter the Expense ID to settle (or -1 to exit): ");
    int expenseID;
    scanf("%d", &expenseID);

    while (expenseID != -1) {
        int expenseIndex = -1;
        for (int i = 0; i < numExpenses; i++) {
            if (expenses[i].id == expenseID) {
                expenseIndex = i;
                break;
            }
        }

        if (expenseIndex == -1) {
            printf("Expense ID not found. Enter a valid Expense ID: ");
        } else {
            // Check if the expense is associated with both users
            int foundUser1 = 0;
            int foundUser2 = 0;

            for (int i = 0; i < expenses[expenseIndex].numSharedUsers; i++) {
                if (expenses[expenseIndex].sharedWith[i] == userID1) {
                    foundUser1 = 1;
                }
                if (expenses[expenseIndex].sharedWith[i] == userID2) {
                    foundUser2 = 1;
                }
            }

            if (foundUser1 && foundUser2) {
                // Expense can be settled
                double sharePerUser = expenses[expenseIndex].amount / expenses[expenseIndex].numSharedUsers;
                for (int i = 0; i < numUsers; i++) {
                    if (users[i].id == userID1) {
                        printf("%s (User %03d) owes $%.2f to %s (User %03d) for Expense ID %03d\n",
                               users[i].name, userID1, sharePerUser,
                               users[i].name, userID2, expenses[expenseIndex].id);
                    } else if (users[i].id == userID2) {
                        printf("%s (User %03d) owes $%.2f to %s (User %03d) for Expense ID %03d\n",
                               users[i].name, userID2, sharePerUser,
                               users[i].name, userID1, expenses[expenseIndex].id);
                    }
                }

                // Update the expense
                for (int i = expenseIndex; i < numExpenses - 1; i++) {
                    expenses[i] = expenses[i + 1];
                }
                numExpenses--;

                printf("Settlement completed for Expense ID %03d.\n", expenseID);
            } else {
                printf("The expense is not associated with both users. Enter a valid Expense ID: ");
            }
        }

        printf("Enter the Expense ID to settle (or -1 to exit): ");
        scanf("%d", &expenseID);
    }
}
// Function to delete a user when all balances are settled
void DeleteUser(int userID) {
    double balance = users[userID - 1].totalSpent;

    // Check if the user owes or is owed money
    for (int i = 0; i < numExpenses; i++) {
        if (expenses[i].paidBy == userID) {
            double sharePerUser = expenses[i].amount / expenses[i].numSharedUsers;
            balance -= sharePerUser;
        } else {
            for (int j = 0; j < expenses[i].numSharedUsers; j++) {
                if (expenses[i].sharedWith[j] == userID) {
                    double sharePerUser = expenses[i].amount / expenses[i].numSharedUsers;
                    balance += sharePerUser;
                }
            }
        }
    }

    if (balance == 0) {
        // All balances are settled so delete the user
        int deleteUserIndex = -1;
        for (int i = 0; i < numUsers; i++) {
            if (users[i].id == userID) {
                deleteUserIndex = i;
                break;
            }
        }

        if (deleteUserIndex != -1) {
            for (int i = deleteUserIndex; i < numUsers - 1; i++) {
                users[i] = users[i + 1];
            }
            numUsers--;

            printf("User %03d (%s) has been deleted.\n", userID, users[deleteUserIndex].name);
        }
    } else {
        // User has unsettled balances display them and ask to settle
        printf("User %03d (%s) cannot be deleted until all balances are settled.\n", userID, users[userID - 1].name);
        AmountOwed(userID);
        Amount_to_pay(userID);
    }
}
// Function to delete an expense record
void DeleteExpense(int expenseID, int userID) {
    int deleteExpenseIndex = -1;

    // Check if the expense ID and User ID combination matches a recorded expense
    for (int i = 0; i < numExpenses; i++) {
        if (expenses[i].id == expenseID && expenses[i].paidBy == userID) {
            deleteExpenseIndex = i;
            break;
        }
    }

    if (deleteExpenseIndex != -1) {
        // Delete the expense record
        for (int i = deleteExpenseIndex; i < numExpenses - 1; i++) {
            expenses[i] = expenses[i + 1];
        }
        numExpenses--;

        printf("Expense ID %03d has been deleted.\n", expenseID);
    } else {
        printf("Enter correct User ID and Expense ID combination.\n");
    }
}



int main() {
    int choice;

    while (1) {
        printf("Expense Manager Menu:\n");
        printf("1. Add User\n");
        printf("2. Add Expense\n");
        printf("3. Calculate Amount Owed\n");
        printf("4. Calculate Amount to Pay\n");
        printf("5. Display User Balances\n");
        printf("6. Settle Expenses\n");
        printf("7. Delete User\n");
        printf("8. Delete Expenses\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                int id;
                char name[100];
                double totalSpent;
                printf("Enter User ID (3-digit): ");
                scanf("%d", &id);
                printf("Enter User Name: ");
                scanf("%s", name);
                printf("Enter Total Amount Spent: ");
                scanf("%lf", &totalSpent);
                AddUser(id, name, totalSpent);
                break;
            case 2:
                int expenseID;
                double expenseAmount;
                int paidBy;
                int numSharedUsers;
                int sharedWith[MAX_SHARED_USERS];
                printf("Enter Expense ID (3-digit): ");
                scanf("%d", &expenseID);
                printf("Enter Expense Amount: ");
                scanf("%lf", &expenseAmount);
                printf("Enter User ID who paid the expense: ");
                scanf("%d", &paidBy);
                printf("Enter the number of users shared this expense: ");
                scanf("%d", &numSharedUsers);
                printf("Enter User IDs shared this expense:\n");
                for (int i = 0; i < numSharedUsers; i++) {
                    scanf("%d", &sharedWith[i]);
                }
                AddExpense(expenseID, expenseAmount, paidBy, sharedWith, numSharedUsers);
                break;
            case 3:
                int userIDOwed;
                printf("Enter User ID to calculate the amount owed: ");
                scanf("%d", &userIDOwed);
                AmountOwed(userIDOwed);
                break;
            case 4:
                int userIDToPay;
                printf("Enter User ID to calculate the amount to be paid: ");
                scanf("%d", &userIDToPay);
                Amount_to_pay(userIDToPay);
                break;
            case 5:
                User_balances();
                break;
            case 6:
                int userID1, userID2;
                printf("Enter User ID 1: ");
                scanf("%d", &userID1);
                printf("Enter User ID 2: ");
                scanf("%d", &userID2);
                Settle_up(userID1, userID2);
                break;
            case 7:
                int userIDToDelete;
                printf("Enter User ID to delete: ");
                scanf("%d", &userIDToDelete);
                DeleteUser(userIDToDelete);
                break;
            case 8:
               int deleteExpenseID, deleteExpenseUserID;
               printf("Enter Expense ID to delete: ");
               scanf("%d", &deleteExpenseID);
               printf("Enter User ID associated with the expense: ");
               scanf("%d", &deleteExpenseUserID);
               DeleteExpense(deleteExpenseID, deleteExpenseUserID);
               break;

            case 9:
                return 0;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    }
}