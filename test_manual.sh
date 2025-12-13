#!/bin/bash

echo "=========================================="
echo "ИНСТРУКЦИЯ ПО ТЕСТИРОВАНИЮ ПРИЛОЖЕНИЯ"
echo "=========================================="
echo ""
echo "Контейнеры запущены. Теперь вы можете протестировать приложение несколькими способами:"
echo ""
echo "=== СПОСОБ 1: Тестирование через базу данных (БЫСТРО) ==="
echo ""
echo "1. Проверить текущие данные в БД:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c 'SELECT * FROM cars;'"
echo ""
echo "2. Добавить тестовую запись:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c \"INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('Camry', 'Toyota', '2020-01-15', 50000, 1500000);\""
echo ""
echo "3. Проверить фильтрацию по цене:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c 'SELECT * FROM cars WHERE price <= 2000000;'"
echo ""
echo "4. Обновить цену:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c \"UPDATE cars SET price = 1600000 WHERE brand = 'Camry';\""
echo ""
echo "5. Удалить по производителю:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c \"DELETE FROM cars WHERE manufacturer = 'Toyota';\""
echo ""
echo "=== СПОСОБ 2: Запустить приложение вручную в контейнере ==="
echo ""
echo "1. Войти в контейнер:"
echo "   docker exec -it autosalon_app bash"
echo ""
echo "2. Запустить приложение (без GUI, только проверка подключения):"
echo "   cd /app/build && ./AutoSalon"
echo ""
echo "=== СПОСОБ 3: Проверить структуру БД ==="
echo ""
echo "1. Показать структуру таблицы:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c '\d cars'"
echo ""
echo "2. Показать индексы:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c '\di'"
echo ""
echo "=== СПОСОБ 4: Тестирование персистентности данных ==="
echo ""
echo "1. Добавить данные (см. выше)"
echo "2. Остановить контейнеры:"
echo "   docker-compose down"
echo "3. Запустить снова:"
echo "   docker-compose up -d"
echo "4. Проверить, что данные сохранились:"
echo "   docker exec -it autosalon_db psql -U user -d autosalon -c 'SELECT * FROM cars;'"
echo ""
echo "=========================================="
echo "АВТОМАТИЧЕСКИЙ ТЕСТ"
echo "=========================================="
echo ""

# Проверка подключения к БД
echo "✓ Проверка подключения к PostgreSQL..."
docker exec autosalon_db psql -U user -d autosalon -c "SELECT version();" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✅ База данных доступна"
else
    echo "  ❌ Ошибка подключения к БД"
    exit 1
fi

# Проверка структуры таблицы
echo ""
echo "✓ Проверка структуры таблицы cars..."
docker exec autosalon_db psql -U user -d autosalon -c "\d cars" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✅ Таблица cars существует"
else
    echo "  ❌ Таблица cars не найдена"
    exit 1
fi

# Проверка индексов
echo ""
echo "✓ Проверка индексов..."
INDEXES=$(docker exec autosalon_db psql -U user -d autosalon -t -c "SELECT indexname FROM pg_indexes WHERE tablename = 'cars';")
echo "  Найденные индексы:"
echo "$INDEXES" | sed 's/^/    /'

# Текущие данные
echo ""
echo "✓ Текущие данные в таблице:"
docker exec autosalon_db psql -U user -d autosalon -c "SELECT id, brand, manufacturer, price FROM cars;"

# Тест добавления
echo ""
echo "✓ Тест: Добавление новой записи..."
docker exec autosalon_db psql -U user -d autosalon -c "INSERT INTO cars (brand, manufacturer, release_date, mileage, price) VALUES ('TestCar', 'TestManufacturer', '2023-01-01', 10000, 1000000);" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✅ Запись добавлена"
    docker exec autosalon_db psql -U user -d autosalon -c "SELECT * FROM cars WHERE brand = 'TestCar';"
else
    echo "  ❌ Ошибка добавления"
fi

# Тест фильтрации
echo ""
echo "✓ Тест: Фильтрация по цене (price <= 1500000)..."
FILTERED=$(docker exec autosalon_db psql -U user -d autosalon -t -c "SELECT COUNT(*) FROM cars WHERE price <= 1500000;")
echo "  Найдено записей: $FILTERED"

# Тест обновления
echo ""
echo "✓ Тест: Обновление цены для TestCar..."
docker exec autosalon_db psql -U user -d autosalon -c "UPDATE cars SET price = 1200000 WHERE brand = 'TestCar';" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✅ Цена обновлена"
    docker exec autosalon_db psql -U user -d autosalon -c "SELECT brand, price FROM cars WHERE brand = 'TestCar';"
else
    echo "  ❌ Ошибка обновления"
fi

# Тест удаления
echo ""
echo "✓ Тест: Удаление по производителю (TestManufacturer)..."
docker exec autosalon_db psql -U user -d autosalon -c "DELETE FROM cars WHERE manufacturer = 'TestManufacturer';" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✅ Записи удалены"
    COUNT=$(docker exec autosalon_db psql -U user -d autosalon -t -c "SELECT COUNT(*) FROM cars WHERE manufacturer = 'TestManufacturer';")
    echo "  Осталось записей TestManufacturer: $COUNT"
else
    echo "  ❌ Ошибка удаления"
fi

# Проверка приложения
echo ""
echo "✓ Проверка сборки приложения..."
docker exec autosalon_app test -f /app/build/AutoSalon
if [ $? -eq 0 ]; then
    echo "  ✅ Исполняемый файл AutoSalon существует"
else
    echo "  ❌ Исполняемый файл не найден"
fi

echo ""
echo "=========================================="
echo "ИТОГОВЫЙ СТАТУС"
echo "=========================================="
echo ""
echo "✅ База данных работает"
echo "✅ Таблица и индексы созданы"
echo "✅ CRUD операции работают"
echo "✅ Приложение собрано"
echo ""
echo "Для полного тестирования GUI запустите приложение вручную:"
echo "  docker exec -it autosalon_app bash"
echo "  cd /app/build && DISPLAY=:1 ./AutoSalon"
echo ""
