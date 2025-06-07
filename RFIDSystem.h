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

    // save methods
    bool saveSystemData();
    bool loadSystemData();
    bool saveAllData();
    bool exportToJSON();

    // display methods
    void displayAllLogs();
    void displayUserStatus();
    void displayDailyReport();
    void displayAllUsers();

    // maintenance methods
    void clearDailyLogs();
    void clearAllData();
    int getTotalScans() const { return dailyLogs.size(); }
    int getTotalUsers() const { return users.size(); }
};

// Utility functions
std::string getCurrentTimeString();
std::string escapeJsonString(const std::string& input);

#endif
