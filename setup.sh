#!/bin/bash

echo "🚀 AutoSalon - Полная автоматическая установка"
echo "=============================================="
if [ "$EUID" -eq 0 ]; then
    echo "❌ Не запускайте этот скрипт с sudo!"
    echo "Просто выполните: ./setup.sh"
    exit 1
fi

check_success() {
    if [ $? -ne 0 ]; then
        echo "❌ Ошибка: $1"
        exit 1
    fi
}

echo "🔍 Проверка системы..."

# Проверить, что это Ubuntu/Debian система
if ! command -v apt &> /dev/null; then
    echo "❌ Этот скрипт работает только на Ubuntu/Debian системах"
    exit 1
fi

echo "📦 Обновление системы..."
sudo apt update
check_success "Не удалось обновить список пакетов"

echo "🐳 Проверка Docker..."

# Проверить, установлен ли Docker
if ! command -v docker &> /dev/null; then
    echo "📥 Установка Docker..."
    sudo apt install -y docker.io
    check_success "Не удалось установить Docker"
else
    echo "✅ Docker уже установлен"
fi

# Проверяем Docker Compose (v1 или v2)
if command -v docker-compose &> /dev/null; then
    DOCKER_COMPOSE="docker-compose"
    echo "✅ Docker Compose v1 уже установлен"
elif docker compose version &> /dev/null 2>&1; then
    DOCKER_COMPOSE="docker compose"
    echo "✅ Docker Compose v2 уже установлен"
else
    echo "📥 Установка Docker Compose..."
    sudo apt install -y docker-compose-v2
    if docker compose version &> /dev/null 2>&1; then
        DOCKER_COMPOSE="docker compose"
    else
        sudo apt install -y docker-compose
        DOCKER_COMPOSE="docker-compose"
    fi
    check_success "Не удалось установить Docker Compose"
fi

echo "📦 Используется: $DOCKER_COMPOSE"

echo "🛠️ Установка дополнительных утилит..."
sudo apt install -y git curl
check_success "Не удалось установить дополнительные утилиты"

echo "🔧 Настройка Docker..."

sudo systemctl start docker
check_success "Не удалось запустить Docker"

sudo systemctl enable docker
check_success "Не удалось включить автозапуск Docker"

sudo usermod -aG docker $USER
check_success "Не удалось добавить пользователя в группу docker"

sudo chmod 666 /var/run/docker.sock
check_success "Не удалось исправить права Docker"

echo "🖥️ Настройка X11 для GUI..."

# Проверяем, что есть графический интерфейс
if [ -z "$DISPLAY" ]; then
    export DISPLAY=:0
    echo "📺 Установлена переменная DISPLAY=:0"
fi

# Разрешаем подключения к X11 серверу
xhost +local:docker 2>/dev/null || echo "⚠️ Не удалось настроить xhost (возможно, нет GUI)"

# Проверяем, что X11 работает
if command -v xdpyinfo &> /dev/null; then
    if xdpyinfo &> /dev/null; then
        echo "✅ X11 сервер работает"
    else
        echo "⚠️ X11 сервер не отвечает, но продолжаем..."
    fi
else
    echo "📦 Установка X11 утилит..."
    sudo apt install -y x11-utils
fi

echo "🚀 Запуск приложения..."

echo "🧹 Очистка предыдущих запусков..."
$DOCKER_COMPOSE down -v 2>/dev/null || true

echo "🔨 Сборка и запуск AutoSalon..."
$DOCKER_COMPOSE up -d --build
check_success "Не удалось запустить приложение"

echo "⏳ Ожидание запуска контейнеров..."
sleep 10

echo "📊 Проверка статуса..."
$DOCKER_COMPOSE ps

if $DOCKER_COMPOSE ps | grep -q "Up"; then
    echo ""
    echo "🎉 УСПЕХ! AutoSalon успешно установлен и запущен!"
    echo "=============================================="
    echo ""
    echo "🖥️ Qt приложение должно открыться автоматически!"
    echo ""
    echo "🔧 Полезные команды:"
    echo "   Остановить:     $DOCKER_COMPOSE down"
    echo "   Перезапустить:  $DOCKER_COMPOSE restart"
    echo "   Логи:          $DOCKER_COMPOSE logs"
    echo ""
    echo "💡 Если приложение не открылось:"
    echo "   1. Убедитесь, что у вас есть графический интерфейс"
    echo "   2. Выполните: export DISPLAY=:0"
    echo "   3. Перезапустите: $DOCKER_COMPOSE restart"
    echo ""
    echo "🐛 Если возникнут проблемы с правами Docker:"
    echo "   ./fix-docker-permissions.sh"
    echo ""
else
    echo "⚠️ Приложение запущено, но возможны проблемы."
    echo "Проверьте логи: $DOCKER_COMPOSE logs"
fi