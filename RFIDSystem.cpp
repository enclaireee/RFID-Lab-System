#include "RFIDSystem.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

RFIDSystem::RFIDSystem() {
    createDataDirectory();

    if (!loadAllData()) {
        std::cout << "No system data found, loading legacy logs only..." << std::endl;
        loadFromBinaryFile();
    }

    for (const auto& user : users) {
        if (userStatus.find(user.id) == userStatus.end()) {
            userStatus[user.id] = "OUT";
        }
    }
}

void RFIDSystem::createDataDirectory() {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        if (mkdir("data", 0755) != 0) {
            std::cerr << "Error creating data directory" << std::endl;
        }
    }
}

void RFIDSystem::addUser(const std::string& id, const std::string& name, const std::string& role) {
    users.emplace_back(id, name, role);
    userStatus[id] = "OUT";
    std::cout << "User added: " << name << " (" << id << ") - " << role << std::endl;

    // Auto-save after adding user
    saveAllData();
}

User* RFIDSystem::findUser(const std::string& id) {
    for (auto& user : users) {
        if (user.id == id) {
            return &user;
        }
    }
    return nullptr;
}

bool RFIDSystem::scanRFID(const std::string& userId) {
    User* user = findUser(userId);
    if (!user) {
        std::cout << "ERROR: User ID " << userId << " not found!" << std::endl;
        return false;
    }

    // Determine action based on current status
    std::string action = (userStatus[userId] == "OUT") ? "IN" : "OUT";
    userStatus[userId] = action;

    // Create and add scan log
    ScanLog log(userId, user->name, action);
    dailyLogs.push_back(log);

    std::cout << "SCAN SUCCESS: " << user->name << " (" << userId << ") - "
              << action << " at " << log.getFormattedTime() << std::endl;

    // Auto-save after scan
    saveAllData();
    return true;
}

std::vector<ScanLog> RFIDSystem::searchLogsByUserId(const std::string& userId) {
    std::vector<ScanLog> userLogs;

    for (const auto& log : dailyLogs) {
        if (log.userId == userId) {
            userLogs.push_back(log);
        }
    }

    std::sort(userLogs.begin(), userLogs.end());
    return userLogs;
}

std::vector<ScanLog> RFIDSystem::getSortedLogs() {
    std::vector<ScanLog> sortedLogs = dailyLogs;
    std::sort(sortedLogs.begin(), sortedLogs.end());
    return sortedLogs;
}

bool RFIDSystem::saveAllData(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    // Write file version for future compatibility
    uint32_t version = 1;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // Write number of users
    size_t userCount = users.size();
    file.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));

    // Write each user
    for (const auto& user : users) {
        // Write user ID
        size_t idLen = user.id.length();
        file.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
        file.write(user.id.c_str(), idLen);

        // Write user name
        size_t nameLen = user.name.length();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(user.name.c_str(), nameLen);

        // Write user role
        size_t roleLen = user.role.length();
        file.write(reinterpret_cast<const char*>(&roleLen), sizeof(roleLen));
        file.write(user.role.c_str(), roleLen);

        // Write user status
        std::string status = userStatus.at(user.id);
        size_t statusLen = status.length();
        file.write(reinterpret_cast<const char*>(&statusLen), sizeof(statusLen));
        file.write(status.c_str(), statusLen);
    }

    // Write number of logs
    size_t logCount = dailyLogs.size();
    file.write(reinterpret_cast<const char*>(&logCount), sizeof(logCount));

    // Write each log
    for (const auto& log : dailyLogs) {
        // Write userId
        size_t userIdLen = log.userId.length();
        file.write(reinterpret_cast<const char*>(&userIdLen), sizeof(userIdLen));
        file.write(log.userId.c_str(), userIdLen);

        // Write userName
        size_t userNameLen = log.userName.length();
        file.write(reinterpret_cast<const char*>(&userNameLen), sizeof(userNameLen));
        file.write(log.userName.c_str(), userNameLen);

        // Write action
        size_t actionLen = log.action.length();
        file.write(reinterpret_cast<const char*>(&actionLen), sizeof(actionLen));
        file.write(log.action.c_str(), actionLen);

        // Write timestamp
        file.write(reinterpret_cast<const char*>(&log.timestamp), sizeof(log.timestamp));
    }

    file.close();
    std::cout << "System data saved: " << users.size() << " users, " << dailyLogs.size() << " logs" << std::endl;
    return true;
}

bool RFIDSystem::loadAllData(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        // File doesn't exist yet, not an error for first run
        return false;
    }

    users.clear();
    dailyLogs.clear();
    userStatus.clear();

    // Read and verify file version
    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != 1) {
        std::cerr << "Unsupported file version: " << version << std::endl;
        file.close();
        return false;
    }

    // Read number of users
    size_t userCount;
    file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));

    // Read each user
    for (size_t i = 0; i < userCount; ++i) {
        User user;
        std::string status;

        // Read user ID
        size_t idLen;
        file.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));
        user.id.resize(idLen);
        file.read(&user.id[0], idLen);

        // Read user name
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        user.name.resize(nameLen);
        file.read(&user.name[0], nameLen);

        // Read user role
        size_t roleLen;
        file.read(reinterpret_cast<char*>(&roleLen), sizeof(roleLen));
        user.role.resize(roleLen);
        file.read(&user.role[0], roleLen);

        // Read user status
        size_t statusLen;
        file.read(reinterpret_cast<char*>(&statusLen), sizeof(statusLen));
        status.resize(statusLen);
        file.read(&status[0], statusLen);

        users.push_back(user);
        userStatus[user.id] = status;
    }

    // Read number of logs
    size_t logCount;
    file.read(reinterpret_cast<char*>(&logCount), sizeof(logCount));

    // Read each log
    for (size_t i = 0; i < logCount; ++i) {
        ScanLog log;

        // Read userId
        size_t userIdLen;
        file.read(reinterpret_cast<char*>(&userIdLen), sizeof(userIdLen));
        log.userId.resize(userIdLen);
        file.read(&log.userId[0], userIdLen);

        // Read userName
        size_t userNameLen;
        file.read(reinterpret_cast<char*>(&userNameLen), sizeof(userNameLen));
        log.userName.resize(userNameLen);
        file.read(&log.userName[0], userNameLen);

        // Read action
        size_t actionLen;
        file.read(reinterpret_cast<char*>(&actionLen), sizeof(actionLen));
        log.action.resize(actionLen);
        file.read(&log.action[0], actionLen);

        // Read timestamp
        file.read(reinterpret_cast<char*>(&log.timestamp), sizeof(log.timestamp));

        dailyLogs.push_back(log);
    }

    file.close();
    std::cout << "System data loaded: " << users.size() << " users, " << dailyLogs.size() << " logs" << std::endl;
    return true;
}

std::string getCurrentTimeString() {
    std::time_t now = std::time(nullptr);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string escapeJsonString(const std::string& input) {
    std::string escaped;
    for (char c : input) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

bool RFIDSystem::exportToJSON(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error opening file for JSON export: " << filename << std::endl;
        return false;
    }

    file << "{\n";

    // Export users
    file << "  \"users\": [\n";
    for (size_t i = 0; i < users.size(); ++i) {
        const auto& user = users[i];
        file << "    {\n";
        file << "      \"id\": \"" << escapeJsonString(user.id) << "\",\n";
        file << "      \"name\": \"" << escapeJsonString(user.name) << "\",\n";
        file << "      \"role\": \"" << escapeJsonString(user.role) << "\",\n";
        file << "      \"status\": \"" << escapeJsonString(userStatus.at(user.id)) << "\"\n";
        file << "    }";
        if (i < users.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    // Export logs
    file << "  \"daily_logs\": [\n";
    auto sortedLogs = getSortedLogs();
    for (size_t i = 0; i < sortedLogs.size(); ++i) {
        const auto& log = sortedLogs[i];
        file << "    {\n";
        file << "      \"user_id\": \"" << escapeJsonString(log.userId) << "\",\n";
        file << "      \"user_name\": \"" << escapeJsonString(log.userName) << "\",\n";
        file << "      \"action\": \"" << escapeJsonString(log.action) << "\",\n";
        file << "      \"timestamp\": \"" << log.getFormattedTime() << "\",\n";
        file << "      \"unix_timestamp\": " << log.timestamp << "\n";
        file << "    }";
        if (i < sortedLogs.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    // Export summary
    file << "  \"summary\": {\n";
    file << "    \"total_users\": " << users.size() << ",\n";
    file << "    \"total_scans\": " << dailyLogs.size() << ",\n";
    file << "    \"export_time\": \"" << getCurrentTimeString() << "\"\n";
    file << "  }\n";
    file << "}\n";

    file.close();
    std::cout << "Complete system data exported to JSON: " << filename << std::endl;
    return true;
}

// Legacy methods for backward compatibility
bool RFIDSystem::saveToBinaryFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    // Write number of logs
    size_t logCount = dailyLogs.size();
    file.write(reinterpret_cast<const char*>(&logCount), sizeof(logCount));

    // Write each log
    for (const auto& log : dailyLogs) {
        size_t userIdLen = log.userId.length();
        file.write(reinterpret_cast<const char*>(&userIdLen), sizeof(userIdLen));
        file.write(log.userId.c_str(), userIdLen);

        size_t userNameLen = log.userName.length();
        file.write(reinterpret_cast<const char*>(&userNameLen), sizeof(userNameLen));
        file.write(log.userName.c_str(), userNameLen);

        size_t actionLen = log.action.length();
        file.write(reinterpret_cast<const char*>(&actionLen), sizeof(actionLen));
        file.write(log.action.c_str(), actionLen);

        file.write(reinterpret_cast<const char*>(&log.timestamp), sizeof(log.timestamp));
    }

    file.close();
    std::cout << "Daily logs saved to binary file: " << filename << std::endl;
    return true;
}

bool RFIDSystem::loadFromBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return true; // File doesn't exist, not an error
    }

    dailyLogs.clear();

    size_t logCount;
    file.read(reinterpret_cast<char*>(&logCount), sizeof(logCount));

    for (size_t i = 0; i < logCount; ++i) {
        ScanLog log;

        size_t userIdLen;
        file.read(reinterpret_cast<char*>(&userIdLen), sizeof(userIdLen));
        log.userId.resize(userIdLen);
        file.read(&log.userId[0], userIdLen);

        size_t userNameLen;
        file.read(reinterpret_cast<char*>(&userNameLen), sizeof(userNameLen));
        log.userName.resize(userNameLen);
        file.read(&log.userName[0], userNameLen);

        size_t actionLen;
        file.read(reinterpret_cast<char*>(&actionLen), sizeof(actionLen));
        log.action.resize(actionLen);
        file.read(&log.action[0], actionLen);

        file.read(reinterpret_cast<char*>(&log.timestamp), sizeof(log.timestamp));
        dailyLogs.push_back(log);
    }

    file.close();
    std::cout << "Loaded " << logCount << " logs from legacy binary file." << std::endl;
    return true;
}

void RFIDSystem::displayAllLogs() {
    auto sortedLogs = getSortedLogs();

    std::cout << "\n=== ALL SCAN LOGS (Sorted by Time) ===\n";
    std::cout << std::left << std::setw(12) << "User ID"
              << std::setw(20) << "Name"
              << std::setw(8) << "Action"
              << "Timestamp\n";
    std::cout << std::string(60, '-') << "\n";

    for (const auto& log : sortedLogs) {
        std::cout << std::left << std::setw(12) << log.userId
                  << std::setw(20) << log.userName
                  << std::setw(8) << log.action
                  << log.getFormattedTime() << "\n";
    }
    std::cout << "\nTotal scans: " << sortedLogs.size() << "\n";
}

void RFIDSystem::displayAllUsers() {
    std::cout << "\n=== ALL REGISTERED USERS ===\n";
    std::cout << std::left << std::setw(12) << "User ID"
              << std::setw(20) << "Name"
              << std::setw(10) << "Role"
              << "Registration\n";
    std::cout << std::string(50, '-') << "\n";

    for (const auto& user : users) {
        std::cout << std::left << std::setw(12) << user.id
                  << std::setw(20) << user.name
                  << std::setw(10) << user.role
                  << "Active\n";
    }
    std::cout << "\nTotal users: " << users.size() << "\n";
}

void RFIDSystem::displayUserStatus() {
    std::cout << "\n=== CURRENT USER STATUS ===\n";
    std::cout << std::left << std::setw(12) << "User ID"
              << std::setw(20) << "Name"
              << std::setw(10) << "Role"
              << "Status\n";
    std::cout << std::string(50, '-') << "\n";

    for (const auto& user : users) {
        std::cout << std::left << std::setw(12) << user.id
                  << std::setw(20) << user.name
                  << std::setw(10) << user.role
                  << userStatus[user.id] << "\n";
    }
}

void RFIDSystem::displayDailyReport() {
    std::cout << "\n=== DAILY ATTENDANCE REPORT ===\n";

    std::map<std::string, int> userScanCount;
    std::map<std::string, std::string> lastAction;

    for (const auto& log : dailyLogs) {
        userScanCount[log.userId]++;
        lastAction[log.userId] = log.action;
    }

    std::cout << std::left << std::setw(12) << "User ID"
              << std::setw(20) << "Name"
              << std::setw(12) << "Total Scans"
              << "Last Action\n";
    std::cout << std::string(60, '-') << "\n";

    for (const auto& user : users) {
        int scans = userScanCount[user.id];
        std::string action = lastAction[user.id].empty() ? "NONE" : lastAction[user.id];

        std::cout << std::left << std::setw(12) << user.id
                  << std::setw(20) << user.name
                  << std::setw(12) << scans
                  << action << "\n";
    }
}

void RFIDSystem::clearDailyLogs() {
    dailyLogs.clear();
    for (auto& status : userStatus) {
        status.second = "OUT";
    }
    saveAllData(); // Auto-save after clearing
    std::cout << "Daily logs cleared and all users set to OUT status.\n";
}

void RFIDSystem::clearAllData() {
    users.clear();
    dailyLogs.clear();
    userStatus.clear();
    saveAllData(); // Save empty state
    std::cout << "All system data cleared (users and logs).\n";
}
