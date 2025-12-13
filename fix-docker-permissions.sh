#!/bin/bash

# Quick Docker Permissions Fix
# Быстрое исправление прав Docker

echo "🔧 Исправление прав Docker..."

# Проверить, запущен ли Docker
if ! systemctl is-active --quiet docker; then
    echo "🚀 Запуск Docker..."
    sudo systemctl start docker
fi

# Исправить права на Docker socket
sudo chmod 666 /var/run/docker.sock

# Добавить пользователя в группу docker (если еще не добавлен)
sudo usermod -aG docker $USER

echo "✅ Права исправлены!"
echo ""
echo "🎯 Теперь можете запустить:"
echo "   docker-compose up -d --build"
echo ""
echo "💡 Для постоянного решения выполните:"
echo "   newgrp docker"
echo "   или перелогиньтесь в систему"