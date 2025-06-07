#ifndef USER_H
#define USER_H

#include <string>

struct User {
    std::string id;
    std::string name;
    std::string role; // student, staff, faculty

    User() = default;
    User(const std::string& userId, const std::string& userName, const std::string& userRole)
        : id(userId), name(userName), role(userRole) {}
};

#endif
