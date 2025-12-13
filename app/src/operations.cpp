#include "operations.h"

// Валидация входных данных
// Requirements: 6.2, 6.3, 8.2, 12.1
bool Operations::validateCarData(const QString& brand,
                                const QString& manufacturer,
                                const QDate& releaseDate,
                                int mileage,
                                int price,
                                QString& errorMessage) {
    // Проверка пустых полей (Requirement 6.2)
    if (brand.trimmed().isEmpty()) {
        errorMessage = "Марка автомобиля не может быть пустой.";
        return false;
    }
    
    if (manufacturer.trimmed().isEmpty()) {
        errorMessage = "Производитель не может быть пустым.";
        return false;
    }
    
    // Проверка корректности даты (Requirement 6.3)
    if (!releaseDate.isValid()) {
        errorMessage = "Дата выпуска должна быть в корректном формате.";
        return false;
    }
    
    // Проверка бизнес-правил: неотрицательный пробег (Requirement 6.3)
    if (mileage < 0) {
        errorMessage = "Пробег не может быть отрицательным.";
        return false;
    }
    
    // Проверка бизнес-правил: положительная цена (Requirements 6.3, 8.2)
    if (price <= 0) {
        errorMessage = "Цена должна быть положительным числом.";
        return false;
    }
    
    return true;
}

// Добавление новой записи об автомобиле
// Requirements: 6.1, 6.5, 10.5, 12.3
bool Operations::addCar(const QString& brand,
                       const QString& manufacturer,
                       const QDate& releaseDate,
                       int mileage,
                       int price) {
    // Валидация входных данных
    QString errorMessage;
    if (!validateCarData(brand, manufacturer, releaseDate, mileage, price, errorMessage)) {
        qWarning() << "Ошибка валидации:" << errorMessage;
        return false;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
    // Использование транзакции для обеспечения целостности данных (Requirement 12.3)
    db.transaction();
    
    // Использование prepared statements для защиты от SQL injection (Requirement 10.5)
    QSqlQuery query(db);
    query.prepare("INSERT INTO cars (brand, manufacturer, release_date, mileage, price) "
                  "VALUES (:brand, :manufacturer, :release_date, :mileage, :price)");
    query.bindValue(":brand", brand.trimmed());
    query.bindValue(":manufacturer", manufacturer.trimmed());
    query.bindValue(":release_date", releaseDate);
    query.bindValue(":mileage", mileage);
    query.bindValue(":price", price);
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления записи:" << query.lastError().text();
        db.rollback();
        return false;
    }
    
    db.commit();
    qDebug() << "Запись успешно добавлена:" << brand << manufacturer;
    return true;
}

// Удаление всех автомобилей указанного производителя
// Requirements: 7.1, 7.2, 10.5, 12.3
int Operations::deleteByManufacturer(const QString& manufacturer) {
    if (manufacturer.trimmed().isEmpty()) {
        qWarning() << "Производитель не может быть пустым";
        return 0;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
    // Использование транзакции для обеспечения целостности данных (Requirement 12.3)
    db.transaction();
    
    // Использование prepared statements для защиты от SQL injection (Requirement 10.5)
    QSqlQuery query(db);
    query.prepare("DELETE FROM cars WHERE manufacturer = :manufacturer");
    query.bindValue(":manufacturer", manufacturer.trimmed());
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления записей:" << query.lastError().text();
        db.rollback();
        return 0;
    }
    
    int deletedCount = query.numRowsAffected();
    db.commit();
    
    qDebug() << "Удалено записей:" << deletedCount << "для производителя:" << manufacturer;
    return deletedCount;
}

// Обновление цены для всех автомобилей указанной марки
// Requirements: 8.1, 8.2, 8.3, 10.5, 12.3
int Operations::updatePriceByBrand(const QString& brand, int newPrice) {
    if (brand.trimmed().isEmpty()) {
        qWarning() << "Марка не может быть пустой";
        return 0;
    }
    
    // Проверка положительной цены (Requirement 8.2)
    if (newPrice <= 0) {
        qWarning() << "Цена должна быть положительным числом";
        return 0;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
    // Использование транзакции для обеспечения целостности данных (Requirement 12.3)
    db.transaction();
    
    // Использование prepared statements для защиты от SQL injection (Requirement 10.5)
    QSqlQuery query(db);
    query.prepare("UPDATE cars SET price = :price WHERE brand = :brand");
    query.bindValue(":price", newPrice);
    query.bindValue(":brand", brand.trimmed());
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления цены:" << query.lastError().text();
        db.rollback();
        return 0;
    }
    
    int updatedCount = query.numRowsAffected();
    db.commit();
    
    qDebug() << "Обновлено записей:" << updatedCount << "для марки:" << brand;
    return updatedCount;
}

// Экспорт данных в текстовый файл
// Requirements: 4.1, 4.2, 4.3
bool Operations::exportToFile(const QString& filename) {
    if (filename.trimmed().isEmpty()) {
        qWarning() << "Имя файла не может быть пустым";
        return false;
    }
    
    // Открытие файла для записи
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл для записи:" << filename;
        return false;
    }
    
    QTextStream out(&file);
    
    // Получение данных из базы данных
    QSqlDatabase db = Database::getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT brand, mileage, price FROM cars ORDER BY brand");
    
    if (!query.exec()) {
        qWarning() << "Ошибка выполнения запроса:" << query.lastError().text();
        file.close();
        return false;
    }
    
    int recordCount = 0;
    
    // Запись данных в файл в формате: "Brand: <brand>, Mileage: <mileage>, Price: <price>"
    // Requirement 4.2
    while (query.next()) {
        QString brand = query.value(0).toString();
        int mileage = query.value(1).toInt();
        int price = query.value(2).toInt();
        
        out << "Brand: " << brand << ", Mileage: " << mileage << ", Price: " << price << "\n";
        recordCount++;
    }
    
    file.close();
    qDebug() << "Экспортировано записей:" << recordCount << "в файл:" << filename;
    return true;
}

// Чтение содержимого файла
// Requirements: 5.1, 5.2, 5.3, 5.4
QString Operations::readFileContent(const QString& filename) {
    if (filename.trimmed().isEmpty()) {
        qWarning() << "Имя файла не может быть пустым";
        return QString();
    }
    
    // Проверка существования файла (Requirement 5.2)
    QFile file(filename);
    if (!file.exists()) {
        qWarning() << "Файл не найден:" << filename;
        return QString();
    }
    
    // Открытие файла для чтения (Requirement 5.3)
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл для чтения:" << filename;
        return QString();
    }
    
    QTextStream in(&file);
    
    // Чтение всего содержимого файла с сохранением форматирования (Requirement 5.4)
    QString content = in.readAll();
    
    file.close();
    qDebug() << "Прочитан файл:" << filename << "размер:" << content.length() << "символов";
    
    return content;
}
