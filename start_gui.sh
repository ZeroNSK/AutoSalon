#!/bin/bash

echo "=========================================="
echo "ЗАПУСК GUI ПРИЛОЖЕНИЯ"
echo "=========================================="
echo ""

# Проверка, что контейнеры запущены
if ! docker ps | grep -q autosalon_app; then
    echo "❌ Контейнер autosalon_app не запущен!"
    echo "Запустите: docker-compose up -d"
    exit 1
fi

echo "✓ Контейнеры запущены"
echo ""

# Вариант 1: Запуск с Qt VNC платформой
echo "=========================================="
echo "ВАРИАНТ 1: Qt VNC (встроенный VNC сервер)"
echo "=========================================="
echo ""
echo "Запускаю приложение с встроенным VNC сервером Qt..."
echo ""

# Остановить предыдущий процесс, если есть
docker exec autosalon_app pkill -f AutoSalon 2>/dev/null

# Запустить приложение с VNC платформой (размер окна 1400x800)
docker exec -d autosalon_app bash -c "cd /app/build && QT_QPA_PLATFORM=vnc:port=5900:size=1400x800 ./AutoSalon"

sleep 3

# Проверить, запустилось ли
if docker exec autosalon_app pgrep -f AutoSalon > /dev/null; then
    echo "✅ Приложение запущено!"
    echo ""
    echo "=========================================="
    echo "КАК ПОДКЛЮЧИТЬСЯ:"
    echo "=========================================="
    echo ""
    echo "1. Установите VNC клиент (если еще не установлен):"
    echo "   - macOS: RealVNC Viewer (https://www.realvnc.com/download/viewer/)"
    echo "   - или: brew install --cask vnc-viewer"
    echo ""
    echo "2. Откройте VNC клиент и подключитесь к:"
    echo "   localhost:5900"
    echo ""
    echo "3. Пароль не требуется (или попробуйте: password)"
    echo ""
    echo "=========================================="
    echo ""
    echo "Для остановки приложения:"
    echo "  docker exec autosalon_app pkill -f AutoSalon"
    echo ""
else
    echo "❌ Не удалось запустить приложение"
    echo ""
    echo "Проверьте логи:"
    echo "  docker exec autosalon_app bash -c 'cd /app/build && QT_QPA_PLATFORM=vnc ./AutoSalon 2>&1 | head -20'"
fi

echo ""
echo "=========================================="
echo "АЛЬТЕРНАТИВНЫЙ ВАРИАНТ: Offscreen режим"
echo "=========================================="
echo ""
echo "Если VNC не работает, можно запустить в offscreen режиме"
echo "для тестирования логики без GUI:"
echo ""
echo "  docker exec -it autosalon_app bash"
echo "  cd /app/build"
echo "  QT_QPA_PLATFORM=offscreen ./AutoSalon"
echo ""
