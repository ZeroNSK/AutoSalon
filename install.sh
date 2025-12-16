#!/bin/bash

echo "🚀 AutoSalon - Универсальная установка"
echo "====================================="

# Проверка прав
if [ "$EUID" -eq 0 ]; then
    echo "❌ Не запускайте этот скрипт с sudo!"
    echo "Просто выполните: ./install.sh"
    exit 1
fi

check_success() {
    if [ $? -ne 0 ]; then
        echo "❌ Ошибка: $1"
        exit 1
    fi
}

# Определение операционной системы
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt &> /dev/null; then
            echo "ubuntu"
        elif command -v yum &> /dev/null; then
            echo "centos"
        else
            echo "linux"
        fi
    else
        echo "unknown"
    fi
}

OS=$(detect_os)
echo "🔍 Обнаружена ОС: $OS"

# Установка Docker в зависимости от ОС
install_docker() {
    case $OS in
        "macos")
            if ! command -v docker &> /dev/null; then
                echo "❌ Docker не найден. Установите Docker Desktop для macOS:"
                echo "   https://www.docker.com/products/docker-desktop"
                echo "   Или используйте: brew install --cask docker"
                exit 1
            fi
            echo "✅ Docker найден"
            ;;
        "ubuntu")
            if ! command -v docker &> /dev/null; then
                echo "📥 Установка Docker..."
                sudo apt update
                sudo apt install -y docker.io
                check_success "Не удалось установить Docker"
                
                sudo systemctl start docker
                sudo systemctl enable docker
                sudo usermod -aG docker $USER
                sudo chmod 666 /var/run/docker.sock
            else
                echo "✅ Docker уже установлен"
            fi
            ;;
        *)
            echo "❌ Неподдерживаемая ОС: $OS"
            exit 1
            ;;
    esac
}

# Проверка Docker Compose
check_docker_compose() {
    if command -v docker-compose &> /dev/null; then
        DOCKER_COMPOSE="docker-compose"
        echo "✅ Docker Compose v1 найден"
    elif docker compose version &> /dev/null 2>&1; then
        DOCKER_COMPOSE="docker compose"
        echo "✅ Docker Compose v2 найден"
    else
        case $OS in
            "macos")
                echo "❌ Docker Compose не найден. Установите Docker Desktop"
                exit 1
                ;;
            "ubuntu")
                echo "📥 Установка Docker Compose..."
                sudo apt install -y docker-compose-v2
                if docker compose version &> /dev/null 2>&1; then
                    DOCKER_COMPOSE="docker compose"
                else
                    sudo apt install -y docker-compose
                    DOCKER_COMPOSE="docker-compose"
                fi
                check_success "Не удалось установить Docker Compose"
                ;;
        esac
    fi
}

# Функция открытия браузера
open_browser() {
    local url="http://localhost:6080"
    echo "🌐 Открытие браузера..."
    
    case $OS in
        "macos")
            open "$url" 2>/dev/null || echo "⚠️ Не удалось автоматически открыть браузер"
            ;;
        "ubuntu")
            if command -v xdg-open &> /dev/null; then
                xdg-open "$url" 2>/dev/null || echo "⚠️ Не удалось автоматически открыть браузер"
            elif command -v firefox &> /dev/null; then
                firefox "$url" 2>/dev/null &
            elif command -v google-chrome &> /dev/null; then
                google-chrome "$url" 2>/dev/null &
            elif command -v chromium-browser &> /dev/null; then
                chromium-browser "$url" 2>/dev/null &
            else
                echo "⚠️ Браузер не найден. Откройте вручную: $url"
            fi
            ;;
    esac
}

main() {
    echo "🔧 Установка Docker..."
    install_docker
    
    echo "🔧 Проверка Docker Compose..."
    check_docker_compose
    
    echo "🧹 Остановка существующих контейнеров..."
    $DOCKER_COMPOSE down -v 2>/dev/null || true
    
    echo "🔨 Сборка и запуск AutoSalon..."
    $DOCKER_COMPOSE up -d --build
    check_success "Не удалось запустить приложение"
    
    echo "⏳ Ожидание запуска контейнеров..."
    sleep 20
    
    echo "📊 Проверка статуса..."
    $DOCKER_COMPOSE ps
    
    # Проверка логов приложения
    echo ""
    echo "📋 Логи приложения:"
    $DOCKER_COMPOSE logs app | tail -10
    
    if $DOCKER_COMPOSE ps | grep -q "Up"; then
        echo ""
        echo "🎉 УСПЕХ! AutoSalon успешно установлен и запущен!"
        echo "=============================================="
        echo ""
        echo "🖥️ Доступ к приложению:"
        echo "   Веб-интерфейс: http://localhost:6080"
        echo "   VNC клиент:    localhost:5900"
        echo ""
        
        open_browser
        
        echo "🔧 Полезные команды:"
        echo "   Остановить:     $DOCKER_COMPOSE down"
        echo "   Перезапустить:  $DOCKER_COMPOSE restart"
        echo "   Логи:          $DOCKER_COMPOSE logs app"
        echo ""
        echo "💡 Если GUI не отображается:"
        echo "   1. Подождите 30-60 секунд для полной загрузки"
        echo "   2. Обновите страницу в браузере"
        echo "   3. Проверьте логи: $DOCKER_COMPOSE logs app"
        echo ""
    else
        echo "⚠️ Приложение запущено, но возможны проблемы."
        echo "Проверьте логи: $DOCKER_COMPOSE logs"
    fi
}

main
