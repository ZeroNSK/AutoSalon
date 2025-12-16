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
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), selectedRow(-1) {
    setupUi();
    
    // Настройка модели данных
    model = new QSqlTableModel(this);
    model->setTable("cars");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // Изменения только через код или кнопки
    model->select();
    
    // Установка заголовков
    // Примечание: brand = производитель (Tesla, BMW), manufacturer = модель (Model 3, X5)
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Производитель"); 
    model->setHeaderData(2, Qt::Horizontal, "Модель");       
    model->setHeaderData(3, Qt::Horizontal, "Дата выпуска");
    model->setHeaderData(4, Qt::Horizontal, "Пробег");
    model->setHeaderData(5, Qt::Horizontal, "Цена");

    tableView->setModel(model);
    tableView->hideColumn(0); // Скрываем ID
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Подключение сигнала клика по строке
    connect(tableView, &QTableView::clicked, this, &MainWindow::onTableRowClicked);
    
    setupTableHeader();
    setupContextMenu();
    updateSelectedRowButtons();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    // Кнопка добавления 
    btnAdd = new QPushButton("+ Добавить автомобиль", this);
    btnAdd->setToolTip("Добавить новую запись об автомобиле в базу данных");
    btnAdd->setMinimumHeight(45);
    btnAdd->setMinimumWidth(200);
    
    topLayout->addWidget(btnAdd);
    topLayout->addStretch(); // Растягиваем пространство
    
    // Кнопки управления данными
    btnFilter = new QPushButton("Фильтр по цене", this);
    btnFilter->setToolTip("Показать автомобили с ценой не выше указанной");
    btnFilter->setMinimumHeight(45);
    btnFilter->setMinimumWidth(160);
    
    btnRefresh = new QPushButton("Обновить", this);
    btnRefresh->setToolTip("Перезагрузить данные из базы данных и сбросить фильтры");
    btnRefresh->setMinimumHeight(45);
    btnRefresh->setMinimumWidth(130);
    
    topLayout->addWidget(btnFilter);
    topLayout->addWidget(btnRefresh);
    
    mainLayout->addLayout(topLayout);
    
    QVBoxLayout *tableContainer = new QVBoxLayout();
    
    // Заголовок таблицы с кнопкой сортировки
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout();
    
    QLabel *tableTitle = new QLabel("Список автомобилей", this);
    tableTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
    
    // Кнопка сортировки
    btnSort = new QToolButton(this);
    btnSort->setText("Сортировка");
    btnSort->setToolTip("Выберите способ сортировки данных");
    btnSort->setPopupMode(QToolButton::InstantPopup);
    btnSort->setMinimumHeight(35);
    btnSort->setMinimumWidth(120);
    
    // Создание меню сортировки
    sortMenu = new QMenu(this);
    actionSortByPrice = sortMenu->addAction("По цене (возрастание)");
    actionSortByBrand = sortMenu->addAction("По производителю (А-Я)");
    actionSortByDate = sortMenu->addAction("По дате выпуска");
    actionSortByMileage = sortMenu->addAction("По пробегу");
    sortMenu->addSeparator();
    actionResetSort = sortMenu->addAction("Сбросить сортировку");
    
    btnSort->setMenu(sortMenu);
    
    tableHeaderLayout->addWidget(tableTitle);
    tableHeaderLayout->addStretch();
    tableHeaderLayout->addWidget(btnSort);
    
    tableContainer->addLayout(tableHeaderLayout);
    
    // Таблица
    tableView = new QTableView(this);
    
    // ОТКЛЮЧЕНИЕ РЕДАКТИРОВАНИЯ В ТАБЛИЦЕ
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Включаем выбор строк
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Включить чередующиеся строки
    tableView->setAlternatingRowColors(true);
    
    tableContainer->addWidget(tableView);
    mainLayout->addLayout(tableContainer);
    
    //ПАНЕЛЬ ДЛЯ ВЫБРАННОЙ СТРОКИ 
    selectedRowFrame = new QFrame(this);
    selectedRowFrame->setFrameStyle(QFrame::StyledPanel);
    selectedRowFrame->setVisible(false);
    
    QHBoxLayout *selectedLayout = new QHBoxLayout(selectedRowFrame);
    
    selectedRowLabel = new QLabel("Выбран автомобиль:", this);
    selectedRowLabel->setStyleSheet("font-weight: bold; color: #2980b9;");
    
    btnEditSelected = new QPushButton("Редактировать", this);
    btnEditSelected->setToolTip("Редактировать выбранный автомобиль");
    btnEditSelected->setMinimumHeight(40);
    
    btnDeleteSelected = new QPushButton("Удалить", this);
    btnDeleteSelected->setToolTip("Удалить выбранный автомобиль из таблицы");
    btnDeleteSelected->setMinimumHeight(40);
    
    selectedLayout->addWidget(selectedRowLabel);
    selectedLayout->addStretch();
    selectedLayout->addWidget(btnEditSelected);
    selectedLayout->addWidget(btnDeleteSelected);
    
    mainLayout->addWidget(selectedRowFrame);
    
    //ПАНЕЛЬ С МАССОВЫМИ ОПЕРАЦИЯМИ
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    
    // Массовые операции
    QLabel *massOpsLabel = new QLabel("Массовые операции:", this);
    massOpsLabel->setStyleSheet("font-weight: bold; color: #7f8c8d;");
    
    btnDelete = new QPushButton("Удалить по модели", this);
    btnDelete->setToolTip("Удалить все автомобили указанной модели");
    btnDelete->setMinimumHeight(40);
    btnDelete->setMinimumWidth(180);
    
    btnUpdate = new QPushButton("Изменить цену по марке", this);
    btnUpdate->setToolTip("Обновить цену для всех автомобилей указанной марки");
    btnUpdate->setMinimumHeight(40);
    btnUpdate->setMinimumWidth(200);
    
    bottomLayout->addWidget(massOpsLabel);
    bottomLayout->addWidget(btnDelete);
    bottomLayout->addWidget(btnUpdate);
    bottomLayout->addStretch();
    
    // Операции с файлами
    btnExport = new QPushButton("Экспорт в файл", this);
    btnExport->setToolTip("Экспортировать данные об автомобилях в текстовый файл");
    btnExport->setMinimumHeight(40);
    btnExport->setMinimumWidth(150);
    
    btnShowFile = new QPushButton("Показать файл", this);
    btnShowFile->setToolTip("Показать содержимое экспортированного файла");
    btnShowFile->setMinimumHeight(40);
    btnShowFile->setMinimumWidth(150);
    
    bottomLayout->addWidget(btnExport);
    bottomLayout->addWidget(btnShowFile);
    
    mainLayout->addLayout(bottomLayout);
    
    //ПОДКЛЮЧЕНИЕ СИГНАЛОВ 
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddCar);
    connect(btnEditSelected, &QPushButton::clicked, this, &MainWindow::onEditSelected);
    connect(btnDeleteSelected, &QPushButton::clicked, this, &MainWindow::onDeleteSelected);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteByManufacturer);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindow::onUpdatePrice);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::onFilterByPrice);
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportToFile);
    connect(btnShowFile, &QPushButton::clicked, this, &MainWindow::onShowFile);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshTable);
    
    // Подключение меню сортировки
    connect(actionSortByPrice, &QAction::triggered, this, &MainWindow::onSortByPrice);
    connect(actionSortByBrand, &QAction::triggered, this, &MainWindow::onSortByBrand);
    connect(actionSortByDate, &QAction::triggered, this, &MainWindow::onSortByDate);
    connect(actionSortByMileage, &QAction::triggered, this, &MainWindow::onSortByMileage);
    connect(actionResetSort, &QAction::triggered, this, &MainWindow::onResetSort);
    
    setWindowTitle("Автосалон - Система управления");
    
    // Установка размеров окна для VNC
    setMinimumSize(1200, 700);
    resize(1400, 850);
    // Не используем showMaximized() в VNC - устанавливаем фиксированный размер
    setFixedSize(1400, 850);
    
    //УЛУЧШЕННЫЕ СТИЛИ
    QString style = R"(
        /* Общий фон */
        QMainWindow {
            background-color: #f5f5f5;
        }
        
        /* Таблица */
        QTableView {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 5px;
            gridline-color: #e0e0e0;
            font-size: 14px;
            selection-background-color: #e3f2fd;
        }
        
        /* Заголовки таблицы */
        QHeaderView::section {
            background-color: #4a90e2;
            color: white;
            padding: 12px 8px;
            border: none;
            font-weight: bold;
            font-size: 14px;
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
        
        /* Основные кнопки - УВЕЛИЧЕННЫЕ */
        QPushButton {
            background-color: #4a90e2;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 15px 25px;
            font-size: 15px;
            font-weight: bold;
            min-width: 150px;
            min-height: 45px;
        }
        
        QPushButton:hover {
            background-color: #357abd;
        }
        
        QPushButton:pressed {
            background-color: #2868a8;
        }
        
        /* Кнопка добавления - зеленая */
        QPushButton#btnAdd {
            background-color: #27ae60;
            font-size: 16px;
        }
        
        QPushButton#btnAdd:hover {
            background-color: #229954;
        }
        
        /* Кнопки удаления - красные */
        QPushButton#btnDeleteSelected, QPushButton#btnDelete {
            background-color: #e74c3c;
        }
        
        QPushButton#btnDeleteSelected:hover, QPushButton#btnDelete:hover {
            background-color: #c0392b;
        }
        
        /* Кнопка обновления - оранжевая */
        QPushButton#btnRefresh {
            background-color: #f39c12;
        }
        
        QPushButton#btnRefresh:hover {
            background-color: #e67e22;
        }
        
        /* Кнопка редактирования - фиолетовая */
        QPushButton#btnEditSelected {
            background-color: #9b59b6;
        }
        
        QPushButton#btnEditSelected:hover {
            background-color: #8e44ad;
        }
        
        /* Кнопка сортировки */
        QToolButton {
            background-color: #34495e;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: bold;
            min-height: 40px;
        }
        
        QToolButton:hover {
            background-color: #2c3e50;
        }
        
        /* Панель выбранной строки */
        QFrame#selectedRowFrame {
            background-color: #ecf0f1;
            border: 2px solid #3498db;
            border-radius: 8px;
            padding: 10px;
        }
        
        /* Метки */
        QLabel {
            color: #2c3e50;
            font-size: 14px;
        }
        
        /* Меню */
        QMenu {
            background-color: white;
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            padding: 5px;
            font-size: 14px;
        }
        
        QMenu::item {
            padding: 10px 20px;
            border-radius: 3px;
        }
        
        QMenu::item:selected {
            background-color: #3498db;
            color: white;
        }
    )";
    
    setStyleSheet(style);
    
    // Установка имен объектов для стилей
    btnAdd->setObjectName("btnAdd");
    btnDeleteSelected->setObjectName("btnDeleteSelected");
    btnDelete->setObjectName("btnDelete");
    btnRefresh->setObjectName("btnRefresh");
    btnEditSelected->setObjectName("btnEditSelected");
    selectedRowFrame->setObjectName("selectedRowFrame");
}

void MainWindow::setupTableHeader() {
    // Дополнительная настройка заголовков таблицы
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false);
    tableView->setShowGrid(true);
}

void MainWindow::setupContextMenu() {
    // Контекстное меню для таблицы
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::updateSelectedRowButtons() {
    bool hasSelection = (selectedRow >= 0);
    selectedRowFrame->setVisible(hasSelection);
    
    if (hasSelection && selectedRow < model->rowCount()) {
        QString brand = model->data(model->index(selectedRow, 1)).toString();
        QString manufacturer = model->data(model->index(selectedRow, 2)).toString();
        selectedRowLabel->setText(QString("Выбран: %1 %2").arg(brand).arg(manufacturer));
    }
}

void MainWindow::onTableRowClicked(const QModelIndex &index) {
    if (index.isValid()) {
        selectedRow = index.row();
        updateSelectedRowButtons();
    }
}

void MainWindow::onSortByPrice() {
    model->setSort(5, Qt::AscendingOrder); // Колонка цены
    model->select();
}

void MainWindow::onSortByBrand() {
    model->setSort(1, Qt::AscendingOrder); // Колонка производителя
    model->select();
}

void MainWindow::onSortByDate() {
    model->setSort(3, Qt::DescendingOrder); // Колонка даты (новые сначала)
    model->select();
}

void MainWindow::onSortByMileage() {
    model->setSort(4, Qt::AscendingOrder); // Колонка пробега
    model->select();
}

void MainWindow::onResetSort() {
    model->setSort(-1, Qt::AscendingOrder); // Сброс сортировки
    model->select();
}

void MainWindow::onRefreshTable() {
    // Сброс всех фильтров
    model->setFilter("");
    
    // Сброс сортировки
    model->setSort(-1, Qt::AscendingOrder);
    
    // Перезагрузка данных из базы данных
    model->select();
    
    // Сброс выбранной строки
    selectedRow = -1;
    updateSelectedRowButtons();
    
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
    
    form.addRow("Производитель:", brandEdit); 
    form.addRow("Модель:", manufEdit);        
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
        
        // Проверка пустых полей
        if (brand.isEmpty() || manuf.isEmpty() || dateStr.isEmpty() || 
            mileageStr.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
        // Проверка формата даты
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
        
        // Проверка числовых значений 
        bool mileageOk, priceOk;
        int mileage = mileageStr.toInt(&mileageOk);
        int price = priceStr.toInt(&priceOk);
        
        if (!mileageOk || !priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Пробег и цена должны быть числами.");
            return;
        }
        
        // Добавление записи 
        if (Operations::addCar(brand, manuf, date, mileage, price)) {
            QMessageBox::information(this, "Успех", 
                QString("Автомобиль '%1' успешно добавлен в базу данных.").arg(brand));
            onRefreshTable(); 
        } else {
            QMessageBox::critical(this, "Ошибка", 
                "Не удалось добавить запись в базу данных. Проверьте подключение.");
        }
    }
}

void MainWindow::onEditSelected() {
    if (selectedRow < 0 || selectedRow >= model->rowCount()) {
        QMessageBox::information(this, "Информация", 
            "Выберите автомобиль в таблице для редактирования.");
        return;
    }
    
    // Получить данные выбранной строки
    int id = model->data(model->index(selectedRow, 0)).toInt();
    QString currentBrand = model->data(model->index(selectedRow, 1)).toString();
    QString currentManuf = model->data(model->index(selectedRow, 2)).toString();
    QDate currentDate = model->data(model->index(selectedRow, 3)).toDate();
    int currentMileage = model->data(model->index(selectedRow, 4)).toInt();
    int currentPrice = model->data(model->index(selectedRow, 5)).toInt();
    
    // Диалог редактирования
    QDialog dialog(this);
    dialog.setWindowTitle("Редактировать автомобиль");
    
    QFormLayout form(&dialog);
    
    QLineEdit *brandEdit = new QLineEdit(&dialog);
    brandEdit->setText(currentBrand);
    
    QLineEdit *manufEdit = new QLineEdit(&dialog);
    manufEdit->setText(currentManuf);
    
    QLineEdit *dateEdit = new QLineEdit(&dialog);
    dateEdit->setText(currentDate.toString("dd.MM.yyyy"));
    dateEdit->setPlaceholderText("ДД.ММ.ГГГГ (например, 15.01.2020)");
    
    QLineEdit *mileageEdit = new QLineEdit(&dialog);
    mileageEdit->setText(QString::number(currentMileage));
    
    QLineEdit *priceEdit = new QLineEdit(&dialog);
    priceEdit->setText(QString::number(currentPrice));
    
    form.addRow("Производитель:", brandEdit);
    form.addRow("Модель:", manufEdit);
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
        
        // Проверка пустых полей
        if (brand.isEmpty() || manuf.isEmpty() || dateStr.isEmpty() || 
            mileageStr.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
        // Проверка формата даты
        QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
        if (!date.isValid()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Дата должна быть в формате ДД.ММ.ГГГГ (например, 15.01.2020).");
            return;
        }
        
        // Проверка года
        if (date.year() > 2025) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Год выпуска не может быть позже 2025.");
            return;
        }
        
        // Проверка числовых значений
        bool mileageOk, priceOk;
        int mileage = mileageStr.toInt(&mileageOk);
        int price = priceStr.toInt(&priceOk);
        
        if (!mileageOk || !priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Пробег и цена должны быть числами.");
            return;
        }
        
        if (mileage < 0 || price <= 0) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Пробег не может быть отрицательным, цена должна быть положительной.");
            return;
        }
        
        // Обновление записи
        QSqlQuery query;
        query.prepare("UPDATE cars SET brand = :brand, manufacturer = :manufacturer, "
                     "release_date = :date, mileage = :mileage, price = :price WHERE id = :id");
        query.bindValue(":brand", brand);
        query.bindValue(":manufacturer", manuf);
        query.bindValue(":date", date);
        query.bindValue(":mileage", mileage);
        query.bindValue(":price", price);
        query.bindValue(":id", id);
        
        if (query.exec()) {
            QMessageBox::information(this, "Успех", 
                "Данные автомобиля успешно обновлены.");
            onRefreshTable();
        } else {
            QMessageBox::critical(this, "Ошибка", 
                QString("Не удалось обновить запись:\n%1").arg(query.lastError().text()));
        }
    }
}

void MainWindow::onDeleteSelected() {
    if (selectedRow < 0 || selectedRow >= model->rowCount()) {
        QMessageBox::information(this, "Информация", 
            "Выберите автомобиль в таблице для удаления.");
        return;
    }
    
    // Получить данные выбранной строки
    QString brand = model->data(model->index(selectedRow, 1)).toString();
    QString manufacturer = model->data(model->index(selectedRow, 2)).toString();
    int id = model->data(model->index(selectedRow, 0)).toInt();
    
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
        
        // Диалог подтверждения
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение удаления", 
            QString("Вы уверены, что хотите удалить все автомобили модели '%1'?\n"
                    "Это действие нельзя отменить.").arg(manuf),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            // Выполнение удаления
            int deletedCount = Operations::deleteByManufacturer(manuf);
            
            if (deletedCount > 0) {
                // Успешное удаление
                QMessageBox::information(this, "Успех", 
                    QString("Удалено записей: %1").arg(deletedCount));
                onRefreshTable();
            } else if (deletedCount == 0) {
                // Нет записей для удаления
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
    
    form.addRow("Производитель:", brandEdit);
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
        
        // Проверка числового значения 
        bool priceOk;
        int price = priceStr.toInt(&priceOk);
        
        if (!priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Цена должна быть числом.");
            return;
        }
        
        // Проверка положительной цены 
        if (price <= 0) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Цена должна быть положительным числом.");
            return;
        }
        
        // Выполнение обновления 
        int updatedCount = Operations::updatePriceByBrand(brand, price);
        
        if (updatedCount > 0) {
            QMessageBox::information(this, "Успех", 
                QString("Обновлено записей: %1\nНовая цена для марки '%2': %3 руб.")
                    .arg(updatedCount).arg(brand).arg(price));
            onRefreshTable();
        } else if (updatedCount == 0) {
            // Нет записей для обновления
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
        // Применение фильтра к модели 
        QString filter = QString("price <= %1").arg(maxPrice);
        model->setFilter(filter);
        model->select();
        
        // Проверка наличия результатов
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
    // Диалог выбора имени файла
    QString filename = QFileDialog::getSaveFileName(this, 
        "Экспорт данных в файл", 
        "export.txt",
        "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!filename.isEmpty()) {
        // Выполнение экспорта
        if (Operations::exportToFile(filename)) {
            QFileInfo fileInfo(filename);
            // Сообщение об успехе с путем к файлу 
            QMessageBox::information(this, "Успех", 
                QString("Данные успешно экспортированы в файл:\n%1\n\n"
                        "Расположение: %2")
                    .arg(fileInfo.fileName())
                    .arg(fileInfo.absoluteFilePath()));
        } else {
            // Обработка ошибок записи
            QMessageBox::critical(this, "Ошибка экспорта", 
                QString("Не удалось создать или записать файл:\n%1\n\n"
                        "Проверьте права доступа и наличие свободного места на диске.")
                    .arg(filename));
        }
    }
}

void MainWindow::onShowFile() {
    // Диалог выбора файла 
    QString filename = QFileDialog::getOpenFileName(this, 
        "Выбрать файл для просмотра", 
        "",
        "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!filename.isEmpty()) {
        // Чтение содержимого файла
        QString content = Operations::readFileContent(filename);
        
        if (content.isEmpty()) {
            // Обработка ошибок чтения
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
            // Показ содержимого файла 
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
