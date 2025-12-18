#include "database.h"
#include <QProcessEnvironment>

bool Database::connect() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    
     
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString dbHost = env.value("DB_HOST", "db");
    QString dbPort = env.value("DB_PORT", "5432");
    QString dbName = env.value("DB_NAME", "autosalon");
    QString dbUser = env.value("DB_USER", "user");
    QString dbPassword = env.value("DB_PASSWORD", "pass");
    
    db.setHostName(dbHost);
    db.setPort(dbPort.toInt());
    db.setDatabaseName(dbName);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);

    if (!db.open()) {
        qDebug() << "Ошибка подключения к БД:" << db.lastError().text();
        return false;
    }
    
    qDebug() << "Успешное подключение к БД:" << dbHost << ":" << dbPort;
    return true;
}

bool Database::initTable() {
    QSqlQuery query;
    
     
    QString createTableQuery =
        "CREATE TABLE IF NOT EXISTS cars ("
        "id SERIAL PRIMARY KEY, "
        "brand VARCHAR(100) NOT NULL, "
        "manufacturer VARCHAR(100) NOT NULL, "
        "release_date DATE NOT NULL, "
        "mileage INTEGER NOT NULL CHECK (mileage >= 0), "
        "price INTEGER NOT NULL CHECK (price > 0)"
        ");";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Таблица cars успешно создана/проверена";
    
     
     
    query.exec("CREATE INDEX IF NOT EXISTS idx_cars_brand ON cars(brand);");
    
     
    query.exec("CREATE INDEX IF NOT EXISTS idx_cars_manufacturer ON cars(manufacturer);");
    
     
    query.exec("CREATE INDEX IF NOT EXISTS idx_cars_price ON cars(price);");
    
    qDebug() << "Индексы успешно созданы/проверены";
    
    return true;
}

QSqlDatabase Database::getDatabase() {
    return QSqlDatabase::database();
}
