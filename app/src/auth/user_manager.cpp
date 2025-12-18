#include "user_manager.h"

bool UserManager::initializeUserTable() {
    QSqlDatabase db = Database::getDatabase();
    
    QSqlQuery query(db);
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            username VARCHAR(50) UNIQUE NOT NULL,
            password_hash VARCHAR(255) NOT NULL,
            full_name VARCHAR(100) NOT NULL,
            role VARCHAR(20) NOT NULL DEFAULT 'guest',
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createTableSQL)) {
        qWarning() << "Ошибка создания таблицы пользователей:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Таблица пользователей успешно создана или уже существует";
    return true;
}

bool UserManager::createDefaultUsers() {
     
    if (!userExists("admin")) {
        createUser("admin", "admin123", "Администратор системы", UserRole::Admin);
    }
    
    if (!userExists("manager")) {
        createUser("manager", "manager123", "Менеджер автосалона", UserRole::Manager);
    }
    
    if (!userExists("seller")) {
        createUser("seller", "seller123", "Продавец", UserRole::Seller);
    }
    
    if (!userExists("guest")) {
        createUser("guest", "guest123", "Гостевой пользователь", UserRole::Guest);
    }
    
    qDebug() << "Пользователи по умолчанию созданы";
    return true;
}

User UserManager::authenticateUser(const QString& username, const QString& password) {
    QSqlDatabase db = Database::getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT id, username, password_hash, full_name, role, created_at FROM users WHERE username = :username");
    query.bindValue(":username", username.trimmed());
    
    if (!query.exec()) {
        qWarning() << "Ошибка поиска пользователя:" << query.lastError().text();
        return User();  
    }
    
    if (query.next()) {
        QString storedHash = query.value("password_hash").toString();
        
        if (verifyPassword(password, storedHash)) {
             
            User user;
            user.id = query.value("id").toInt();
            user.username = query.value("username").toString();
            user.fullName = query.value("full_name").toString();
            user.role = User::roleFromString(query.value("role").toString());
            user.createdAt = query.value("created_at").toDateTime();
            
            qDebug() << "Пользователь успешно аутентифицирован:" << user.username;
            return user;
        } else {
            qWarning() << "Неверный пароль для пользователя:" << username;
        }
    } else {
        qWarning() << "Пользователь не найден:" << username;
    }
    
    return User();  
}

bool UserManager::createUser(const QString& username, const QString& password, 
                            const QString& fullName, UserRole role) {
    if (userExists(username)) {
        qWarning() << "Пользователь уже существует:" << username;
        return false;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
    db.transaction();
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password_hash, full_name, role) "
                  "VALUES (:username, :password_hash, :full_name, :role)");
    query.bindValue(":username", username.trimmed());
    query.bindValue(":password_hash", hashPassword(password));
    query.bindValue(":full_name", fullName.trimmed());
    query.bindValue(":role", User::roleToString(role));
    
    if (!query.exec()) {
        qWarning() << "Ошибка создания пользователя:" << query.lastError().text();
        db.rollback();
        return false;
    }
    
    db.commit();
    qDebug() << "Пользователь успешно создан:" << username;
    return true;
}

User UserManager::getUserById(int userId) {
    QSqlDatabase db = Database::getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT id, username, full_name, role, created_at FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec()) {
        qWarning() << "Ошибка поиска пользователя по ID:" << query.lastError().text();
        return User();
    }
    
    if (query.next()) {
        User user;
        user.id = query.value("id").toInt();
        user.username = query.value("username").toString();
        user.fullName = query.value("full_name").toString();
        user.role = User::roleFromString(query.value("role").toString());
        user.createdAt = query.value("created_at").toDateTime();
        return user;
    }
    
    return User();
}

QList<User> UserManager::getAllUsers() {
    QList<User> users;
    QSqlDatabase db = Database::getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT id, username, full_name, role, created_at FROM users ORDER BY username");
    
    if (!query.exec()) {
        qWarning() << "Ошибка получения списка пользователей:" << query.lastError().text();
        return users;
    }
    
    while (query.next()) {
        User user;
        user.id = query.value("id").toInt();
        user.username = query.value("username").toString();
        user.fullName = query.value("full_name").toString();
        user.role = User::roleFromString(query.value("role").toString());
        user.createdAt = query.value("created_at").toDateTime();
        users.append(user);
    }
    
    return users;
}

bool UserManager::changePassword(int userId, const QString& newPassword) {
    QSqlDatabase db = Database::getDatabase();
    
    db.transaction();
    
    QSqlQuery query(db);
    query.prepare("UPDATE users SET password_hash = :password_hash WHERE id = :id");
    query.bindValue(":password_hash", hashPassword(newPassword));
    query.bindValue(":id", userId);
    
    if (!query.exec()) {
        qWarning() << "Ошибка изменения пароля:" << query.lastError().text();
        db.rollback();
        return false;
    }
    
    db.commit();
    qDebug() << "Пароль успешно изменен для пользователя ID:" << userId;
    return true;
}

bool UserManager::deleteUser(int userId) {
    QSqlDatabase db = Database::getDatabase();
    
    db.transaction();
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления пользователя:" << query.lastError().text();
        db.rollback();
        return false;
    }
    
    db.commit();
    qDebug() << "Пользователь успешно удален ID:" << userId;
    return true;
}

bool UserManager::userExists(const QString& username) {
    QSqlDatabase db = Database::getDatabase();
    
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username.trimmed());
    
    if (!query.exec()) {
        qWarning() << "Ошибка проверки существования пользователя:" << query.lastError().text();
        return false;
    }
    
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

QString UserManager::hashPassword(const QString& password) {
     
    QString saltedPassword = "autosalon_salt_" + password + "_2024";
    QByteArray hash = QCryptographicHash::hash(saltedPassword.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

bool UserManager::verifyPassword(const QString& password, const QString& hash) {
    return hashPassword(password) == hash;
}