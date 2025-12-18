#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class Database {
public:
    static bool connect();
    static bool initTable();
    static QSqlDatabase getDatabase();
};

#endif  
