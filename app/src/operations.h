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
    static bool addCar(const QString& brand, 
                      const QString& manufacturer, 
                      const QDate& releaseDate, 
                      int mileage, 
                      int price);
    
    // Удаление всех автомобилей указанного производителя
    // Возвращает количество удаленных записей
    static int deleteByManufacturer(const QString& manufacturer);
    
    // Обновление цены для всех автомобилей указанной марки
    // Возвращает количество обновленных записей
    static int updatePriceByBrand(const QString& brand, int newPrice);
    
    // Экспорт данных (марка, пробег, цена) в текстовый файл
    // Формат: "Brand: <brand>, Mileage: <mileage>, Price: <price>"
    static bool exportToFile(const QString& filename);
    
    // Чтение содержимого файла
    static QString readFileContent(const QString& filename);
    
    // Валидация входных данных
    static bool validateCarData(const QString& brand,
                               const QString& manufacturer,
                               const QDate& releaseDate,
                               int mileage,
                               int price,
                               QString& errorMessage);
};

#endif 
