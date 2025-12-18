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
    static bool addCar(const QString& brand, 
                      const QString& manufacturer, 
                      const QDate& releaseDate, 
                      int mileage, 
                      int price);
    
     
     
     
    static int deleteByManufacturer(const QString& manufacturer);
    
     
     
     
    static int updatePriceByBrand(const QString& brand, int newPrice);
    
     
     
     
    static bool exportToFile(const QString& filename);
    
     
     
    static QString readFileContent(const QString& filename);
    
     
     
    static bool validateCarData(const QString& brand,
                               const QString& manufacturer,
                               const QDate& releaseDate,
                               int mileage,
                               int price,
                               QString& errorMessage);
};

#endif  
