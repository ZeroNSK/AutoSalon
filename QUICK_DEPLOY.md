# ⚡ Быстрое развертывание AutoSalon

## 🔥 Загрузка на GitHub (5 минут)

```bash
# В папке проекта
git init
git add .
git commit -m "AutoSalon: Car dealership database management system"

# Замените YOUR_USERNAME на ваш GitHub username
git remote add origin https://github.com/YOUR_USERNAME/autosalon-db.git
git branch -M main
git push -u origin main
```

**Создайте репозиторий на GitHub:** https://github.com/new
- Название: `autosalon-db`
- Public/Private - на ваш выбор
- НЕ добавляйте README/gitignore

---

## 🐧 Установка на Linux (3 команды)

### Быстрая установка

```bash
# 1. Установить Docker (Ubuntu/Debian)
sudo apt update && sudo apt install -y docker.io docker-compose

# 2. Клонировать и запустить
git clone https://github.com/YOUR_USERNAME/autosalon-db.git
cd autosalon-db && docker-compose up -d --build

# 3. Открыть в браузере
echo "Откройте: http://localhost:6080"
```

### Для других дистрибутивов

**CentOS/RHEL:**
```bash
sudo yum install -y docker docker-compose
sudo systemctl start docker
```

**Arch Linux:**
```bash
sudo pacman -S docker docker-compose
sudo systemctl start docker
```

---

## ✅ Проверка (30 секунд)

```bash
# Статус контейнеров
docker-compose ps

# Должно показать 2 контейнера: autosalon_app и autosalon_db
# Оба в состоянии "Up"
```

**Откройте браузер:** http://localhost:6080

Должно появиться окно Qt приложения с таблицей автомобилей.

---

## 🚨 Если что-то не работает

```bash
# Посмотреть логи
docker logs autosalon_app
docker logs autosalon_db

# Перезапустить
docker-compose restart

# Пересобрать с нуля
docker-compose down -v
docker-compose up -d --build
```

---

## 🎯 Готово к защите!

**Демонстрация функций:**
1. Просмотр таблицы ✅
2. Добавление записи ✅
3. Поиск по цене ✅
4. Экспорт в файл ✅
5. Чтение файла ✅
6. Обновление цены ✅
7. Удаление по производителю ✅
8. Все данные в PostgreSQL ✅

**URL для демонстрации:** http://localhost:6080

---

## 📱 Удаленный доступ

Если ставите на сервер, замените `localhost` на IP сервера:
- http://YOUR_SERVER_IP:6080

**Откройте порты:**
```bash
sudo ufw allow 6080  # Ubuntu
sudo firewall-cmd --add-port=6080/tcp --permanent && sudo firewall-cmd --reload  # CentOS
```