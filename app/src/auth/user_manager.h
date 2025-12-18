#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "user.h"
#include "../database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>

class UserManager {
public:
     
    static bool initializeUserTable();
    
     
    static bool createDefaultUsers();
    
     
    static User authenticateUser(const QString& username, const QString& password);
    
    static bool registerUser(const QString& username, const QString& password, 
                           const QString& fullName, UserRole role);
    
    static bool createUser(const QString& username, const QString& password, 
                          const QString& fullName, UserRole role);
    
     
    static User getUserById(int userId);
    
     
    static QList<User> getAllUsers();
    
     
    static bool changePassword(int userId, const QString& newPassword);
    
     
    static bool deleteUser(int userId);
    
     
    static bool userExists(const QString& username);

private:
     
    static QString hashPassword(const QString& password);
    
     
    static bool verifyPassword(const QString& password, const QString& hash);
};

#endif  