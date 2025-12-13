# 🚀 Развертывание проекта AutoSalon

## Загрузка на GitHub

### 1. Создание репозитория на GitHub

1. Зайдите на [github.com](https://github.com)
2. Нажмите "New repository"
3. Назовите репозиторий: `autosalon-db`
4. Выберите "Public" или "Private"
5. НЕ добавляйте README, .gitignore, license (они уже есть)
6. Нажмите "Create repository"

### 2. Инициализация Git и загрузка

```bash
# В папке с проектом autosalon/
git init
git add .
git commit -m "Initial commit: AutoSalon database management system"

# Добавить remote (замените YOUR_USERNAME на ваш GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/autosalon-db.git

# Загрузить на GitHub
git branch -M main
git push -u origin main
```

### 3. Проверка загрузки

Откройте ваш репозиторий на GitHub и убедитесь, что все файлы загружены.

---

## Установка на Linux через Docker

### Вариант 1: Клонирование с GitHub (рекомендуется)

```bash
# 1. Установить Git и Docker (если не установлены)
sudo apt update
sudo apt install -y git docker.io docker-compose

# 2. Запустить Docker сервис
sudo systemctl start docker
sudo systemctl enable docker

# 3. Добавить пользователя в группу docker (чтобы не использовать sudo)
sudo usermod -aG docker $USER
# Перелогиньтесь или выполните: newgrp docker

# 4. Клонировать репозиторий
git clone https://github.com/YOUR_USERNAME/autosalon-db.git
cd autosalon-db

# 5. Запустить приложение
docker-compose up -d --build

# 6. Открыть в браузере
# http://localhost:6080
```

### Вариант 2: Прямая загрузка архива

```bash
# 1. Скачать архив с GitHub
wget https://github.com/YOUR_USERNAME/autosalon-db/archive/refs/heads/main.zip

# 2. Распаковать
unzip main.zip
cd autosalon-db-main

# 3. Запустить (Docker должен быть установлен)
docker-compose up -d --build

# 4. Открыть в браузере
# http://localhost:6080
```

---

## Проверка установки

### 1. Проверить статус контейнеров

```bash
docker-compose ps
```

Должно показать:
```
       Name                     Command               State                    Ports                  
----------------------------------------------------------------------------------------------------
autosalon_app        /usr/bin/supervisord             Up      0.0.0.0:5900->5900/tcp, 0.0.0.0:6080->80/tcp
autosalon_db         docker-entrypoint.sh postgres    Up      0.0.0.0:5432->5432/tcp
```

### 2. Проверить логи

```bash
# Логи приложения
docker logs autosalon_app

# Логи базы данных
docker logs autosalon_db
```

### 3. Проверить доступ к GUI

1. Откройте браузер
2. Перейдите на `http://localhost:6080`
3. Должно появиться окно с Qt приложением

### 4. Проверить базу данных

```bash
# Подключиться к PostgreSQL
docker exec -it autosalon_db psql -U user -d autosalon

# Внутри psql:
\dt                    # Показать таблицы
SELECT * FROM cars;    # Показать данные
\q                     # Выйти
```

---

## Системные требования для Linux

### Минимальные требования

- **ОС:** Ubuntu 18.04+, CentOS 7+, Debian 9+, или любой Linux с Docker
- **RAM:** 4 ГБ
- **Диск:** 2 ГБ свободного места
- **CPU:** 2 ядра
- **Сеть:** Доступ к интернету для загрузки Docker образов

### Установка Docker на разных дистрибутивах

#### Ubuntu/Debian

```bash
# Обновить пакеты
sudo apt update

# Установить зависимости
sudo apt install -y apt-transport-https ca-certificates curl gnupg lsb-release

# Добавить GPG ключ Docker
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

# Добавить репозиторий Docker
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# Установить Docker
sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin

# Или установить docker-compose отдельно
sudo apt install -y docker-compose
```

#### CentOS/RHEL

```bash
# Установить зависимости
sudo yum install -y yum-utils

# Добавить репозиторий Docker
sudo yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo

# Установить Docker
sudo yum install -y docker-ce docker-ce-cli containerd.io

# Установить docker-compose
sudo curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose

# Запустить Docker
sudo systemctl start docker
sudo systemctl enable docker
```

---

## Управление приложением

### Запуск

```bash
docker-compose up -d --build
```

### Остановка

```bash
docker-compose down
```

### Перезапуск

```bash
docker-compose restart
```

### Обновление кода

```bash
# Получить последние изменения
git pull origin main

# Пересобрать и запустить
docker-compose down
docker-compose up -d --build
```

### Полная очистка (удаление данных БД)

```bash
docker-compose down -v
docker system prune -f
```

---

## Доступ к приложению

### Локальный доступ

- **GUI в браузере:** http://localhost:6080
- **VNC клиент:** localhost:5900
- **PostgreSQL:** localhost:5432

### Удаленный доступ

Если устанавливаете на удаленный сервер, замените `localhost` на IP адрес сервера:

- **GUI в браузере:** http://YOUR_SERVER_IP:6080
- **VNC клиент:** YOUR_SERVER_IP:5900

**Важно:** Убедитесь, что порты 6080 и 5900 открыты в файрволе!

```bash
# Ubuntu/Debian
sudo ufw allow 6080
sudo ufw allow 5900

# CentOS/RHEL
sudo firewall-cmd --permanent --add-port=6080/tcp
sudo firewall-cmd --permanent --add-port=5900/tcp
sudo firewall-cmd --reload
```

---

## Устранение проблем

### Проблема: "Permission denied" при запуске Docker

**Решение:**
```bash
sudo usermod -aG docker $USER
newgrp docker
# Или перелогиньтесь
```

### Проблема: Порт уже занят

**Решение:**
```bash
# Найти процесс, использующий порт
sudo netstat -tulpn | grep :6080

# Остановить процесс или изменить порт в docker-compose.yml
```

### Проблема: GUI не отображается

**Решение:**
1. Подождите 1-2 минуты после запуска
2. Обновите страницу браузера
3. Проверьте логи: `docker logs autosalon_app`

### Проблема: Ошибка подключения к БД

**Решение:**
```bash
# Проверить статус БД
docker logs autosalon_db

# Перезапустить БД
docker-compose restart db
```

---

## Готово! 🎉

Теперь ваше приложение AutoSalon:
- ✅ Загружено на GitHub
- ✅ Установлено на Linux
- ✅ Доступно через браузер
- ✅ Готово к демонстрации

**Для демонстрации откройте:** http://localhost:6080 (или http://YOUR_SERVER_IP:6080)