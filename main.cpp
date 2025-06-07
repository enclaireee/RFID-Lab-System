#include "RFIDSystem.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

enum class UserType {
    ADMIN,
    USER,
    INVALID
};

void displayLoginMenu() {
    cout << "\n========== RFID LAB SYSTEM LOGIN ==========\n";
    cout << "1. Admin Login\n";
    cout << "2. User Mode\n";
    cout << "0. Exit System\n";
    cout << "==========================================\n";
    cout << "Select login type: ";
}

void displayAdminMenu() {
    cout << "\n========== ADMIN PANEL ==========\n";
    cout << "1. Add New User\n";
    cout << "2. Scan Card\n";
    cout << "3. Search User Logs\n";
    cout << "4. Display All Logs\n";
    cout << "5. Display User Status\n";
    cout << "6. Display Daily Report\n";
    cout << "7. Display All Users\n";
    cout << "8. Save System Data\n";
    cout << "9. Export to JSON\n";
    cout << "10. Clear Daily Logs\n";
    cout << "11. Clear All Data\n";
    cout << "12. Logout to Main Menu\n";
    cout << "0. Exit System\n";
    cout << "================================\n";
    cout << "Enter your Choice: ";
}

void displayUserMenu() {
    cout << "\n========== USER INTERFACE ==========\n";
    cout << "1. Scan RFID Card\n";
    cout << "2. Back to Main Menu\n";
    cout << "0. Exit System\n";
    cout << "===================================\n";
    cout << "Enter your Choice: ";
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool isValidUserId(const string& id) {
    if (id.length() < 3 || id.length() > 10) return false;
    for (char c : id) {
        if (!isalnum(c)) return false;
    }
    return true;
}

bool isValidName(const string& name) {
    if (name.length() < 2 || name.length() > 50) return false;
    for (char c : name) {
        if (!isalpha(c) && c != ' ' && c != '.' && c != '-' && c != '\'') return false;
    }
    if (name.front() == ' ' || name.back() == ' ') return false;
    return true;
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getValidIntInput(int min, int max) {
    int input;
    while (true) {
        if (cin >> input) {
            if (input >= min && input <= max) {
                cin.ignore(); // Clear the newline
                return input;
            } else {
                cout << "Error: Please enter a number between " << min << " and " << max << ": ";
            }
        } else {
            clearInputBuffer();
            cout << "Error: Invalid input. Please enter a number: ";
        }
    }
}

UserType authenticateAdmin() {
    string username, password;
    const string ADMIN_USERNAME = "testAdmin";
    const string ADMIN_PASSWORD = "alprog05";

    cout << "\n========== ADMIN LOGIN ==========\n";
    cout << "Username: ";
    getline(cin, username);
    username = trim(username);

    cout << "Password: ";
    getline(cin, password);
    password = trim(password);

    if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
        cout << "✓ Admin login successful!\n";
        return UserType::ADMIN;
    } else {
        cout << "✗ Invalid credentials! Access denied.\n";
        return UserType::INVALID;
    }
}

void addUserInterface(RFIDSystem& system) {
    cout << "\n========== ADD NEW USER ==========\n";

    string id, name, role;

    while (true) {
        cout << "Enter User ID (3-10 alphanumeric characters): ";
        getline(cin, id);
        id = trim(id);

        if (id.empty()) {
            cout << "Error: User ID cannot be empty.\n";
            continue;
        }

        if (!isValidUserId(id)) {
            cout << "Error: Invalid User ID format. Use 3-10 alphanumeric characters only.\n";
            continue;
        }

        if (system.findUser(id) != nullptr) {
            cout << "Error: User ID '" << id << "' already exists. Please choose a different ID.\n";
            continue;
        }

        break;
    }

    while (true) {
        cout << "Enter Full Name (2-50 characters): ";
        getline(cin, name);
        name = trim(name);

        if (name.empty()) {
            cout << "Error: Name cannot be empty.\n";
            continue;
        }

        if (!isValidName(name)) {
            cout << "Error: Invalid name format. Use letters, spaces, dots, hyphens, and apostrophes only.\n";
            continue;
        }

        break;
    }

    while (true) {
        cout << "\nSelect User Role:\n";
        cout << "1. Student\n";
        cout << "2. Staff\n";
        cout << "3. Faculty\n";
        cout << "Enter choice (1-3): ";

        int roleChoice = getValidIntInput(1, 3);

        switch (roleChoice) {
            case 1:
                role = "student";
                break;
            case 2:
                role = "staff";
                break;
            case 3:
                role = "faculty";
                break;
        }
        break;
    }

    // Confirm user data
    cout << "\n========== CONFIRM USER DATA ==========\n";
    cout << "User ID: " << id << "\n";
    cout << "Name: " << name << "\n";
    cout << "Role: " << role << "\n";
    cout << "======================================\n";

    char confirm;
    cout << "Add this user? (y/n): ";
    cin >> confirm;
    clearInputBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        system.addUser(id, name, role);
        cout << "✓ Success: User successfully added and saved to system!\n";
    } else {
        cout << "User addition cancelled.\n";
    }
}

void scanCardInterface(RFIDSystem& system) {
    string userId;
    cout << "\n========== RFID CARD SCANNER ==========\n";
    cout << "Enter User ID to scan: ";
    getline(cin, userId);
    userId = trim(userId);

    if (userId.empty()) {
        cout << "Error: User ID cannot be empty.\n";
    } else {
        system.scanRFID(userId);
    }
}

void searchUserLogs(RFIDSystem& system) {
    string userId;
    cout << "\n========== SEARCH USER LOGS ==========\n";
    cout << "Enter User ID to search: ";
    getline(cin, userId);
    userId = trim(userId);

    if (userId.empty()) {
        cout << "Error: User ID cannot be empty.\n";
        return;
    }

    auto userLogs = system.searchLogsByUserId(userId);
    if (userLogs.empty()) {
        cout << "No logs found for user: " << userId << endl;
    } else {
        cout << "\n========== LOGS FOR USER: " << userId << " ==========\n";
        cout << left << setw(8) << "Action" << "Timestamp\n";
        cout << "======================================\n";
        for (const auto& log : userLogs) {
            cout << left << setw(8) << log.action
                      << log.getFormattedTime() << "\n";
        }
        cout << "Total entries: " << userLogs.size() << "\n";
    }
}

bool confirmAction(const string& message) {
    char confirm;
    cout << message << " (y/n): ";
    cin >> confirm;
    clearInputBuffer();
    return (confirm == 'y' || confirm == 'Y');
}

bool confirmDangerousAction(const string& message, const string& confirmationText) {
    if (!confirmAction(message)) {
        return false;
    }

    cout << "Type '" << confirmationText << "' to confirm: ";
    string confirmation;
    getline(cin, confirmation);

    return confirmation == confirmationText;
}

void saveAndExit(RFIDSystem& system) {
    cout << "\nSaving system data before exit...\n";
    if (system.saveAllData()) {
        cout << "✓ System data saved successfully!\n";
    } else {
        cout << "✗ Warning: Failed to save some system data!\n";
    }

    if (system.exportToJSON()) {
        cout << "✓ JSON export completed successfully!\n";
    } else {
        cout << "✗ Warning: Failed to export JSON data!\n";
    }

    cout << "========== GOODBYE! ==========\n";
}

void runAdminMode(RFIDSystem& system) {
    while (true) {
        displayAdminMenu();
        int choice = getValidIntInput(0, 12);

        switch (choice) {
            case 1:
                addUserInterface(system);
                break;

            case 2:
                scanCardInterface(system);
                break;

            case 3:
                searchUserLogs(system);
                break;

            case 4:
                system.displayAllLogs();
                break;

            case 5:
                system.displayUserStatus();
                break;

            case 6:
                system.displayDailyReport();
                break;

            case 7:
                system.displayAllUsers();
                break;

            case 8:
                cout << "\nSaving system data...\n";
                if (system.saveAllData()) {
                    cout << "✓ System data saved successfully!\n";
                } else {
                    cout << "✗ Failed to save system data.\n";
                }
                break;

            case 9:
                cout << "\nExporting system data to JSON...\n";
                if (system.exportToJSON()) {
                    cout << "✓ System data exported successfully!\n";
                } else {
                    cout << "✗ Failed to export system data.\n";
                }
                break;

            case 10:
                if (confirmAction("\nWarning: This will clear all daily logs but keep users!\nAre you sure?")) {
                    system.clearDailyLogs();
                    cout << "✓ Daily logs cleared successfully!\n";
                } else {
                    cout << "Operation cancelled.\n";
                }
                break;

            case 11:
                if (confirmDangerousAction("\nWARNING: This will delete ALL data (users and logs)!\nThis action cannot be undone. Are you absolutely sure?", "DELETE ALL")) {
                    system.clearAllData();
                    cout << "✓ All system data cleared successfully!\n";
                } else {
                    cout << "Confirmation failed. Operation cancelled.\n";
                }
                break;

            case 12:
                cout << "Logging out of admin panel...\n";
                return; // Return to main menu

            case 0:
                saveAndExit(system);
                exit(0);

            default:
                cout << "Error: Invalid option. Please choose a number between 0-12.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }
}

void runUserMode(RFIDSystem& system) {
    cout << "\n========== USER MODE ACTIVATED ==========\n";
    cout << "Welcome! You can scan RFID cards to log in/out.\n";
    cout << "========================================\n";

    while (true) {
        displayUserMenu();
        int choice = getValidIntInput(0, 2);

        switch (choice) {
            case 1:
                scanCardInterface(system);
                break;

            case 2:
                cout << "Returning to main menu...\n";
                return;

            case 0:
                saveAndExit(system);
                exit(0);

            default:
                cout << "Error: Invalid option. Please choose 0, 1, or 2.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }
}

int main() {
    RFIDSystem system;

    cout << "========== RFID LAB SYSTEM ==========\n";
    cout << "Users: " << system.getTotalUsers() << ", Logs: " << system.getTotalScans() << "\n";
    cout << "=====================================\n";

    while (true) {
        displayLoginMenu();
        int loginChoice = getValidIntInput(0, 2);

        switch (loginChoice) {
            case 1: {
                UserType userType = authenticateAdmin();
                if (userType == UserType::ADMIN) {
                    runAdminMode(system);
                }
                break;
            }

            case 2:
                runUserMode(system);
                break;

            case 0:
                saveAndExit(system);
                return 0;

            default:
                cout << "Error: Invalid option. Please choose 0, 1, or 2.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    return 0;
}
