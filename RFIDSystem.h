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

    bool saveToBinaryFile(const std::string& filename = "data/daily_logs.bin");
    bool loadFromBinaryFile(const std::string& filename = "data/daily_logs.bin");
    bool exportToJSON(const std::string& filename = "data/daily_logs.json");

    void displayAllLogs();
    void displayUserStatus();
    void displayDailyReport();

    void clearDailyLogs();
    int getTotalScans() const { return dailyLogs.size(); }
};

#endif
