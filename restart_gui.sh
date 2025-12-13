#!/bin/bash

echo "=========================================="
echo "ПОЛНЫЙ ПЕРЕЗАПУСК GUI"
echo "=========================================="
echo ""

# Остановить приложение
echo "1. Остановка приложения..."
docker exec autosalon_app pkill -f AutoSalon 2>/dev/null
sleep 2

# Полная пересборка
echo "2. Полная пересборка приложения..."
docker exec autosalon_app bash -c "cd /app/build && rm -rf * && cmake .. && cmake --build ." | tail -10

# Запустить
echo "3. Запуск приложения..."
docker exec -d autosalon_app bash -c "cd /app/build && QT_QPA_PLATFORM=vnc:port=5900:size=1400x800 ./AutoSalon"
sleep 3

# Проверить
echo "4. Проверка..."
if docker exec autosalon_app pgrep -f AutoSalon > /dev/null; then
    echo "✅ Приложение запущено!"
    echo ""
    echo "=========================================="
    echo "ГОТОВО!"
    echo "=========================================="
    echo ""
    echo "Теперь:"
    echo "1. ОТКЛЮЧИТЕСЬ от текущей VNC сессии"
    echo "2. ПОДКЛЮЧИТЕСЬ ЗАНОВО к localhost:5900"
    echo "3. Проверьте изменения:"
    echo "   - Формат даты: ДД.ММ.ГГГГ"
    echo "   - Кнопка 'Удалить выбранный'"
    echo "   - Колонки: Производитель | Модель"
    echo ""
else
    echo "❌ Ошибка запуска!"
    echo ""
    echo "Проверьте логи:"
    docker exec autosalon_app bash -c "cd /app/build && QT_QPA_PLATFORM=vnc ./AutoSalon 2>&1 | head -20"
fi
