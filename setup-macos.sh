#!/bin/bash

echo "AutoSalon - Установка для macOS"
echo "=================================="

check_success() {
    if [ $? -ne 0 ]; then
        echo "Ошибка: $1"
        exit 1
    fi
}

echo "🔍 Проверка системы..."

# Проверка Docker
if ! command -v docker &> /dev/null; then
    echo "Docker не найден. Установите Docker Desktop для macOS:"
    echo "https://www.docker.com/products/docker-desktop"
    exit 1
fi

echo "✅ Docker найден"

# Проверка Docker Compose
if command -v docker-compose &> /dev/null; then
    DOCKER_COMPOSE="docker-compose"
    echo "Docker Compose v1 найден"
elif docker compose version &> /dev/null 2>&1; then
    DOCKER_COMPOSE="docker compose"
    echo "Docker Compose v2 найден"
else
    echo "Docker Compose не найден. Установите Docker Desktop для macOS"
    exit 1
fi

echo "🖥️ Настройка X11 для GUI..."

# Проверка XQuartz
if ! command -v xquartz &> /dev/null && ! ls /Applications/Utilities/XQuartz.app &> /dev/null; then
    echo "   XQuartz не найден. Установите его:"
    echo "   brew install --cask xquartz"
    echo "   Или скачайте с https://www.xquartz.org/"
    echo ""
    echo "После установки:"
    echo "1. Запустите XQuartz"
    echo "2. В настройках XQuartz включите 'Allow connections from network clients'"
    echo "3. Перезапустите этот скрипт"
    exit 1
fi

echo "  XQuartz найден"

# Проверка, запущен ли XQuartz
if ! pgrep -x "Xquartz" > /dev/null; then
    echo "  Запуск XQuartz..."
    open -a XQuartz
    echo "  Ожидание запуска XQuartz..."
    sleep 5
fi

# Настройка X11 forwarding для macOS
echo "🔧 Настройка X11 forwarding..."
export DISPLAY=:0
xhost +localhost 2>/dev/null || echo "Не удалось выполнить xhost (XQuartz может быть не готов)"

echo "🧹 Остановка существующих контейнеров..."
$DOCKER_COMPOSE down -v 2>/dev/null || true

echo "🔨 Сборка и запуск приложения..."
$DOCKER_COMPOSE up -d --build
check_success "Не удалось запустить приложение"

echo "  Ожидание запуска контейнеров..."
sleep 15

echo "  Проверка статуса..."
$DOCKER_COMPOSE ps

# Проверка логов приложения
echo ""
echo "  Логи приложения:"
$DOCKER_COMPOSE logs app | tail -10

if $DOCKER_COMPOSE ps | grep -q "Up"; then
    echo ""
    echo "УСПЕХ! AutoSalon успешно установлен!"
    echo "======================================="
    echo ""
    echo "Qt приложение должно открыться в XQuartz!"
    echo ""
    echo "   Если приложение не открылось:"
    echo "   1. Убедитесь, что XQuartz запущен"
    echo "   2. В XQuartz включите 'Allow connections from network clients'"
    echo "   3. Выполните: xhost +localhost"
    echo "   4. Перезапустите: $DOCKER_COMPOSE restart app"
    echo ""
    echo "   Полезные команды:"
    echo "   Остановить:     $DOCKER_COMPOSE down"
    echo "   Перезапустить:  $DOCKER_COMPOSE restart"
    echo "   Логи:          $DOCKER_COMPOSE logs app"
    echo ""
else
    echo "Приложение запущено, но возможны проблемы."
    echo "Проверьте логи: $DOCKER_COMPOSE logs"
fi