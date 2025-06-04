#include "RFIDSystem.h"
#include <iostream>
#include <string>

void displayMenu() {
    std::cout << "\n========== RFID LAB SYSTEM ==========\n";
    std::cout << "1. Add New User\n";
    std::cout << "2. Scan Card\n";
    std::cout << "3. Search Logs\n";
    std::cout << "4. Display All Logs\n";
    std::cout << "5. Display User Status\n";
    std::cout << "6. Display Daily Report\n";
    std::cout << "7. Save Data to Binary File\n";
    std::cout << "8. Export Data to JSON\n";
    std::cout << "9. Clear Daily Logs\n";
    std::cout << "0. Exit System\n";
    std::cout << "===============================================\n";
    std::cout << "Enter your Choice: ";
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
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
        if (!std::isalpha(c) && c != ' ' && c != '.' && c != '-') return false;
    }
    return true;
}

void addUserInterface(RFIDSystem& system) {
    std::cout << "\n========== ADD NEW USER ==========\n";

    std::string id, name, role;
    int roleChoice;

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

    while (true) {
        std::cout << "Enter Full Name (2-50 characters): ";
        std::getline(std::cin, name);
        name = trim(name);

        if (name.empty()) {
            std::cout << "Error: Name cannot be empty.\n";
            continue;
        }

        if (!isValidName(name)) {
            std::cout << "Error: Invalid name format. Use letters, spaces, dots, and hyphens only.\n";
            continue;
        }

        break;
    }

    while (true) {
        std::cout << "\nSelect User Role:\n";
        std::cout << "1. Student\n";
        std::cout << "2. Staff\n";
        std::cout << "3. Faculty\n";
        std::cout << "Enter choice: ";

        if (!(std::cin >> roleChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Error: Invalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore();

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
            default:
                std::cout << "Error: Invalid choice. Please select 1, 2, or 3.\n";
                continue;
        }
        break;
    }


    std::cout << "\n========== CONFIRM USER DATA ==========\n";
    std::cout << "User ID: " << id << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Role: " << role << "\n";
    std::cout << "======================================\n";

    char confirm;
    std::cout << "Add this user? (y/n): ";
    std::cin >> confirm;
    std::cin.ignore();

    if (confirm == 'y' || confirm == 'Y') {
        system.addUser(id, name, role);
        std::cout << "Success: User successfully added to the system!\n";
    } else {
        std::cout << "User addition cancelled.\n";
    }
}

int main() {
    RFIDSystem system;

    std::cout << "========== RFID LAB SYSTEM ==========\n";
    std::cout << "Loading previous data...\n";
    std::cout << "=====================================\n";

    int choice;

    while (true) {
        displayMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Error: Invalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore();

        switch (choice) {
            case 1: {
                addUserInterface(system);
                break;
            }

            case 2: {
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
                break;
            }

            case 3: {
                std::string userId;
                std::cout << "\n========== SEARCH USER LOGS ==========\n";
                std::cout << "Enter User ID to search: ";
                std::getline(std::cin, userId);
                userId = trim(userId);

                if (userId.empty()) {
                    std::cout << "Error: User ID cannot be empty.\n";
                    break;
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
                break;
            }

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
                std::cout << "\nSaving data to binary file...\n";
                if (system.saveToBinaryFile()) {
                    std::cout << "Data saved successfully!\n";
                } else {
                    std::cout << "Failed to save data.\n";
                }
                break;

            case 8:
                std::cout << "\nExporting data to JSON...\n";
                if (system.exportToJSON()) {
                    std::cout << "Data exported successfully!\n";
                } else {
                    std::cout << "Failed to export data.\n";
                }
                break;

            case 9: {
                char confirm;
                std::cout << "\nWarning: This will clear all daily logs!\n";
                std::cout << "Are you sure? (y/n): ";
                std::cin >> confirm;
                std::cin.ignore();

                if (confirm == 'y' || confirm == 'Y') {
                    system.clearDailyLogs();
                    std::cout << "Daily logs cleared successfully!\n";
                } else {
                    std::cout << "Operation cancelled.\n";
                }
                break;
            }

            case 0:
                std::cout << "\nSaving data before exit...\n";
                system.saveToBinaryFile();
                system.exportToJSON();
                std::cout << "========== GOODBYE! ==========\n";
                return 0;

            default:
                std::cout << "Error: Invalid option. Please choose a number between 0-9.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    return 0;
}
