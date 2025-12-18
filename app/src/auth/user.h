#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

enum class UserRole {
    Guest = 0,       
    Seller = 1,      
    Manager = 2,     
    Admin = 3        
};

struct User {
    int id;
    QString username;
    QString fullName;
    UserRole role;
    QDateTime createdAt;
    
    User() : id(-1), role(UserRole::Guest) {}
    
    User(int id, const QString& username, const QString& fullName, UserRole role, const QDateTime& createdAt = QDateTime::currentDateTime())
        : id(id), username(username), fullName(fullName), role(role), createdAt(createdAt) {}
    
    bool isValid() const { return id >= 0; }
    
    QString getRoleString() const {
        switch (role) {
            case UserRole::Guest: return "Гость";
            case UserRole::Seller: return "Продавец";
            case UserRole::Manager: return "Менеджер";
            case UserRole::Admin: return "Администратор";
            default: return "Неизвестно";
        }
    }
    
    static UserRole roleFromString(const QString& roleStr) {
        if (roleStr == "admin") return UserRole::Admin;
        if (roleStr == "manager") return UserRole::Manager;
        if (roleStr == "seller") return UserRole::Seller;
        return UserRole::Guest;
    }
    
    static QString roleToString(UserRole role) {
        switch (role) {
            case UserRole::Admin: return "admin";
            case UserRole::Manager: return "manager";
            case UserRole::Seller: return "seller";
            case UserRole::Guest: return "guest";
            default: return "guest";
        }
    }
};

#endif  