# Документ проектирования

## Обзор

Система АвтоСалон - это настольное приложение для управления базой данных автосалона, построенное на C++ с использованием фреймворка Qt для графического интерфейса и PostgreSQL для хранения данных. Приложение работает в Docker контейнерах и предоставляет доступ к GUI через VNC/noVNC в веб-браузере.

Система следует объектно-ориентированной архитектуре с четким разделением ответственности между слоями: представления (GUI), бизнес-логики (Operations) и доступа к данным (Database).

## Архитектура

### Общая архитектура системы

Система использует трехслойную архитектуру:

```
┌─────────────────────────────────────┐
│     Presentation Layer (Qt GUI)     │
│         MainWindow Class            │
└──────────────┬──────────────────────┘
               │
               ▼
┌─────────────────────────────────────┐
│    Business Logic Layer             │
│      Operations Class               │
└──────────────┬──────────────────────┘
               │
               ▼
┌─────────────────────────────────────┐
│    Data Access Layer                │
│      Database Class                 │
└──────────────┬──────────────────────┘
               │
               ▼
┌─────────────────────────────────────┐
│      PostgreSQL Database            │
└─────────────────────────────────────┘
```

### Архитектура развертывания

```
┌──────────────────────────────────────────────┐
│           Docker Host (Linux)                │
│                                              │
│  ┌────────────────────────────────────────┐ │
│  │   App Container                        │ │
│  │   ┌──────────────────────────────┐    │ │
│  │   │  VNC Server (port 5900)      │    │ │
│  │   │  noVNC (port 6080)           │    │ │
│  │   │  ┌────────────────────────┐  │    │ │
│  │   │  │  AutoSalon Qt App      │  │    │ │
│  │   │  └────────────────────────┘  │    │ │
│  │   └──────────────────────────────┘    │ │
│  └────────────────────────────────────────┘ │
│                    │                         │
│                    │ TCP/IP                  │
│                    ▼                         │
│  ┌────────────────────────────────────────┐ │
│  │   DB Container                         │ │
│  │   PostgreSQL (port 5432)               │ │
│  │   Volume: pgdata                       │ │
│  └────────────────────────────────────────┘ │
└──────────────────────────────────────────────┘
                    │
                    │ HTTP
                    ▼
         ┌──────────────────────┐
         │   Web Browser        │
         │   (localhost:6080)   │
         └──────────────────────┘
```

## Компоненты и интерфейсы

### 1. Database Class (Слой доступа к данным)

**Ответственность:**
- Управление подключением к PostgreSQL
- Инициализация схемы базы данных
- Предоставление доступа к объекту QSqlDatabase

**Интерфейс:**

```cpp
class Database {
public:
    // Установка соединения с PostgreSQL
    static bool connect();
    
    // Создание таблицы cars, если она не существует
    static bool initTable();
    
    // Получение экземпляра базы данных для выполнения запросов
    static QSqlDatabase getDatabase();
    
private:
    static const QString DB_HOST;
    static const QString DB_NAME;
    static const QString DB_USER;
    static const QString DB_PASSWORD;
    static const int DB_PORT;
};
```

**Детали реализации:**
- Использует Qt SQL модуль (QSqlDatabase, QSqlQuery)
- Драйвер: QPSQL (PostgreSQL)
- Параметры подключения читаются из переменных окружения или констант
- Таблица `cars` содержит поля: id (SERIAL PRIMARY KEY), brand (VARCHAR), manufacturer (VARCHAR), release_date (DATE), mileage (INTEGER), price (INTEGER)

### 2. Operations Class (Слой бизнес-логики)

**Ответственность:**
- Выполнение CRUD операций над записями автомобилей
- Валидация входных данных
- Экспорт и импорт данных
- Обработка ошибок базы данных

**Интерфейс:**

```cpp
class Operations {
public:
    // Добавление новой записи об автомобиле
    // Возвращает true при успехе, false при ошибке
    static bool addCar(const QString& brand, 
                      const QString& manufacturer, 
                      const QDate& releaseDate, 
                      int mileage, 
                      int price);
    
    // Удаление всех автомобилей указанного производителя
    // Возвращает количество удаленных записей
    static int deleteByManufacturer(const QString& manufacturer);
    
    // Обновление цены для всех автомобилей указанной марки
    // Возвращает количество обновленных записей
    static int updatePriceByBrand(const QString& brand, int newPrice);
    
    // Экспорт данных (марка, пробег, цена) в текстовый файл
    // Формат: "Brand: <brand>, Mileage: <mileage>, Price: <price>"
    static bool exportToFile(const QString& filename);
    
    // Чтение содержимого файла
    static QString readFileContent(const QString& filename);
    
    // Валидация входных данных
    static bool validateCarData(const QString& brand,
                               const QString& manufacturer,
                               const QDate& releaseDate,
                               int mileage,
                               int price,
                               QString& errorMessage);
};
```

**Детали реализации:**
- Все методы статические для простоты использования
- Использует QSqlQuery для выполнения SQL запросов
- Применяет prepared statements для защиты от SQL injection
- Валидация: непустые строки, положительные числа, корректные даты
- Экспорт использует QFile и QTextStream
- Обработка ошибок через возвращаемые значения и сообщения

### 3. MainWindow Class (Слой представления)

**Ответственность:**
- Отображение GUI
- Обработка пользовательского ввода
- Отображение данных в табличном виде
- Показ диалогов для ввода данных и сообщений об ошибках

**Интерфейс:**

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Обработчики событий кнопок
    void onAddCar();
    void onDeleteByManufacturer();
    void onUpdatePrice();
    void onFilterByPrice();
    void onExportToFile();
    void onShowFile();
    void onRefreshTable();

private:
    void setupUi();
    void loadTableData();
    void showError(const QString& message);
    void showSuccess(const QString& message);
    
    // UI компоненты
    QSqlTableModel *model;
    QTableView *tableView;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QPushButton *btnUpdate;
    QPushButton *btnFilter;
    QPushButton *btnExport;
    QPushButton *btnShowFile;
    QPushButton *btnRefresh;
};
```

**Детали реализации:**
- Использует QSqlTableModel для отображения данных из БД
- QTableView для табличного представления
- Диалоги ввода: QInputDialog для простых полей, QDialog для сложных форм
- Сообщения: QMessageBox для ошибок и подтверждений
- Layout: QVBoxLayout и QHBoxLayout для организации элементов
- Кнопки с иконками и подсказками (tooltips)

## Модели данных

### Таблица cars

```sql
CREATE TABLE IF NOT EXISTS cars (
    id SERIAL PRIMARY KEY,
    nameCar VARCHAR(100) NOT NULL,
    brand VARCHAR(100) NOT NULL,
    release_date DATE NOT NULL,
    kilometrs INTEGER NOT NULL CHECK (kilometrs >= 0),
    price INTEGER NOT NULL CHECK (price > 0)
);
```

**Поля:**
- `id`: Уникальный идентификатор (автоинкремент)
- `nameCar`: Имя автомобиля (например, "Camry", "Model 3")
- `brand`: Производитель (например, "Toyota", "Tesla")
- `release_date`: Дата выпуска автомобиля
- `kilometrs`: Пробег в километрах (неотрицательное число)
- `price`: Цена в рублях (положительное число)

**Индексы:**
- PRIMARY KEY на id
- Рекомендуется создать индекс на brand для ускорения поиска
- Рекомендуется создать индекс на manufacturer для ускорения удаления

### Формат файла экспорта

```
Brand: Toyota Camry, Mileage: 50000, Price: 1500000
Brand: Tesla Model 3, Mileage: 20000, Price: 3500000
Brand: BMW X5, Mileage: 75000, Price: 2800000
```

Каждая строка содержит одну запись с тремя полями, разделенными запятыми.


## Correctness Properties

*A property is a characteristic or behavior that should hold true across all valid executions of a system-essentially, a formal statement about what the system should do. Properties serve as the bridge between human-readable specifications and machine-verifiable correctness guarantees.*

### Property 1: Идемпотентность инициализации таблицы

*For any* состояние базы данных (таблица существует или нет), вызов initTable() должен успешно завершиться и гарантировать существование таблицы cars с корректной схемой.

**Validates: Requirements 1.2**

### Property 2: Полнота отображения данных

*For any* запись об автомобиле в базе данных, при отображении в табличном представлении все поля (марка, производитель, дата выпуска, пробег, цена) должны быть видны.

**Validates: Requirements 2.3**

### Property 3: Корректность фильтрации по цене

*For any* максимальное значение цены P, все записи, отображаемые после применения фильтра, должны иметь цену <= P.

**Validates: Requirements 3.1**

### Property 4: Валидация нечисловых значений цены

*For any* нечисловая строка, предоставленная как значение цены, система должна отклонить ввод и отобразить сообщение об ошибке.

**Validates: Requirements 3.2**

### Property 5: Round-trip фильтрации

*For any* начальное состояние таблицы, применение фильтра по цене и последующее его снятие должно вернуть таблицу к исходному состоянию (отображение всех записей).

**Validates: Requirements 3.4**

### Property 6: Полнота экспорта данных

*For any* набор записей в базе данных, экспортированный файл должен содержать ровно столько же строк, сколько записей в БД, и каждая строка должна содержать марку, пробег и цену соответствующей записи.

**Validates: Requirements 4.1**

### Property 7: Корректность формата экспорта

*For any* запись об автомобиле, её представление в экспортированном файле должно соответствовать формату "Brand: <brand>, Mileage: <mileage>, Price: <price>" и находиться на отдельной строке.

**Validates: Requirements 4.2**

### Property 8: Round-trip экспорта и чтения

*For any* набор данных в базе данных, экспорт в файл и последующее чтение этого файла должно вернуть данные в том же формате и с тем же содержимым.

**Validates: Requirements 5.1, 5.4**

### Property 9: Персистентность добавления записи

*For any* валидные данные об автомобиле (непустые строки, корректная дата, положительные числа), после вызова addCar() запись должна существовать в базе данных и быть доступной для чтения.

**Validates: Requirements 6.1**

### Property 10: Отклонение пустых полей

*For any* набор данных, где хотя бы одно обязательное поле (марка, производитель, дата выпуска, пробег, цена) пустое или невалидное, система должна отклонить добавление записи.

**Validates: Requirements 6.2**

### Property 11: Валидация типов данных

*For any* данные с невалидными типами (нечисловой пробег, нечисловая цена, некорректная дата), система должна отклонить ввод и отобразить соответствующее сообщение об ошибке.

**Validates: Requirements 6.3**

### Property 12: Синхронизация UI после добавления

*For any* успешно добавленная запись, табличное представление должно немедленно отобразить эту запись без необходимости ручного обновления.

**Validates: Requirements 6.4**

### Property 13: Полнота удаления по производителю

*For any* название производителя M, после выполнения deleteByManufacturer(M) в базе данных не должно остаться ни одной записи с manufacturer = M.

**Validates: Requirements 7.1**

### Property 14: Синхронизация UI после удаления

*For any* успешно выполненное удаление записей, табличное представление должно немедленно отразить изменения (удаленные записи не должны отображаться).

**Validates: Requirements 7.3**

### Property 15: Полнота обновления цены по марке

*For any* марка автомобиля B и новая цена P, после выполнения updatePriceByBrand(B, P) все записи в базе данных с brand = B должны иметь price = P.

**Validates: Requirements 8.1**

### Property 16: Валидация нечисловой цены при обновлении

*For any* нечисловое значение, предоставленное как новая цена при обновлении, система должна отклонить ввод и отобразить сообщение об ошибке.

**Validates: Requirements 8.2**

### Property 17: Синхронизация UI после обновления

*For any* успешно выполненное обновление цен, табличное представление должно немедленно отобразить новые значения цен без необходимости ручного обновления.

**Validates: Requirements 8.4**

### Property 18: Производительность основных операций

*For any* основная операция (просмотр, добавление, удаление, обновление) на типичном наборе данных (до 1000 записей), время выполнения должно быть <= 2 секунд.

**Validates: Requirements 11.2**

## Обработка ошибок

### Стратегия обработки ошибок

Система использует многоуровневую стратегию обработки ошибок:

1. **Валидация на уровне UI:**
   - Проверка пустых полей перед отправкой
   - Проверка типов данных (числа, даты)
   - Немедленная обратная связь пользователю

2. **Валидация на уровне бизнес-логики:**
   - Класс Operations выполняет дополнительную валидацию
   - Проверка бизнес-правил (положительные цены, неотрицательный пробег)
   - Возврат детальных сообщений об ошибках

3. **Обработка ошибок базы данных:**
   - Использование try-catch для перехвата исключений Qt SQL
   - Логирование ошибок для отладки
   - Дружественные сообщения пользователю

4. **Обработка ошибок файловой системы:**
   - Проверка существования файлов перед чтением
   - Проверка прав доступа
   - Обработка ошибок записи (нет места на диске, и т.д.)

### Типы ошибок и их обработка

| Тип ошибки | Обработка | Пользовательское сообщение |
|------------|-----------|----------------------------|
| Ошибка подключения к БД | Показать QMessageBox::Critical, блокировать операции | "Не удалось подключиться к базе данных. Проверьте настройки подключения." |
| Пустые обязательные поля | Показать QMessageBox::Warning, не выполнять операцию | "Все поля должны быть заполнены." |
| Невалидный тип данных | Показать QMessageBox::Warning, не выполнять операцию | "Пробег и цена должны быть числами." |
| Ошибка SQL запроса | Показать QMessageBox::Critical, откатить транзакцию | "Ошибка выполнения операции: [детали ошибки]" |
| Файл не найден | Показать QMessageBox::Warning | "Файл не найден: [путь к файлу]" |
| Ошибка записи файла | Показать QMessageBox::Critical | "Не удалось записать файл: [причина]" |
| Нет результатов поиска | Показать QMessageBox::Information | "Не найдено автомобилей с ценой <= [значение]" |

### Транзакционность

Для обеспечения целостности данных критические операции выполняются в транзакциях:

```cpp
QSqlDatabase db = Database::getDatabase();
db.transaction();

try {
    // Выполнение операций
    if (success) {
        db.commit();
    } else {
        db.rollback();
    }
} catch (...) {
    db.rollback();
    throw;
}
```

Операции, требующие транзакций:
- Массовое удаление записей
- Массовое обновление цен
- Добавление записи с валидацией

## Стратегия тестирования

### Подход к тестированию

Система использует двойной подход к тестированию:

1. **Unit тесты** - проверяют конкретные примеры и граничные случаи
2. **Property-based тесты** - проверяют универсальные свойства на множестве входных данных

Оба типа тестов дополняют друг друга: unit тесты ловят конкретные баги, property тесты проверяют общую корректность.

### Фреймворк для тестирования

**Property-Based Testing:** Будет использоваться библиотека **RapidCheck** для C++

RapidCheck - это библиотека property-based testing для C++, вдохновленная QuickCheck. Она позволяет:
- Генерировать случайные тестовые данные
- Автоматически находить минимальные failing примеры
- Запускать тесты множество раз с разными данными

**Unit Testing:** Будет использоваться **Qt Test Framework**

Qt Test - встроенный фреймворк тестирования Qt, который предоставляет:
- Макросы для assertions (QCOMPARE, QVERIFY)
- Поддержку тестирования GUI компонентов
- Интеграцию с Qt сигналами и слотами

### Конфигурация тестов

Каждый property-based тест должен быть настроен на минимум **100 итераций** для обеспечения достаточного покрытия входного пространства.

```cpp
// Пример конфигурации RapidCheck теста
rc::check("Property name", [](/* parameters */) {
    // Test logic
});
// По умолчанию RapidCheck выполняет 100 итераций
```

### Тегирование тестов

Каждый property-based тест ДОЛЖЕН быть помечен комментарием, явно ссылающимся на correctness property из design документа:

```cpp
// **Feature: car-salon-db, Property 9: Персистентность добавления записи**
TEST(Operations, AddCarPersistence) {
    rc::check("Added car persists in database", []() {
        // Generate random valid car data
        auto brand = *rc::gen::nonEmpty<std::string>();
        auto manufacturer = *rc::gen::nonEmpty<std::string>();
        // ... test implementation
    });
}
```

### Покрытие тестами

**Property-based тесты должны покрывать:**

1. **Инвариантные свойства:**
   - Property 1: Идемпотентность инициализации
   - Property 2: Полнота отображения данных
   - Property 9: Персистентность добавления

2. **Round-trip свойства:**
   - Property 5: Round-trip фильтрации
   - Property 8: Round-trip экспорта и чтения

3. **Валидационные свойства:**
   - Property 4: Валидация нечисловых значений
   - Property 10: Отклонение пустых полей
   - Property 11: Валидация типов данных

4. **Операционные свойства:**
   - Property 3: Корректность фильтрации
   - Property 6: Полнота экспорта
   - Property 13: Полнота удаления
   - Property 15: Полнота обновления

5. **Производительность:**
   - Property 18: Производительность операций

**Unit тесты должны покрывать:**

1. **Конкретные примеры:**
   - Добавление конкретного автомобиля
   - Удаление конкретного производителя
   - Обновление цены конкретной марки

2. **Граничные случаи (edge cases):**
   - Пустая база данных (2.4)
   - Нет результатов поиска (3.3)
   - Несуществующий файл (5.2)
   - Нет записей для удаления (7.2)
   - Нет записей для обновления (8.3)

3. **Обработка ошибок:**
   - Ошибка подключения к БД (1.3)
   - Ошибка создания файла (4.3)
   - Ошибка вставки в БД (6.5)
   - Ошибка удаления (7.4)
   - Ошибка обновления (8.5)

4. **Интеграционные тесты:**
   - Полный цикл: добавление → просмотр → экспорт → чтение
   - Последовательность операций: добавление → обновление → удаление

### Генераторы для Property-Based Testing

Для эффективного property-based тестирования необходимо создать генераторы валидных данных:

```cpp
// Генератор валидных марок автомобилей
auto genBrand = rc::gen::nonEmpty(rc::gen::string<std::string>());

// Генератор валидных производителей
auto genManufacturer = rc::gen::nonEmpty(rc::gen::string<std::string>());

// Генератор валидных дат (последние 50 лет)
auto genReleaseDate = rc::gen::inRange(1974, 2024).as("year")
    .then([](int year) {
        return rc::gen::construct<QDate>(
            rc::gen::just(year),
            rc::gen::inRange(1, 12),
            rc::gen::inRange(1, 28)
        );
    });

// Генератор валидного пробега (0 - 500000 км)
auto genMileage = rc::gen::inRange(0, 500000);

// Генератор валидной цены (100000 - 10000000 рублей)
auto genPrice = rc::gen::inRange(100000, 10000000);

// Генератор полной записи об автомобиле
auto genCar = rc::gen::construct<CarData>(
    genBrand,
    genManufacturer,
    genReleaseDate,
    genMileage,
    genPrice
);
```

### Стратегия выполнения тестов

1. **Во время разработки:**
   - Запуск unit тестов после каждого изменения
   - Запуск property тестов для измененных компонентов

2. **Перед коммитом:**
   - Запуск всех unit тестов
   - Запуск всех property тестов

3. **В CI/CD:**
   - Автоматический запуск всех тестов
   - Проверка покрытия кода
   - Fail build при падении тестов

### Критерии успешности тестирования

- Все unit тесты проходят (100%)
- Все property тесты проходят на 100 итерациях
- Покрытие кода >= 80% для критических компонентов (Database, Operations)
- Нет известных багов в основном функционале


## Технолстек

### Основные технологии

| Компонент | Технология | Версия | Назначение |
|-----------|-----------|--------|------------|
| Язык программирования | C++ | 17 | Основной язык разработки |
| GUI Framework | Qt | 5.x | Графический интерфейс |
| База данных | PostgreSQL | 15 | Хранение данных |
| Контейнеризация | Docker | latest | Изоляция и развертывание |
| Оркестрация | Docker Compose | 3.9 | Управление контейнерами |
| VNC Server | TigerVNC | latest | Удаленный доступ к GUI |
| Web VNC | noVNC | latest | Доступ к GUI через браузер |
| Build System | CMake | 3.16+ | Сборка проекта |
| Testi | Qt Test | 5.x | Unit тестирование |
| Testing (Property) | RapidCheck | latest | Property-based тестирование |

### Зависимости Qt модулей

```cmake
find_package(Qt5 REQUIRED ENTS
    Core        # Базовая функциональность Qt
    Sql         # Работа с базами данных
    Widgets     # GUI компоненты
    Test        # Тестирование
)
```

### Зависимости PostgreSQL

```cmake
# PostgreSQL клиентская библиотека
find_package(PostgreSQL REQUIRED)

# Qt PostgreSQL драйвер (QPSQL)
# Устанавливается через пакетный менеджер
```

## Структура проекта

```
autosalon/
├── app/
│   ├── Docke                 # Конфигурация Docker контейнера
│   ├── CMakeLists.txt            # Конфигурация сборки
│   ├── src/
│   │   ├── main.cpp              # Точка входа приложения
│   │   ├── database.h            # Заголовок класса Database
│   │   ├── database.cpp          # Реализация класса Database
│   │   ├── operations.h          # Заголовок класса Operations
│   │   ├── operations.cpp        # Реализация класса Operations
│   │   └── gui/
│   │       ├── mainwindow.h      # Заголовок класса MainWindow
│   │       └── mainwindow.cpp    # Реализация класса MainWindow
│   ├── tests/
│   │   ├── CMakeLists.txt        # Конфигурация тестов
│   │   ├── test_database.cpp     # Тесты класса Database
│   │   ├── test_operations.cpp   # Тесты класса Operations
│   │   └── test_properties.cpp   # Property-based тесты
│   └── build/                    # Директория сборки (создается)
├── docker-compose.yml            # Конфигурация Docker Compose
├── README.md                     # Документация проекта
└── .kiro/
    └── specs/
        └── car-salon-db/
            ├── requirements.md   # Документ требований
            ├── design.md         # Документ проектирования (этот файл)
            └── tasks.md          # План реализации (будет создан)
```

## Конфигурация Docker

### Dockerfile для приложения

```dockerfile
FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get insta
    build-essential \
    
    qt5-default \
    libqt5sql5-psql \
    postgresql-client \
    tigervnc-standalone-server \
    novnc \
    supervisor \
    && rm -rf /var/lib/apt/lists/*

# Настройка VNC
ENV DISPLAY=:1
RUN mkdir -p ~/.vnc && \
    echo "password" | vncpasswd -f > ~/.vnc/passwd && \
    chmod 600 ~/.vnc/passwd

# Копирование исходного кода
WORKDIR /app
COPY . /app

# Сборка приложения
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# Настройка supervisor для запуска VNC и noVNC
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

EXPOSE 5900 6080

CMD ["/usr/bin/supervisord"]
```

### docker-compose.yml

```yaml
version: "3.9"

services:
  app:
    build: ./app
    container_name: autosalon_app
    ports:
      - "6080:80"     # noVNC (GUI в браузере)
      - "5900:5900"   # VNC
    depends_on:
      - db
    environment:
      - DB_HOST=db
      - DB_PORT=5432
      - DB_NAME=autosalon
      - DB_USER=user
      - DB_PASSWORD=pass

  db:
    image: postgres:15
    container_name: autosalon_db
    environment:
      POSTGRES_USER: user
      POSTGRES_PASSWORD: pass
      POSTGRES_DB: autosalon
    ports:
      - "5432:5432"
    volumes:
      - pgdata:/var/lib/postgresql/data

volumes:
  pgdata:
```

## Безопасность

### Защита от SQL Injection

Все SQL запросы используют prepared statements:

```cpp
QSqlQuery query;
query.prepare("INSERT INTO cars (brand, manufacturer, release_date, mileage, price) "
              "VALUES (:brand, :manufacturer, :release_date, :mileage, :price)");
query.bindValue(":brand", brand);
query.bindValue(":manufacturer", manufacturer);
query.bindValue(":release_date", releaseDate);
query.bindValue(":mileage", mileage);
query.bindValue(":price", price);
query.exec();
```

### Валидация входных данных

Многоуровневая валидация:
1. UI уровень - проверка формата
2. Business logic уровень - проверка бизнес-правил
3. Database уровень - constraints в схеме БД

### Управление паролями

- Пароли БД хранятся в переменных окружения
- Не хардкодятся в исходном коде
- Передаются через docker-compose.yml

### Права доступа к файлам

- Экспортированные файлы создаются с правами 644
- Проверка прав доступа перед чтением файлов
- Обработка ошибок доступа

## Производительность

### Оптимизация запросов к БД

1. **Индексы:**
   ```sql
   CREATE INDEX idx_cars_brand ON cars(brand);
   CREATE INDEX idx_cars_manufacturer ON cars(manufacturer);
   CREATE INDEX idx_cars_price ON cars(price);
   ```

2. **Пакетные операции:**
   - Использование транзакций для множественных вставок
   - Batch updates для массовых изменений

3. **Connection pooling:**
   - Переиспользование соединения с БД
   - Один экземпляр QSqlDatabase на приложение

### Оптимизация UI

1. **Ленивая загрузка:**
   - QSqlTableModel загружает данные по требованию
   - Пагинация для больших наборов данных

2. **Кэширование:**
   - Кэширование результатов фильтрации
   - Минимизация перерисовки UI

3. **Асинхронные операции:**
   - Длительные операции (экспорт) выполняются в отдельном потоке
   - QProgressDialog для индикации прогресса

### Целевые метрики производительности

| Операция | Целевое время | Максимальное время |
|----------|---------------|-------------------|
| Запуск приложения | 3 сек | 5 сек |
| Загрузка таблицы (1000 записей) | 0.5 сек | 1 сек |
| Добавление записи | 0.1 сек | 0.5 сек |
| Удаление записей | 0.5 сек | 2 сек |
| Обновление цен | 0.5 сек | 2 сек |
| Фильтрация | 0.2 сек | 1 сек |
| Экспорт (1000 записей) | 1 сек | 3 сек |

## Масштабируемость

### Текущие ограничения

- Однопользовательское приложение
- Локальное подключение к БД
- Ограничено ресурсами одного контейнера

### Возможности расширения

1. **Многопользовательский режим:**
   - Добавление аутентификации
   - Управление сессиями
   - Разграничение прав доступа

2. **Распределенная архитектура:**
   - Разделение на frontend и backend
   - REST API для взаимодействия
   - Микросервисная архитектура

3. **Дополнительный функционал:**
   - Импорт данных из файлов
   - Расширенная фильтрация и сортировка
   - Отчеты и аналитика
   - История изменений (audit log)

## Развертывание и эксплуатация

### Процесс развертывания

1. **Клонирование репозитория:**
   ```bash
   git clone <repository-url>
   cd autosalon
   ```

2. **Запуск через Docker Compose:**
   ```bash
   docker-compose up -d --build
   ```

3. **Доступ к приложению:**
   - Открыть браузер: `http://localhost:6080`
   - Или VNC клиент: `localhost:5900`

### Мониторинг

1. **Логи приложения:**
   ```bash
   docker logs autosalon_app
   ```

2. **Логи базы данных:**
   ```bash
   docker logs autosalon_db
   ```

3. **Состояние контейнеров:**
   ```bash
   docker-compose ps
   ```

### Резервное копирование

1. **Backup базы данных:**
   ```bash
   docker exec autosalon_db pg_dump -U user autosalon > backup.sql
   ```

2. **Restore базы данных:**
   ```bash
   docker exec -i autosalon_db psql -U user autosalon < backup.sql
   ```

3. **Backup Docker volume:**
   ```bash
   docker run --rm -v autosalon_pgdata:/data -v $(pwd):/backup \
     ubuntu tar czf /backup/pgdata-backup.tar.gz /data
   ```

### Обновление

1. **Обновление кода:**
   ```bash
   git pull
   docker-compose down
   docker-compose up -d --build
   ```

2. **Миграция базы данных:**
   - Создание скриптов миграции
   - Применение через psql

## Заключение

Данный документ проектирования описывает архитектуру, компоненты, модели данных и стратегию тестирования системы управления базой данных автосалона. Система построена на принципах объектно-ориентированного программирования с четким разделением ответственности между слоями.

Ключевые аспекты дизайна:
- Трехслойная архитектура (Presentation, Business Logic, Data Access)
- Использование Qt для кроссплатформенного GUI
- PostgreSQL для надежного хранения данных
- Docker для изоляции и упрощения развертывания
- Property-based testing для обеспечения корректности
- Comprehensive error handling для надежности

Система спроектирована с учетом требований к производительности, надежности и удобству использования, и готова к реализации согласно плану задач.
