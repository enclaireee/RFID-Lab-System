#ifndef RFIDSYSTEM_H
#define RFIDSYSTEM_H

#include "User.h"
#include "ScanLog.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>

class RFIDSystem {
private:
    std::vector<User> users;
    std::vector<ScanLog> dailyLogs;
    std::map<std::string, std::string> userStatus;
    void createDataDirectory();

public:
    RFIDSystem();

    void addUser(const std::string& id, const std::string& name, const std::string& role);
    User* findUser(const std::string& id);

    bool scanRFID(const std::string& userId);

    std::vector<ScanLog> searchLogsByUserId(const std::string& userId);
    std::vector<ScanLog> getSortedLogs();

    bool saveAllData(const std::string& filename = "data/system_data.bin");
    bool loadAllData(const std::string& filename = "data/system_data.bin");
    bool exportToJSON(const std::string& filename = "data/system_export.json");
    bool importFromJSON(const std::string& filename = "data/system_export.json");

    bool saveToBinaryFile(const std::string& filename = "data/daily_logs.bin");
    bool loadFromBinaryFile(const std::string& filename = "data/daily_logs.bin");

    void displayAllLogs();
    void displayUserStatus();
    void displayDailyReport();
    void displayAllUsers();

    void clearDailyLogs();
    void clearAllData();
    int getTotalScans() const { return dailyLogs.size(); }
    int getTotalUsers() const { return users.size(); }
};

#endif
