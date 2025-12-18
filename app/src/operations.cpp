#include "operations.h"

 
 
bool Operations::validateCarData(const QString& brand,
                                const QString& manufacturer,
                                const QDate& releaseDate,
                                int mileage,
                                int price,
                                QString& errorMessage) {
     
    if (brand.trimmed().isEmpty()) {
        errorMessage = "Марка автомобиля не может быть пустой.";
        return false;
    }
    
    if (manufacturer.trimmed().isEmpty()) {
        errorMessage = "Производитель не может быть пустым.";
        return false;
    }
    
     
    if (!releaseDate.isValid()) {
        errorMessage = "Дата выпуска должна быть в корректном формате.";
        return false;
    }
    
     
    if (mileage < 0) {
        errorMessage = "Пробег не может быть отрицательным.";
        return false;
    }
    
     
    if (price <= 0) {
        errorMessage = "Цена должна быть положительным числом.";
        return false;
    }
    
    return true;
}

 
 
bool Operations::addCar(const QString& brand,
                       const QString& manufacturer,
                       const QDate& releaseDate,
                       int mileage,
                       int price) {
     
    QString errorMessage;
    if (!validateCarData(brand, manufacturer, releaseDate, mileage, price, errorMessage)) {
        qWarning() << "Ошибка валидации:" << errorMessage;
        return false;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
     
    db.transaction();
    
     
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

 
 
int Operations::deleteByManufacturer(const QString& manufacturer) {
    if (manufacturer.trimmed().isEmpty()) {
        qWarning() << "Производитель не может быть пустым";
        return 0;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
     
    db.transaction();
    
     
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

 
 
int Operations::updatePriceByBrand(const QString& brand, int newPrice) {
    if (brand.trimmed().isEmpty()) {
        qWarning() << "Марка не может быть пустой";
        return 0;
    }
    
     
    if (newPrice <= 0) {
        qWarning() << "Цена должна быть положительным числом";
        return 0;
    }
    
    QSqlDatabase db = Database::getDatabase();
    
     
    db.transaction();
    
     
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

 
 
bool Operations::exportToFile(const QString& filename) {
    if (filename.trimmed().isEmpty()) {
        qWarning() << "Имя файла не может быть пустым";
        return false;
    }
    
     
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл для записи:" << filename;
        return false;
    }
    
    QTextStream out(&file);
    
     
    QSqlDatabase db = Database::getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT brand, mileage, price FROM cars ORDER BY brand");
    
    if (!query.exec()) {
        qWarning() << "Ошибка выполнения запроса:" << query.lastError().text();
        file.close();
        return false;
    }
    
    int recordCount = 0;
    
     
     
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

 
 
QString Operations::readFileContent(const QString& filename) {
    if (filename.trimmed().isEmpty()) {
        qWarning() << "Имя файла не может быть пустым";
        return QString();
    }
    
     
    QFile file(filename);
    if (!file.exists()) {
        qWarning() << "Файл не найден:" << filename;
        return QString();
    }
    
     
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл для чтения:" << filename;
        return QString();
    }
    
    QTextStream in(&file);
    
     
    QString content = in.readAll();
    
    file.close();
    qDebug() << "Прочитан файл:" << filename << "размер:" << content.length() << "символов";
    
    return content;
}
