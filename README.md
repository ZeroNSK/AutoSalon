# AutoSalon

Приложение управления автосалоном на C++ и Qt с PostgreSQL: учёт автомобилей, поиск, фильтрация и операции с инвентарём.

**Примерный период реализации:** декабрь 2025. Период указан по истории Git и отражает основную работу над проектом.

## Требования

### Общие требования
- Docker Engine 20.10+
- Docker Compose 1.29+

### Требования по ОС

**Linux (Ubuntu/Debian):**
- X11 сервер (обычно уже установлен)
- Права на запуск Docker (скрипт настроит автоматически)

**macOS:**
- XQuartz для отображения GUI
- Docker Desktop для macOS

### Установка XQuartz на macOS
```bash
# Через Homebrew
brew install --cask xquartz

# Или скачайте с официального сайта
# https://www.xquartz.org/
```

**После установки XQuartz:**
1. Запустите XQuartz
2. В настройках включите "Allow connections from network clients"
3. Перезапустите компьютер или выйдите/войдите в систему

## 🚀 Быстрый запуск

### Автоматическая установка (Linux и macOS)

```bash
# 1. Клонировать репозиторий
git clone https://github.com/ZeroNSK/AutoSalon.git
cd AutoSalon

# 2. Запустить установку одной командой
chmod +x install.sh && ./install.sh
```

**Скрипт автоматически:**
- 🔍 Определит операционную систему
- 📦 Установит Docker и Docker Compose
- 🔧 Настроит все зависимости
- 🚀 Запустит приложение
- 🌐 Откроет GUI в браузере

### Доступ к приложению

После установки приложение автоматически откроется в браузере:
- **Веб-интерфейс**: http://localhost:6080
- **VNC клиент**: localhost:5900 (пароль не требуется)

## 🎮 Использование

### Основные функции

1. **➕ Добавление автомобиля** - зеленая кнопка в верхней панели
2. **✏️ Редактирование** - выберите строку в таблице, появится панель редактирования
3. **🗑️ Удаление** - удаление конкретного авто или массовое по модели
4. **🔍 Сортировка** - выпадающее меню в правом верхнем углу таблицы
5. **💰 Фильтр по цене** - показать авто дешевле указанной суммы
6. **📁 Экспорт/импорт** - сохранение и чтение данных из файлов

### Альтернативные способы запуска

**Ручной запуск (если Docker уже настроен):**
```bash
docker-compose up -d --build
```

**Остановка приложения:**
```bash
docker-compose down
```

### 3. Проверка статуса

```bash
# Проверить статус контейнеров
docker-compose ps

# Посмотреть логи приложения
docker logs autosalon_app

# Посмотреть логи базы данных
docker logs autosalon_db
```

### 4. Остановка приложения

```bash
# Остановить контейнеры
docker-compose down

# Остановить и удалить данные БД
docker-compose down -v
```

## Архитектура

### Компоненты системы

- **Database Layer** (`database.h/cpp`) - Управление подключением к PostgreSQL
- **Business Logic** (`operations.h/cpp`) - CRUD операции и валидация данных
- **Presentation Layer** (`gui/mainwindow.h/cpp`) - Qt GUI интерфейс
- **PostgreSQL** - Хранение данных с персистентностью через Docker volume

### Технологический стек

- **Язык:** C++17
- **GUI Framework:** Qt5 (Core, Sql, Widgets)
- **База данных:** PostgreSQL 15
- **Контейнеризация:** Docker + Docker Compose
- **GUI отображение:** X11 forwarding (Linux/macOS)

**Индексы для оптимизации:**
- `idx_cars_brand` - для быстрого обновления цен
- `idx_cars_manufacturer` - для быстрого удаления
- `idx_cars_price` - для быстрой фильтрации

## Переменные окружения

Приложение использует следующие переменные окружения (настроены в docker-compose.yml):

- `DB_HOST=db` - Хост базы данных
- `DB_PORT=5432` - Порт PostgreSQL
- `DB_NAME=autosalon` - Имя базы данных
- `DB_USER=user` - Пользователь БД
- `DB_PASSWORD=pass` - Пароль БД
- `DISPLAY=${DISPLAY}` - Display для X11 forwarding

## Разработка

### Локальная сборка (внутри контейнера)

```bash
# Войти в контейнер
docker exec -it autosalon_app bash

# Пересобрать приложение
cd /app
make clean
make

# Запустить приложение
./AutoSalon
```

### 📁 Структура проекта

```
AutoSalon/
├── app/                        # Исходный код приложения
│   ├── src/
│   │   ├── main.cpp           # Точка входа
│   │   ├── database.h/cpp     # Подключение к PostgreSQL
│   │   ├── operations.h/cpp   # Бизнес-логика и CRUD операции
│   │   └── gui/
│   │       └── mainwindow.h/cpp # Qt GUI интерфейс
│   ├── Dockerfile             # Конфигурация контейнера
│   ├── Makefile              # Сборка приложения
│   └── supervisord.conf      # Управление процессами
├── docker-compose.yml         # Оркестрация контейнеров
├── install.sh                # Универсальный установщик
├── setup.sh                  # Установка для Linux
├── setup-macos.sh           # Установка для macOS
└── README.md                # Документация
```

## Устранение неполадок

### Приложение не запускается

```bash
# Проверить логи
docker logs autosalon_app

# Перезапустить контейнеры
docker-compose restart
```

### GUI не отображается

**Linux:**
```bash
# Проверить X11
echo $DISPLAY
xdpyinfo

# Разрешить Docker доступ к X11
xhost +local:docker

# Перезапустить приложение
docker-compose restart app
```

**macOS:**
```bash
# Проверить XQuartz
ps aux | grep Xquartz

# Запустить XQuartz если не запущен
open -a XQuartz

# Настроить X11 forwarding
export DISPLAY=:0
xhost +localhost

# Перезапустить приложение
docker-compose restart app
```

### Не удается подключиться к БД

```bash
# Проверить, что контейнер БД запущен
docker-compose ps

# Проверить логи БД
docker logs autosalon_db

# Проверить подключение
docker exec -it autosalon_db psql -U user -d autosalon
```

### Проблемы с правами Docker (Linux)

```bash
# Быстрое исправление прав
chmod +x fix-docker-permissions.sh
./fix-docker-permissions.sh

# Или вручную
sudo usermod -aG docker $USER
sudo chmod 666 /var/run/docker.sock
```

### Полная переустановка

```bash
# Остановить и удалить всё
docker-compose down -v

# Пересобрать и запустить
./install.sh
```

---
**Технологии**: C++, Qt5, PostgreSQL, Docker
