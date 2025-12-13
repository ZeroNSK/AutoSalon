#include "mainwindow.h"
#include "../operations.h"
#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include <QFormLayout>
#include <QSqlDatabase>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QSqlQueryModel>
#include <QTextEdit>
#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    
    // Настройка модели данных
    model = new QSqlTableModel(this);
    model->setTable("cars");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // Изменения только через код или кнопки
    model->select();
    
    // Установка заголовков
    // Примечание: brand = производитель (Tesla, BMW), manufacturer = модель (Model 3, X5)
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Производитель"); // brand -> Производитель
    model->setHeaderData(2, Qt::Horizontal, "Модель");        // manufacturer -> Модель
    model->setHeaderData(3, Qt::Horizontal, "Дата выпуска");
    model->setHeaderData(4, Qt::Horizontal, "Пробег");
    model->setHeaderData(5, Qt::Horizontal, "Цена");

    tableView->setModel(model);
    tableView->hideColumn(0); // Скрываем ID
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Таблица
    tableView = new QTableView(this);
    
    // ОТКЛЮЧЕНИЕ РЕДАКТИРОВАНИЯ В ТАБЛИЦЕ
    // Запрещаем любое редактирование ячеек напрямую в таблице
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Включаем выбор строк для удаления
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    mainLayout->addWidget(tableView);
    
    // Кнопки управления
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    
    btnAdd = new QPushButton("Добавить автомобиль", this);
    btnAdd->setToolTip("Добавить новую запись об автомобиле в базу данных");
    
    btnDelete = new QPushButton("Удалить по модели", this);
    btnDelete->setToolTip("Удалить все автомобили указанной модели");
    
    btnDeleteSelected = new QPushButton("Удалить выбранный", this);
    btnDeleteSelected->setToolTip("Удалить выбранный автомобиль из таблицы");
    
    btnUpdate = new QPushButton("Изменить цену", this);
    btnUpdate->setToolTip("Обновить цену для всех автомобилей указанной марки");
    
    btnFilter = new QPushButton("Фильтр по цене", this);
    btnFilter->setToolTip("Показать автомобили с ценой не выше указанной");
    
    btnExport = new QPushButton("Сформировать файл", this);
    btnExport->setToolTip("Экспортировать данные об автомобилях в текстовый файл");
    
    btnShowFile = new QPushButton("Показать файл", this);
    btnShowFile->setToolTip("Показать содержимое экспортированного файла");
    
    btnRefresh = new QPushButton("Обновить таблицу", this);
    btnRefresh->setToolTip("Перезагрузить данные из базы данных и сбросить фильтры");
    
    buttonsLayout->addWidget(btnAdd);
    buttonsLayout->addWidget(btnDeleteSelected);
    buttonsLayout->addWidget(btnDelete);
    buttonsLayout->addWidget(btnUpdate);
    buttonsLayout->addWidget(btnFilter);
    buttonsLayout->addWidget(btnExport);
    buttonsLayout->addWidget(btnShowFile);
    buttonsLayout->addWidget(btnRefresh);
    
    mainLayout->addLayout(buttonsLayout);
    
    // Подключение сигналов
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddCar);
    connect(btnDeleteSelected, &QPushButton::clicked, this, &MainWindow::onDeleteSelected);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteByManufacturer);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindow::onUpdatePrice);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::onFilterByPrice);
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportToFile);
    connect(btnShowFile, &QPushButton::clicked, this, &MainWindow::onShowFile);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshTable);
    
    setWindowTitle("Автосалон - Курсовая работа");
    
    // Установка минимального и начального размера окна
    setMinimumSize(1200, 700);
    resize(1400, 800);
    
    // Максимизация окна для лучшей видимости
    showMaximized();
    
    // Простой, но аккуратный стиль (как будто студент старался)
    QString style = R"(
        /* Общий фон окна */
        QMainWindow {
            background-color: #f5f5f5;
        }
        
        /* Таблица */
        QTableView {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 5px;
            gridline-color: #e0e0e0;
            font-size: 13px;
        }
        
        /* Заголовки таблицы */
        QHeaderView::section {
            background-color: #4a90e2;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
            font-size: 13px;
        }
        
        /* Чередующиеся строки */
        QTableView::item:alternate {
            background-color: #f9f9f9;
        }
        
        /* Выделенная строка */
        QTableView::item:selected {
            background-color: #b3d9ff;
            color: black;
        }
        
        /* Кнопки */
        QPushButton {
            background-color: #4a90e2;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 8px 15px;
            font-size: 13px;
            min-width: 120px;
        }
        
        QPushButton:hover {
            background-color: #357abd;
        }
        
        QPushButton:pressed {
            background-color: #2868a8;
        }
        
        /* Кнопка удаления - красная */
        QPushButton#btnDeleteSelected, QPushButton#btnDelete {
            background-color: #e74c3c;
        }
        
        QPushButton#btnDeleteSelected:hover, QPushButton#btnDelete:hover {
            background-color: #c0392b;
        }
        
        /* Кнопка обновления - зеленая */
        QPushButton#btnRefresh {
            background-color: #27ae60;
        }
        
        QPushButton#btnRefresh:hover {
            background-color: #229954;
        }
    )";
    
    setStyleSheet(style);
    
    // Установка имен для кнопок (для стилей)
    btnDeleteSelected->setObjectName("btnDeleteSelected");
    btnDelete->setObjectName("btnDelete");
    btnRefresh->setObjectName("btnRefresh");
    
    // Включить чередующиеся строки
    tableView->setAlternatingRowColors(true);
}

void MainWindow::onRefreshTable() {
    // Сброс всех фильтров (Requirements 2.2, 3.4)
    model->setFilter("");
    
    // Перезагрузка данных из базы данных (Requirements 2.2)
    model->select();
    
    // Обновление отображения
    tableView->reset();
}

void MainWindow::onAddCar() {
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить автомобиль");
    
    QFormLayout form(&dialog);
    
    QLineEdit *brandEdit = new QLineEdit(&dialog);
    QLineEdit *manufEdit = new QLineEdit(&dialog);
    QLineEdit *dateEdit = new QLineEdit(&dialog);
    dateEdit->setPlaceholderText("ДД.ММ.ГГГГ (например, 15.01.2020)");
    QLineEdit *mileageEdit = new QLineEdit(&dialog);
    QLineEdit *priceEdit = new QLineEdit(&dialog);
    
    form.addRow("Производитель:", brandEdit);  // brand -> Производитель (Tesla, BMW)
    form.addRow("Модель:", manufEdit);          // manufacturer -> Модель (Model 3, X5)
    form.addRow("Дата выпуска:", dateEdit);
    form.addRow("Пробег (км):", mileageEdit);
    form.addRow("Цена (руб):", priceEdit);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString brand = brandEdit->text().trimmed();
        QString manuf = manufEdit->text().trimmed();
        QString dateStr = dateEdit->text().trimmed();
        QString mileageStr = mileageEdit->text().trimmed();
        QString priceStr = priceEdit->text().trimmed();
        
        // Проверка пустых полей (Requirements 6.2)
        if (brand.isEmpty() || manuf.isEmpty() || dateStr.isEmpty() || 
            mileageStr.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
        // Проверка формата даты (Requirements 6.3)
        // Поддержка формата ДД.ММ.ГГГГ (день.месяц.год)
        QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
        if (!date.isValid()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Дата должна быть в формате ДД.ММ.ГГГГ (например, 15.01.2020).");
            return;
        }
        
        // Проверка года (не позже 2025)
        if (date.year() > 2025) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Год выпуска не может быть позже 2025.");
            return;
        }
        
        // Проверка, что дата не в будущем
        if (date > QDate::currentDate()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Дата выпуска не может быть в будущем.");
            return;
        }
        
        // Проверка числовых значений (Requirements 6.3)
        bool mileageOk, priceOk;
        int mileage = mileageStr.toInt(&mileageOk);
        int price = priceStr.toInt(&priceOk);
        
        if (!mileageOk || !priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Пробег и цена должны быть числами.");
            return;
        }
        
        // Добавление записи (Requirements 6.1)
        if (Operations::addCar(brand, manuf, date, mileage, price)) {
            QMessageBox::information(this, "Успех", 
                QString("Автомобиль '%1' успешно добавлен в базу данных.").arg(brand));
            onRefreshTable(); // Requirements 6.4
        } else {
            QMessageBox::critical(this, "Ошибка", 
                "Не удалось добавить запись в базу данных. Проверьте подключение.");
        }
    }
}

void MainWindow::onDeleteSelected() {
    // Получить выбранную строку
    QModelIndexList selection = tableView->selectionModel()->selectedRows();
    
    if (selection.isEmpty()) {
        QMessageBox::information(this, "Информация", 
            "Выберите автомобиль в таблице для удаления.");
        return;
    }
    
    // Получить данные выбранной строки
    int row = selection.at(0).row();
    QString brand = model->data(model->index(row, 1)).toString();
    QString manufacturer = model->data(model->index(row, 2)).toString();
    int id = model->data(model->index(row, 0)).toInt();
    
    // Диалог подтверждения
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления", 
        QString("Вы уверены, что хотите удалить автомобиль:\n"
                "Производитель: %1\n"
                "Модель: %2\n\n"
                "Это действие нельзя отменить.").arg(brand).arg(manufacturer),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Удаление записи по ID
        QSqlQuery query;
        query.prepare("DELETE FROM cars WHERE id = :id");
        query.bindValue(":id", id);
        
        if (query.exec()) {
            QMessageBox::information(this, "Успех", 
                "Автомобиль успешно удален.");
            onRefreshTable();
        } else {
            QMessageBox::critical(this, "Ошибка", 
                QString("Не удалось удалить запись:\n%1").arg(query.lastError().text()));
        }
    }
}

void MainWindow::onDeleteByManufacturer() {
    bool ok;
    QString manuf = QInputDialog::getText(this, "Удаление по модели", 
                                         "Введите название модели:", 
                                         QLineEdit::Normal, "", &ok);
    
    if (ok && !manuf.trimmed().isEmpty()) {
        manuf = manuf.trimmed();
        
        // Диалог подтверждения (Requirements 7.1)
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение удаления", 
            QString("Вы уверены, что хотите удалить все автомобили модели '%1'?\n"
                    "Это действие нельзя отменить.").arg(manuf),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            // Выполнение удаления (Requirements 7.1)
            int deletedCount = Operations::deleteByManufacturer(manuf);
            
            if (deletedCount > 0) {
                // Успешное удаление (Requirements 7.3)
                QMessageBox::information(this, "Успех", 
                    QString("Удалено записей: %1").arg(deletedCount));
                onRefreshTable();
            } else if (deletedCount == 0) {
                // Нет записей для удаления (Requirements 7.2)
                QMessageBox::information(this, "Информация", 
                    QString("Не найдено автомобилей модели '%1'. "
                            "Записи не были удалены.").arg(manuf));
            } else {
                // Ошибка удаления
                QMessageBox::critical(this, "Ошибка", 
                    "Не удалось выполнить операцию удаления. Проверьте подключение к базе данных.");
            }
        }
    }
}

void MainWindow::onUpdatePrice() {
    QDialog dialog(this);
    dialog.setWindowTitle("Обновить цену по марке");
    QFormLayout form(&dialog);
    
    QLineEdit *brandEdit = new QLineEdit(&dialog);
    QLineEdit *priceEdit = new QLineEdit(&dialog);
    
    form.addRow("Производитель:", brandEdit);  // brand -> Производитель
    form.addRow("Новая цена (руб):", priceEdit);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString brand = brandEdit->text().trimmed();
        QString priceStr = priceEdit->text().trimmed();
        
        // Проверка пустых полей
        if (brand.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
        // Проверка числового значения (Requirements 8.2)
        bool priceOk;
        int price = priceStr.toInt(&priceOk);
        
        if (!priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Цена должна быть числом.");
            return;
        }
        
        // Проверка положительной цены (Requirements 8.2)
        if (price <= 0) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Цена должна быть положительным числом.");
            return;
        }
        
        // Выполнение обновления (Requirements 8.1)
        int updatedCount = Operations::updatePriceByBrand(brand, price);
        
        if (updatedCount > 0) {
            // Успешное обновление (Requirements 8.4)
            QMessageBox::information(this, "Успех", 
                QString("Обновлено записей: %1\nНовая цена для марки '%2': %3 руб.")
                    .arg(updatedCount).arg(brand).arg(price));
            onRefreshTable();
        } else if (updatedCount == 0) {
            // Нет записей для обновления (Requirements 8.3)
            QMessageBox::information(this, "Информация", 
                QString("Не найдено автомобилей марки '%1'. "
                        "Записи не были обновлены.").arg(brand));
        } else {
            // Ошибка обновления
            QMessageBox::critical(this, "Ошибка", 
                "Не удалось выполнить операцию обновления. Проверьте подключение к базе данных.");
        }
    }
}

void MainWindow::onFilterByPrice() {
    bool ok;
    int maxPrice = QInputDialog::getInt(this, "Фильтр по цене", 
        "Введите максимальную цену (руб):", 
        1000000, 0, 2147483647, 100000, &ok);
    
    if (ok) {
        // Применение фильтра к модели (Requirements 3.1)
        QString filter = QString("price <= %1").arg(maxPrice);
        model->setFilter(filter);
        model->select();
        
        // Проверка наличия результатов (Requirements 3.3)
        if (model->rowCount() == 0) {
            QMessageBox::information(this, "Результаты фильтрации", 
                QString("Не найдено автомобилей с ценой <= %1 руб.").arg(maxPrice));
        } else {
            QMessageBox::information(this, "Результаты фильтрации", 
                QString("Найдено автомобилей: %1\n"
                        "Показаны автомобили с ценой <= %2 руб.\n\n"
                        "Для сброса фильтра нажмите 'Обновить таблицу'.")
                    .arg(model->rowCount()).arg(maxPrice));
        }
    }
}

void MainWindow::onExportToFile() {
    // Диалог выбора имени файла (Requirements 4.1)
    QString filename = QFileDialog::getSaveFileName(this, 
        "Экспорт данных в файл", 
        "export.txt",
        "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!filename.isEmpty()) {
        // Выполнение экспорта (Requirements 4.1)
        if (Operations::exportToFile(filename)) {
            QFileInfo fileInfo(filename);
            // Сообщение об успехе с путем к файлу (Requirements 4.4)
            QMessageBox::information(this, "Успех", 
                QString("Данные успешно экспортированы в файл:\n%1\n\n"
                        "Расположение: %2")
                    .arg(fileInfo.fileName())
                    .arg(fileInfo.absoluteFilePath()));
        } else {
            // Обработка ошибок записи (Requirements 4.3)
            QMessageBox::critical(this, "Ошибка экспорта", 
                QString("Не удалось создать или записать файл:\n%1\n\n"
                        "Проверьте права доступа и наличие свободного места на диске.")
                    .arg(filename));
        }
    }
}

void MainWindow::onShowFile() {
    // Диалог выбора файла (Requirements 5.1)
    QString filename = QFileDialog::getOpenFileName(this, 
        "Выбрать файл для просмотра", 
        "",
        "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!filename.isEmpty()) {
        // Чтение содержимого файла (Requirements 5.1)
        QString content = Operations::readFileContent(filename);
        
        if (content.isEmpty()) {
            // Обработка ошибок чтения (Requirements 5.2, 5.3)
            QFileInfo fileInfo(filename);
            if (!fileInfo.exists()) {
                QMessageBox::warning(this, "Ошибка чтения", 
                    QString("Файл не найден:\n%1").arg(filename));
            } else if (!fileInfo.isReadable()) {
                QMessageBox::critical(this, "Ошибка чтения", 
                    QString("Не удалось прочитать файл:\n%1\n\n"
                            "Проверьте права доступа к файлу.").arg(filename));
            } else {
                QMessageBox::information(this, "Информация", 
                    "Файл пустой или не содержит данных.");
            }
        } else {
            // Показ содержимого файла (Requirements 5.1, 5.4)
            QDialog dialog(this);
            dialog.setWindowTitle(QString("Содержимое файла: %1").arg(QFileInfo(filename).fileName()));
            dialog.resize(600, 400);
            
            QVBoxLayout *layout = new QVBoxLayout(&dialog);
            QTextEdit *textEdit = new QTextEdit(&dialog);
            textEdit->setReadOnly(true);
            textEdit->setText(content);
            textEdit->setFont(QFont("Courier", 10));
            
            layout->addWidget(textEdit);
            
            QPushButton *closeBtn = new QPushButton("Закрыть", &dialog);
            connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
            layout->addWidget(closeBtn);
            
            dialog.exec();
        }
    }
}
