# Система управления базой данных автосалона

Курсовая работа - Приложение для управления инвентарем автосалона с использованием Qt GUI и PostgreSQL.

## Требования

- Docker Engine 20.10+
- Docker Compose 1.29+
- Современный веб-браузер (Chrome, Firefox, Safari, Edge)

## Быстрый старт

### 1. Сборка и запуск контейнеров

```bash
docker-compose up -d --build
```

Эта команда:
- Соберет Docker образ приложения с Qt и всеми зависимостями
- Запустит контейнер PostgreSQL с базой данных
- Настроит VNC и noVNC для доступа к GUI через браузер

### 2. Доступ к приложению

**Через веб-браузер (рекомендуется):**
- Откройте браузер и перейдите по адресу: `http://localhost:6080`
- Приложение автоматически запустится в графическом окружении

**Через VNC клиент (опционально):**
- Подключитесь к `localhost:5900`

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
- **Удаленный доступ:** VNC + noVNC

## Структура базы данных

```sql
CREATE TABLE cars (
    id SERIAL PRIMARY KEY,
    brand VARCHAR(100) NOT NULL,
    manufacturer VARCHAR(100) NOT NULL,
    release_date DATE NOT NULL,
    mileage INTEGER NOT NULL CHECK (mileage >= 0),
    price INTEGER NOT NULL CHECK (price > 0)
);
```

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
- `DISPLAY=:1` - Display для X11/VNC

## Разработка

### Локальная сборка (внутри контейнера)

```bash
# Войти в контейнер
docker exec -it autosalon_app bash

# Пересобрать приложение
cd /app/build
cmake ..
cmake --build .

# Запустить приложение
./AutoSalon
```

### Структура проекта

```
autosalon/
├── app/
│   ├── Dockerfile              # Конфигурация Docker образа
│   ├── CMakeLists.txt          # Конфигурация сборки CMake
│   ├── src/
│   │   ├── main.cpp            # Точка входа
│   │   ├── database.h/cpp      # Слой доступа к данным
│   │   ├── operations.h/cpp    # Бизнес-логика
│   │   └── gui/
│   │       └── mainwindow.h/cpp # GUI интерфейс
│   └── build/                  # Директория сборки
├── docker-compose.yml          # Оркестрация контейнеров
├── README.md                   # Этот файл
```

## Устранение неполадок

### Приложение не запускается

```bash
# Проверить логи
docker logs autosalon_app

# Перезапустить контейнеры
docker-compose restart
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

### Пересоздать всё с нуля

```bash
# Остановить и удалить всё
docker-compose down -v

# Пересобрать и запустить
docker-compose up -d --build
```

