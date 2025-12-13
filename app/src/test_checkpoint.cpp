#include <QCoreApplication>
#include <QDebug>
#include <QDate>
#include <QSqlQuery>
#include "database.h"
#include "operations.h"

// Вспомогательная функция для вывода результатов теста
void printTestResult(const QString& testName, bool passed) {
    if (passed) {
        qDebug() << "✓" << testName << "- ПРОЙДЕН";
    } else {
        qDebug() << "✗" << testName << "- ПРОВАЛЕН";
    }
}

// Вспомогательная функция для подсчета записей в таблице
int countRecords() {
    QSqlQuery query("SELECT COUNT(*) FROM cars");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

// Вспомогательная функция для очистки таблицы
void clearTable() {
    QSqlQuery query("DELETE FROM cars");
    query.exec();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "=== CHECKPOINT: Проверка базовой функциональности ===\n";
    
    // Тест 1: Подключение к БД
    qDebug() << "Тест 1: Подключение к базе данных";
    bool dbConnected = Database::connect();
    printTestResult("Подключение к БД", dbConnected);
    
    if (!dbConnected) {
        qDebug() << "\nОШИБКА: Не удалось подключиться к БД. Дальнейшие тесты невозможны.";
        return 1;
    }
    
    // Тест 2: Инициализация таблицы
    qDebug() << "\nТест 2: Инициализация таблицы";
    bool tableInitialized = Database::initTable();
    printTestResult("Инициализация таблицы", tableInitialized);
    
    if (!tableInitialized) {
        qDebug() << "\nОШИБКА: Не удалось инициализировать таблицу.";
        return 1;
    }
    
    // Очистка таблицы перед тестами
    clearTable();
    
    // Тест 3: Добавление записи
    qDebug() << "\nТест 3: Добавление записи";
    int countBefore = countRecords();
    bool addResult = Operations::addCar("Camry", "Toyota", QDate(2020, 5, 15), 50000, 1500000);
    int countAfter = countRecords();
    bool addSuccess = addResult && (countAfter == countBefore + 1);
    printTestResult("Добавление записи", addSuccess);
    qDebug() << "  Записей до:" << countBefore << ", после:" << countAfter;
    
    // Добавим еще несколько записей для тестирования
    Operations::addCar("Model 3", "Tesla", QDate(2021, 3, 10), 20000, 3500000);
    Operations::addCar("X5", "BMW", QDate(2019, 8, 20), 75000, 2800000);
    Operations::addCar("Corolla", "Toyota", QDate(2018, 11, 5), 100000, 900000);
    
    qDebug() << "  Добавлено тестовых записей. Всего в БД:" << countRecords();
    
    // Тест 4: Обновление цены по марке
    qDebug() << "\nТест 4: Обновление цены по марке";
    int updatedCount = Operations::updatePriceByBrand("Camry", 1600000);
    bool updateSuccess = (updatedCount > 0);
    printTestResult("Обновление цены", updateSuccess);
    qDebug() << "  Обновлено записей:" << updatedCount;
    
    // Проверим, что цена действительно изменилась
    QSqlQuery checkUpdate("SELECT price FROM cars WHERE brand = 'Camry'");
    if (checkUpdate.next()) {
        int newPrice = checkUpdate.value(0).toInt();
        qDebug() << "  Новая цена Camry:" << newPrice;
        if (newPrice != 1600000) {
            qDebug() << "  ВНИМАНИЕ: Цена не соответствует ожидаемой!";
        }
    }
    
    // Тест 5: Удаление по производителю
    qDebug() << "\nТест 5: Удаление по производителю";
    int countBeforeDelete = countRecords();
    int deletedCount = Operations::deleteByManufacturer("Toyota");
    int countAfterDelete = countRecords();
    bool deleteSuccess = (deletedCount > 0) && (countAfterDelete == countBeforeDelete - deletedCount);
    printTestResult("Удаление по производителю", deleteSuccess);
    qDebug() << "  Удалено записей:" << deletedCount;
    qDebug() << "  Записей до:" << countBeforeDelete << ", после:" << countAfterDelete;
    
    // Тест 6: Экспорт в файл
    qDebug() << "\nТест 6: Экспорт данных в файл";
    QString exportFilename = "/tmp/test_export.txt";
    bool exportSuccess = Operations::exportToFile(exportFilename);
    printTestResult("Экспорт в файл", exportSuccess);
    
    // Тест 7: Чтение файла
    qDebug() << "\nТест 7: Чтение содержимого файла";
    QString fileContent = Operations::readFileContent(exportFilename);
    bool readSuccess = !fileContent.isEmpty();
    printTestResult("Чтение файла", readSuccess);
    
    if (readSuccess) {
        qDebug() << "  Содержимое файла:";
        qDebug() << fileContent;
    }
    
    // Тест 8: Валидация данных
    qDebug() << "\nТест 8: Валидация входных данных";
    QString errorMsg;
    
    // Тест 8.1: Пустая марка
    bool validEmpty = Operations::validateCarData("", "Toyota", QDate(2020, 1, 1), 50000, 1500000, errorMsg);
    bool test8_1 = !validEmpty; // Должна быть ошибка
    printTestResult("Валидация: пустая марка отклонена", test8_1);
    if (!validEmpty) qDebug() << "  Сообщение об ошибке:" << errorMsg;
    
    // Тест 8.2: Отрицательный пробег
    bool validNegMileage = Operations::validateCarData("Camry", "Toyota", QDate(2020, 1, 1), -1000, 1500000, errorMsg);
    bool test8_2 = !validNegMileage; // Должна быть ошибка
    printTestResult("Валидация: отрицательный пробег отклонен", test8_2);
    if (!validNegMileage) qDebug() << "  Сообщение об ошибке:" << errorMsg;
    
    // Тест 8.3: Неположительная цена
    bool validZeroPrice = Operations::validateCarData("Camry", "Toyota", QDate(2020, 1, 1), 50000, 0, errorMsg);
    bool test8_3 = !validZeroPrice; // Должна быть ошибка
    printTestResult("Валидация: нулевая цена отклонена", test8_3);
    if (!validZeroPrice) qDebug() << "  Сообщение об ошибке:" << errorMsg;
    
    // Тест 8.4: Валидные данные
    bool validData = Operations::validateCarData("Camry", "Toyota", QDate(2020, 1, 1), 50000, 1500000, errorMsg);
    printTestResult("Валидация: корректные данные приняты", validData);
    
    // Итоговая статистика
    qDebug() << "\n=== ИТОГИ ПРОВЕРКИ ===";
    qDebug() << "Всех тестов выполнено: 11";
    
    int passedTests = 0;
    if (dbConnected) passedTests++;
    if (tableInitialized) passedTests++;
    if (addSuccess) passedTests++;
    if (updateSuccess) passedTests++;
    if (deleteSuccess) passedTests++;
    if (exportSuccess) passedTests++;
    if (readSuccess) passedTests++;
    if (test8_1) passedTests++;
    if (test8_2) passedTests++;
    if (test8_3) passedTests++;
    if (validData) passedTests++;
    
    qDebug() << "Пройдено тестов:" << passedTests << "из 11";
    
    if (passedTests == 11) {
        qDebug() << "\n✓✓✓ ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО! ✓✓✓";
        qDebug() << "Базовая функциональность работает корректно.";
        return 0;
    } else {
        qDebug() << "\n✗✗✗ НЕКОТОРЫЕ ТЕСТЫ НЕ ПРОЙДЕНЫ ✗✗✗";
        qDebug() << "Требуется дополнительная отладка.";
        return 1;
    }
}
