#!/bin/bash

# Финальный тест приложения Автосалон
# Этот скрипт проверяет все основные функции системы

echo "========================================="
echo "Финальное тестирование системы Автосалон"
echo "========================================="
echo ""

# Цвета для вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Счетчики
PASSED=0
FAILED=0

# Функция для проверки
check_test() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ PASSED${NC}: $2"
        ((PASSED++))
    else
        echo -e "${RED}✗ FAILED${NC}: $2"
        ((FAILED++))
    fi
}

echo "1. Проверка Docker контейнеров..."
echo "-----------------------------------"

# Запуск контейнеров
echo "Запуск контейнеров..."
docker-compose up -d
sleep 5

# Проверка, что контейнеры запущены
DB_RUNNING=$(docker ps | grep autosalon_db | wc -l)
APP_RUNNING=$(docker ps | grep autosalon_app | wc -l)

check_test $((DB_RUNNING > 0 ? 0 : 1)) "Контейнер БД запущен"
check_test $((APP_RUNNING > 0 ? 0 : 1)) "Контейнер приложения запущен"

echo ""
echo "2. Проверка подключения к базе данных..."
echo "-----------------------------------"

# Проверка подключения к PostgreSQL
docker exec autosalon_db psql -U user -d autosalon -c "SELECT 1;" > /dev/null 2>&1
check_test $? "Подключение к PostgreSQL"

# Проверка существования таблицы
docker exec autosalon_db psql -U user -d autosalon -c "\dt" | grep cars > /dev/null 2>&1
check_test $? "Таблица cars существует"

# Проверка структуры таблицы
docker exec autosalon_db psql -U user -d autosalon -c "\d cars" | grep -E "(brand|manufacturer|release_date|mileage|price)" > /dev/null 2>&1
check_test $? "Структура таблицы корректна"

# Проверка индексов
INDEXES=$(docker exec autosalon_db psql -U user -d autosalon -c "\di" | grep -E "(idx_cars_brand|idx_cars_manufacturer|idx_cars_price)" | wc -l)
check_test $((INDEXES >= 3 ? 0 : 1)) "Индексы созданы (найдено: $INDEXES/3)"

echo ""
echo "3. Проверка CRUD операций..."
echo "-----------------------------------"

# Очистка тестовых данных
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE brand = 'TestCar';" > /dev/null 2>&1

# Добавление записи
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('TestCar', 'TestManufacturer', '2023-01-01', 10000, 500000);" > /dev/null 2>&1
check_test $? "Добавление записи (INSERT)"

# Чтение записи
docker exec autosalon_db psql -U user -d autosalon -c "SELECT * FROM cars WHERE brand = 'TestCar';" | grep TestCar > /dev/null 2>&1
check_test $? "Чтение записи (SELECT)"

# Обновление записи
docker exec autosalon_db psql -U user -d autosalon -c "UPDATE cars SET price = 600000 WHERE brand = 'TestCar';" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "SELECT price FROM cars WHERE brand = 'TestCar';" | grep 600000 > /dev/null 2>&1
check_test $? "Обновление записи (UPDATE)"

# Удаление записи
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE brand = 'TestCar';" > /dev/null 2>&1
DELETED=$(docker exec autosalon_db psql -U user -d autosalon -c "SELECT * FROM cars WHERE brand = 'TestCar';" | grep TestCar | wc -l)
check_test $((DELETED == 0 ? 0 : 1)) "Удаление записи (DELETE)"

echo ""
echo "4. Проверка валидации данных..."
echo "-----------------------------------"

# Попытка вставить отрицательный пробег (должна провалиться)
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('BadCar', 'BadManufacturer', '2023-01-01', -1000, 500000);" > /dev/null 2>&1
check_test $(($? != 0 ? 0 : 1)) "Валидация: отрицательный пробег отклонен"

# Попытка вставить нулевую цену (должна провалиться)
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('BadCar', 'BadManufacturer', '2023-01-01', 10000, 0);" > /dev/null 2>&1
check_test $(($? != 0 ? 0 : 1)) "Валидация: нулевая цена отклонена"

# Попытка вставить пустые поля (должна провалиться)
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('', '', '2023-01-01', 10000, 500000);" > /dev/null 2>&1
check_test $(($? != 0 ? 0 : 1)) "Валидация: пустые поля отклонены"

echo ""
echo "5. Проверка фильтрации по цене..."
echo "-----------------------------------"

# Добавление тестовых данных
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE manufacturer = 'FilterTest';" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Car1', 'FilterTest', '2023-01-01', 10000, 1000000);" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Car2', 'FilterTest', '2023-01-01', 20000, 2000000);" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Car3', 'FilterTest', '2023-01-01', 30000, 3000000);" > /dev/null 2>&1

# Фильтрация по цене <= 1500000
FILTERED=$(docker exec autosalon_db psql -U user -d autosalon -t -c "SELECT COUNT(*) FROM cars WHERE manufacturer = 'FilterTest' AND price <= 1500000;")
check_test $((FILTERED == 1 ? 0 : 1)) "Фильтрация по цене (найдено: $FILTERED, ожидалось: 1)"

# Очистка
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE manufacturer = 'FilterTest';" > /dev/null 2>&1

echo ""
echo "6. Проверка массовых операций..."
echo "-----------------------------------"

# Добавление нескольких записей одного производителя
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE manufacturer = 'MassTest';" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Brand1', 'MassTest', '2023-01-01', 10000, 1000000);" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Brand2', 'MassTest', '2023-01-01', 20000, 2000000);" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Brand3', 'MassTest', '2023-01-01', 30000, 3000000);" > /dev/null 2>&1

# Массовое обновление цены по марке
docker exec autosalon_db psql -U user -d autosalon -c "UPDATE cars SET price = 1500000 WHERE brand = 'Brand1';" > /dev/null 2>&1
UPDATED=$(docker exec autosalon_db psql -U user -d autosalon -t -c "SELECT price FROM cars WHERE brand = 'Brand1';")
check_test $((UPDATED == 1500000 ? 0 : 1)) "Массовое обновление цены"

# Массовое удаление по производителю
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE manufacturer = 'MassTest';" > /dev/null 2>&1
REMAINING=$(docker exec autosalon_db psql -U user -d autosalon -t -c "SELECT COUNT(*) FROM cars WHERE manufacturer = 'MassTest';")
check_test $((REMAINING == 0 ? 0 : 1)) "Массовое удаление по производителю"

echo ""
echo "7. Проверка персистентности данных..."
echo "-----------------------------------"

# Добавление тестовой записи
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE brand = 'PersistTest';" > /dev/null 2>&1
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('PersistTest', 'PersistManufacturer', '2023-01-01', 10000, 500000);" > /dev/null 2>&1

echo "Перезапуск контейнеров..."
docker-compose restart
sleep 10

# Проверка, что данные сохранились
docker exec autosalon_db psql -U user -d autosalon -c "SELECT * FROM cars WHERE brand = 'PersistTest';" | grep PersistTest > /dev/null 2>&1
check_test $? "Данные сохранились после перезапуска"

# Очистка
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE brand = 'PersistTest';" > /dev/null 2>&1

echo ""
echo "8. Проверка производительности..."
echo "-----------------------------------"

# Проверка использования индексов
EXPLAIN_OUTPUT=$(docker exec autosalon_db psql -U user -d autosalon -c "EXPLAIN SELECT * FROM cars WHERE brand = 'TestBrand';")
echo "$EXPLAIN_OUTPUT" | grep -i "index" > /dev/null 2>&1
check_test $? "Индекс используется для поиска по марке"

echo ""
echo "========================================="
echo "Результаты тестирования"
echo "========================================="
echo -e "${GREEN}Пройдено тестов: $PASSED${NC}"
echo -e "${RED}Провалено тестов: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!${NC}"
    echo ""
    echo "Система готова к защите курсовой работы."
    exit 0
else
    echo -e "${RED}✗ НЕКОТОРЫЕ ТЕСТЫ ПРОВАЛЕНЫ${NC}"
    echo ""
    echo "Требуется дополнительная отладка."
    exit 1
fi
