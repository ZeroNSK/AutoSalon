#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "database.h"

class Operations {
public:
    // Добавление новой записи об автомобиле
    // Возвращает true при успехе, false при ошибке
    // Requirements: 6.1, 6.5
    static bool addCar(const QString& brand, 
                      const QString& manufacturer, 
                      const QDate& releaseDate, 
                      int mileage, 
                      int price);
    
    // Удаление всех автомобилей указанного производителя
    // Возвращает количество удаленных записей
    // Requirements: 7.1, 7.2
    static int deleteByManufacturer(const QString& manufacturer);
    
    // Обновление цены для всех автомобилей указанной марки
    // Возвращает количество обновленных записей
    // Requirements: 8.1, 8.3
    static int updatePriceByBrand(const QString& brand, int newPrice);
    
    // Экспорт данных (марка, пробег, цена) в текстовый файл
    // Формат: "Brand: <brand>, Mileage: <mileage>, Price: <price>"
    // Requirements: 4.1, 4.3
    static bool exportToFile(const QString& filename);
    
    // Чтение содержимого файла
    // Requirements: 5.1, 5.2
    static QString readFileContent(const QString& filename);
    
    // Валидация входных данных
    // Requirements: 6.2, 6.3, 8.2, 12.1
    static bool validateCarData(const QString& brand,
                               const QString& manufacturer,
                               const QDate& releaseDate,
                               int mileage,
                               int price,
                               QString& errorMessage);
};

#endif // OPERATIONS_H
