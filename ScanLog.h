#ifndef SCANLOG_H
#define SCANLOG_H

#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

struct ScanLog {
    std::string userId;
    std::string userName;
    std::string action; // IN or OUT
    std::time_t timestamp;

    ScanLog() = default;
    ScanLog(const std::string& uid, const std::string& uname, const std::string& act)
        : userId(uid), userName(uname), action(act) {
        timestamp = std::time(nullptr);
    }

    std::string getFormattedTime() const {
        std::stringstream ss;
        ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    bool operator<(const ScanLog& other) const {
        return timestamp < other.timestamp;
    }
};

#endif
