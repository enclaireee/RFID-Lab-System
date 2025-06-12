#include "RFIDSystem.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

using namespace std;

RFIDSystem::RFIDSystem() {
    createDataDirectory();

    if (!loadSystemData()) {
        cout << "No system data found, starting with empty system..." << endl;
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
            cerr << "Error creating data directory" << endl;
        }
    }
}

void RFIDSystem::addUser(const string& id, const string& name, const string& role) {
    users.emplace_back(id, name, role);
    userStatus[id] = "OUT";
    cout << "User added: " << name << " (" << id << ") - " << role << endl;

    saveSystemData();
}

User* RFIDSystem::findUser(const string& id) {
    for (auto& user : users) {
        if (user.id == id) {
            return &user;
        }
    }
    return nullptr;
}

bool RFIDSystem::scanRFID(const string& userId) {
    User* user = findUser(userId);
    if (!user) {
        cout << "ERROR: User ID " << userId << " not found!" << endl;
        return false;
    }
    string action = (userStatus[userId] == "OUT") ? "IN" : "OUT";
    userStatus[userId] = action;

    ScanLog log(userId, user->name, action);
    dailyLogs.push_back(log);

    cout << "SCAN SUCCESS: " << user->name << " (" << userId << ") - "
              << action << " at " << log.getFormattedTime() << endl;

    saveSystemData();
    return true;
}

vector<ScanLog> RFIDSystem::searchLogsByUserId(const string& userId) {
    vector<ScanLog> userLogs;

    for (const auto& log : dailyLogs) {
        if (log.userId == userId) {
            userLogs.push_back(log);
        }
    }

    sort(userLogs.begin(), userLogs.end());
    return userLogs;
}

vector<ScanLog> RFIDSystem::getSortedLogs() {
    vector<ScanLog> sortedLogs = dailyLogs;
    sort(sortedLogs.begin(), sortedLogs.end());
    return sortedLogs;
}

bool RFIDSystem::saveSystemData() {
    ofstream binFile("data/system_data.bin", ios::binary);
    if (!binFile) {
        cerr << "Error saving binary data file" << endl;
        return false;
    }

    uint32_t version = 1;
    binFile.write(reinterpret_cast<const char*>(&version), sizeof(version));

    size_t userCount = users.size();
    binFile.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));

    for (const auto& user : users) {
        size_t idLen = user.id.length();
        binFile.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
        binFile.write(user.id.c_str(), idLen);

        size_t nameLen = user.name.length();
        binFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        binFile.write(user.name.c_str(), nameLen);

        size_t roleLen = user.role.length();
        binFile.write(reinterpret_cast<const char*>(&roleLen), sizeof(roleLen));
        binFile.write(user.role.c_str(), roleLen);

        string status = userStatus.at(user.id);
        size_t statusLen = status.length();
        binFile.write(reinterpret_cast<const char*>(&statusLen), sizeof(statusLen));
        binFile.write(status.c_str(), statusLen);
    }

    size_t logCount = dailyLogs.size();
    binFile.write(reinterpret_cast<const char*>(&logCount), sizeof(logCount));

    for (const auto& log : dailyLogs) {
        size_t userIdLen = log.userId.length();
        binFile.write(reinterpret_cast<const char*>(&userIdLen), sizeof(userIdLen));
        binFile.write(log.userId.c_str(), userIdLen);

        size_t userNameLen = log.userName.length();
        binFile.write(reinterpret_cast<const char*>(&userNameLen), sizeof(userNameLen));
        binFile.write(log.userName.c_str(), userNameLen);

        size_t actionLen = log.action.length();
        binFile.write(reinterpret_cast<const char*>(&actionLen), sizeof(actionLen));
        binFile.write(log.action.c_str(), actionLen);

        binFile.write(reinterpret_cast<const char*>(&log.timestamp), sizeof(log.timestamp));
    }

    binFile.close();
    cout << "Binary data saved: " << users.size() << " users, " << dailyLogs.size() << " logs" << endl;
    return true;
}

bool RFIDSystem::loadSystemData() {
    ifstream file("data/system_data.bin", ios::binary);
    if (!file) {
        return false;
    }

    users.clear();
    dailyLogs.clear();
    userStatus.clear();

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != 1) {
        cerr << "Unsupported file version: " << version << endl;
        file.close();
        return false;
    }

    size_t userCount;
    file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));

    for (size_t i = 0; i < userCount; ++i) {
        User user;
        string status;

        size_t idLen;
        file.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));
        user.id.resize(idLen);
        file.read(&user.id[0], idLen);

        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        user.name.resize(nameLen);
        file.read(&user.name[0], nameLen);

        size_t roleLen;
        file.read(reinterpret_cast<char*>(&roleLen), sizeof(roleLen));
        user.role.resize(roleLen);
        file.read(&user.role[0], roleLen);

        size_t statusLen;
        file.read(reinterpret_cast<char*>(&statusLen), sizeof(statusLen));
        status.resize(statusLen);
        file.read(&status[0], statusLen);

        users.push_back(user);
        userStatus[user.id] = status;
    }

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
    cout << "System data loaded: " << users.size() << " users, " << dailyLogs.size() << " logs" << endl;
    return true;
}

bool RFIDSystem::saveAllData() {
    bool binarySuccess = saveSystemData();
    bool jsonSuccess = exportToJSON();
    return binarySuccess && jsonSuccess;
}

bool RFIDSystem::exportToJSON() {
    ofstream jsonFile("data/system_data.json");
    if (!jsonFile) {
        cerr << "Error creating JSON export file" << endl;
        return false;
    }

    jsonFile << "{\n";

    jsonFile << "  \"users\": [\n";
    for (size_t i = 0; i < users.size(); ++i) {
        const auto& user = users[i];
        jsonFile << "    {\n";
        jsonFile << "      \"id\": \"" << escapeJsonString(user.id) << "\",\n";
        jsonFile << "      \"name\": \"" << escapeJsonString(user.name) << "\",\n";
        jsonFile << "      \"role\": \"" << escapeJsonString(user.role) << "\",\n";
        jsonFile << "      \"status\": \"" << escapeJsonString(userStatus.at(user.id)) << "\"\n";
        jsonFile << "    }";
        if (i < users.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    jsonFile << "  ],\n";

    jsonFile << "  \"daily_logs\": [\n";
    auto sortedLogs = getSortedLogs();
    for (size_t i = 0; i < sortedLogs.size(); ++i) {
        const auto& log = sortedLogs[i];
        jsonFile << "    {\n";
        jsonFile << "      \"user_id\": \"" << escapeJsonString(log.userId) << "\",\n";
        jsonFile << "      \"user_name\": \"" << escapeJsonString(log.userName) << "\",\n";
        jsonFile << "      \"action\": \"" << escapeJsonString(log.action) << "\",\n";
        jsonFile << "      \"timestamp\": \"" << log.getFormattedTime() << "\",\n";
        jsonFile << "      \"unix_timestamp\": " << log.timestamp << "\n";
        jsonFile << "    }";
        if (i < sortedLogs.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    jsonFile << "  ],\n";

    jsonFile << "  \"summary\": {\n";
    jsonFile << "    \"total_users\": " << users.size() << ",\n";
    jsonFile << "    \"total_scans\": " << dailyLogs.size() << ",\n";
    jsonFile << "    \"export_time\": \"" << getCurrentTimeString() << "\"\n";
    jsonFile << "  }\n";
    jsonFile << "}\n";

    jsonFile.close();
    cout << "JSON data exported: " << users.size() << " users, " << dailyLogs.size() << " logs" << endl;
    return true;
}

string getCurrentTimeString() {
    time_t now = time(nullptr);
    stringstream ss;
    ss << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

string escapeJsonString(const string& input) {
    string escaped;
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

void RFIDSystem::displayAllLogs() {
    auto sortedLogs = getSortedLogs();

    cout << "\n=== ALL SCAN LOGS (Sorted by Time) ===\n";
    cout << left << setw(12) << "User ID"
              << setw(20) << "Name"
              << setw(8) << "Action"
              << "Timestamp\n";
    cout << string(60, '-') << "\n";

    for (const auto& log : sortedLogs) {
        cout << left << setw(12) << log.userId
                  << setw(20) << log.userName
                  << setw(8) << log.action
                  << log.getFormattedTime() << "\n";
    }
    cout << "\nTotal scans: " << sortedLogs.size() << "\n";
}

void RFIDSystem::displayAllUsers() {
    cout << "\n=== ALL REGISTERED USERS ===\n";
    cout << left << setw(12) << "User ID"
              << setw(20) << "Name"
              << setw(10) << "Role"
              << "Registration\n";
    cout << string(50, '-') << "\n";

    for (const auto& user : users) {
        cout << left << setw(12) << user.id
                  << setw(20) << user.name
                  << setw(10) << user.role
                  << "Active\n";
    }
    cout << "\nTotal users: " << users.size() << "\n";
}

void RFIDSystem::displayUserStatus() {
    cout << "\n=== CURRENT USER STATUS ===\n";
    cout << left << setw(12) << "User ID"
              << setw(20) << "Name"
              << setw(10) << "Role"
              << "Status\n";
    cout << string(50, '-') << "\n";

    for (const auto& user : users) {
        cout << left << setw(12) << user.id
                  << setw(20) << user.name
                  << setw(10) << user.role
                  << userStatus[user.id] << "\n";
    }
}

void RFIDSystem::displayDailyReport() {
    cout << "\n=== DAILY ATTENDANCE REPORT ===\n";

    map<string, int> userScanCount;
    map<string, string> lastAction;

    for (const auto& log : dailyLogs) {
        userScanCount[log.userId]++;
        lastAction[log.userId] = log.action;
    }

    cout << left << setw(12) << "User ID"
              << setw(20) << "Name"
              << setw(12) << "Total Scans"
              << "Last Action\n";
    cout << string(60, '-') << "\n";

    for (const auto& user : users) {
        int scans = userScanCount[user.id];
        string action = lastAction[user.id].empty() ? "NONE" : lastAction[user.id];

        cout << left << setw(12) << user.id
                  << setw(20) << user.name
                  << setw(12) << scans
                  << action << "\n";
    }
}

void RFIDSystem::clearDailyLogs() {
    dailyLogs.clear();
    for (auto& status : userStatus) {
        status.second = "OUT";
    }
    saveSystemData();
    cout << "Daily logs cleared and all users set to OUT status.\n";
}

void RFIDSystem::clearAllData() {
    users.clear();
    dailyLogs.clear();
    userStatus.clear();
    saveSystemData();
    cout << "All system data cleared (users and logs).\n";
}
