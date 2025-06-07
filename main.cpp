#include "RFIDSystem.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

enum class UserType {
    ADMIN,
    USER,
    INVALID
};

void displayLoginMenu() {
    std::cout << "\n========== RFID LAB SYSTEM LOGIN ==========\n";
    std::cout << "1. Admin Login\n";
    std::cout << "2. User Mode\n";
    std::cout << "0. Exit System\n";
    std::cout << "==========================================\n";
    std::cout << "Select login type: ";
}

void displayAdminMenu() {
    std::cout << "\n========== ADMIN PANEL ==========\n";
    std::cout << "1. Add New User\n";
    std::cout << "2. Scan Card\n";
    std::cout << "3. Search User Logs\n";
    std::cout << "4. Display All Logs\n";
    std::cout << "5. Display User Status\n";
    std::cout << "6. Display Daily Report\n";
    std::cout << "7. Display All Users\n";
    std::cout << "8. Save System Data\n";
    std::cout << "9. Export to JSON\n";
    std::cout << "10. Clear Daily Logs\n";
    std::cout << "11. Clear All Data\n";
    std::cout << "12. Logout to Main Menu\n";
    std::cout << "0. Exit System\n";
    std::cout << "================================\n";
    std::cout << "Enter your Choice: ";
}

void displayUserMenu() {
    std::cout << "\n========== USER INTERFACE ==========\n";
    std::cout << "1. Scan RFID Card\n";
    std::cout << "2. Back to Main Menu\n";
    std::cout << "0. Exit System\n";
    std::cout << "===================================\n";
    std::cout << "Enter your Choice: ";
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool isValidUserId(const std::string& id) {
    if (id.length() < 3 || id.length() > 10) return false;
    for (char c : id) {
        if (!std::isalnum(c)) return false;
    }
    return true;
}

bool isValidName(const std::string& name) {
    if (name.length() < 2 || name.length() > 50) return false;
    for (char c : name) {
        if (!std::isalpha(c) && c != ' ' && c != '.' && c != '-' && c != '\'') return false;
    }
    // Check that name doesn't start or end with space
    if (name.front() == ' ' || name.back() == ' ') return false;
    return true;
}

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidIntInput(int min, int max) {
    int input;
    while (true) {
        if (std::cin >> input) {
            if (input >= min && input <= max) {
                std::cin.ignore(); // Clear the newline
                return input;
            } else {
                std::cout << "Error: Please enter a number between " << min << " and " << max << ": ";
            }
        } else {
            clearInputBuffer();
            std::cout << "Error: Invalid input. Please enter a number: ";
        }
    }
}

UserType authenticateAdmin() {
    std::string username, password;
    const std::string ADMIN_USERNAME = "testAdmin";
    const std::string ADMIN_PASSWORD = "alprog05";

    std::cout << "\n========== ADMIN LOGIN ==========\n";
    std::cout << "Username: ";
    std::getline(std::cin, username);
    username = trim(username);

    std::cout << "Password: ";
    std::getline(std::cin, password);
    password = trim(password);

    if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
        std::cout << "✓ Admin login successful!\n";
        return UserType::ADMIN;
    } else {
        std::cout << "✗ Invalid credentials! Access denied.\n";
        return UserType::INVALID;
    }
}

void addUserInterface(RFIDSystem& system) {
    std::cout << "\n========== ADD NEW USER ==========\n";

    std::string id, name, role;

    // Get User ID with validation
    while (true) {
        std::cout << "Enter User ID (3-10 alphanumeric characters): ";
        std::getline(std::cin, id);
        id = trim(id);

        if (id.empty()) {
            std::cout << "Error: User ID cannot be empty.\n";
            continue;
        }

        if (!isValidUserId(id)) {
            std::cout << "Error: Invalid User ID format. Use 3-10 alphanumeric characters only.\n";
            continue;
        }

        if (system.findUser(id) != nullptr) {
            std::cout << "Error: User ID '" << id << "' already exists. Please choose a different ID.\n";
            continue;
        }

        break;
    }

    // Get Name with validation
    while (true) {
        std::cout << "Enter Full Name (2-50 characters): ";
        std::getline(std::cin, name);
        name = trim(name);

        if (name.empty()) {
            std::cout << "Error: Name cannot be empty.\n";
            continue;
        }

        if (!isValidName(name)) {
            std::cout << "Error: Invalid name format. Use letters, spaces, dots, hyphens, and apostrophes only.\n";
            continue;
        }

        break;
    }

    // Get Role with validation
    while (true) {
        std::cout << "\nSelect User Role:\n";
        std::cout << "1. Student\n";
        std::cout << "2. Staff\n";
        std::cout << "3. Faculty\n";
        std::cout << "Enter choice (1-3): ";

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
    std::cout << "\n========== CONFIRM USER DATA ==========\n";
    std::cout << "User ID: " << id << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Role: " << role << "\n";
    std::cout << "======================================\n";

    char confirm;
    std::cout << "Add this user? (y/n): ";
    std::cin >> confirm;
    clearInputBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        system.addUser(id, name, role);
        std::cout << "✓ Success: User successfully added and saved to system!\n";
    } else {
        std::cout << "User addition cancelled.\n";
    }
}

void scanCardInterface(RFIDSystem& system) {
    std::string userId;
    std::cout << "\n========== RFID CARD SCANNER ==========\n";
    std::cout << "Enter User ID to scan: ";
    std::getline(std::cin, userId);
    userId = trim(userId);

    if (userId.empty()) {
        std::cout << "Error: User ID cannot be empty.\n";
    } else {
        system.scanRFID(userId);
    }
}

void searchUserLogs(RFIDSystem& system) {
    std::string userId;
    std::cout << "\n========== SEARCH USER LOGS ==========\n";
    std::cout << "Enter User ID to search: ";
    std::getline(std::cin, userId);
    userId = trim(userId);

    if (userId.empty()) {
        std::cout << "Error: User ID cannot be empty.\n";
        return;
    }

    auto userLogs = system.searchLogsByUserId(userId);
    if (userLogs.empty()) {
        std::cout << "No logs found for user: " << userId << std::endl;
    } else {
        std::cout << "\n========== LOGS FOR USER: " << userId << " ==========\n";
        std::cout << std::left << std::setw(8) << "Action" << "Timestamp\n";
        std::cout << "======================================\n";
        for (const auto& log : userLogs) {
            std::cout << std::left << std::setw(8) << log.action
                      << log.getFormattedTime() << "\n";
        }
        std::cout << "Total entries: " << userLogs.size() << "\n";
    }
}

bool confirmAction(const std::string& message) {
    char confirm;
    std::cout << message << " (y/n): ";
    std::cin >> confirm;
    clearInputBuffer();
    return (confirm == 'y' || confirm == 'Y');
}

bool confirmDangerousAction(const std::string& message, const std::string& confirmationText) {
    if (!confirmAction(message)) {
        return false;
    }

    std::cout << "Type '" << confirmationText << "' to confirm: ";
    std::string confirmation;
    std::getline(std::cin, confirmation);

    return confirmation == confirmationText;
}

void saveAndExit(RFIDSystem& system) {
    std::cout << "\nSaving system data before exit...\n";
    if (system.saveAllData()) {
        std::cout << "✓ System data saved successfully!\n";
    } else {
        std::cout << "✗ Warning: Failed to save some system data!\n";
    }

    if (system.exportToJSON()) {
        std::cout << "✓ JSON export completed successfully!\n";
    } else {
        std::cout << "✗ Warning: Failed to export JSON data!\n";
    }

    std::cout << "========== GOODBYE! ==========\n";
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
                std::cout << "\nSaving system data...\n";
                if (system.saveAllData()) {
                    std::cout << "✓ System data saved successfully!\n";
                } else {
                    std::cout << "✗ Failed to save system data.\n";
                }
                break;

            case 9:
                std::cout << "\nExporting system data to JSON...\n";
                if (system.exportToJSON()) {
                    std::cout << "✓ System data exported successfully!\n";
                } else {
                    std::cout << "✗ Failed to export system data.\n";
                }
                break;

            case 10:
                if (confirmAction("\nWarning: This will clear all daily logs but keep users!\nAre you sure?")) {
                    system.clearDailyLogs();
                    std::cout << "✓ Daily logs cleared successfully!\n";
                } else {
                    std::cout << "Operation cancelled.\n";
                }
                break;

            case 11:
                if (confirmDangerousAction("\nWARNING: This will delete ALL data (users and logs)!\nThis action cannot be undone. Are you absolutely sure?", "DELETE ALL")) {
                    system.clearAllData();
                    std::cout << "✓ All system data cleared successfully!\n";
                } else {
                    std::cout << "Confirmation failed. Operation cancelled.\n";
                }
                break;

            case 12:
                std::cout << "Logging out of admin panel...\n";
                return; // Return to main menu

            case 0:
                saveAndExit(system);
                exit(0);

            default:
                std::cout << "Error: Invalid option. Please choose a number between 0-12.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void runUserMode(RFIDSystem& system) {
    std::cout << "\n========== USER MODE ACTIVATED ==========\n";
    std::cout << "Welcome! You can scan RFID cards to log in/out.\n";
    std::cout << "========================================\n";

    while (true) {
        displayUserMenu();
        int choice = getValidIntInput(0, 2);

        switch (choice) {
            case 1:
                scanCardInterface(system);
                break;

            case 2:
                std::cout << "Returning to main menu...\n";
                return;

            case 0:
                saveAndExit(system);
                exit(0);

            default:
                std::cout << "Error: Invalid option. Please choose 0, 1, or 2.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

int main() {
    RFIDSystem system;

    std::cout << "========== RFID LAB SYSTEM ==========\n";
    std::cout << "Users: " << system.getTotalUsers() << ", Logs: " << system.getTotalScans() << "\n";
    std::cout << "=====================================\n";

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
                std::cout << "Error: Invalid option. Please choose 0, 1, or 2.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    return 0;
}
