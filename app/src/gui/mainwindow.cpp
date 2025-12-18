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
#include <QStatusBar>
#include <QApplication>
#include "login_dialog.h"

MainWindow::MainWindow(const User& user, QWidget *parent) : QMainWindow(parent), currentUser(user), selectedRow(-1), isFilterActive(false), currentMinPrice(0), currentMaxPrice(0), userWidget(nullptr) {
    setupUi();
    
     
    model = new QSqlTableModel(this);
    model->setTable("cars");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);  
    model->select();
    
     
     
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Производитель");  
    model->setHeaderData(2, Qt::Horizontal, "Модель");         
    model->setHeaderData(3, Qt::Horizontal, "Дата выпуска");
    model->setHeaderData(4, Qt::Horizontal, "Пробег");
    model->setHeaderData(5, Qt::Horizontal, "Цена");

    tableView->setModel(model);
    tableView->hideColumn(0);  
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
     
    connect(tableView, &QTableView::clicked, this, &MainWindow::onTableRowClicked);
    
    setupTableHeader();
    setupContextMenu();
    setupUserInterface();
    updateSelectedRowButtons();
    updateButtonsForRole();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
     
    QHBoxLayout *topLayout = new QHBoxLayout();
    
     
    btnAdd = new QPushButton("+ Добавить автомобиль", this);
    btnAdd->setToolTip("Добавить новую запись об автомобиле в базу данных");
    btnAdd->setMinimumHeight(45);
    btnAdd->setMinimumWidth(200);
    
    topLayout->addWidget(btnAdd);
    topLayout->addStretch();  
    
     
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
    
     
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout();
    
    QLabel *tableTitle = new QLabel("Список автомобилей", this);
    tableTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
    
     
    btnSort = new QToolButton(this);
    btnSort->setText("Сортировка");
    btnSort->setToolTip("Выберите способ сортировки данных");
    btnSort->setPopupMode(QToolButton::InstantPopup);
    btnSort->setMinimumHeight(35);
    btnSort->setMinimumWidth(120);
    
     
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
    
     
    tableView = new QTableView(this);
    
     
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
     
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
     
    tableView->setAlternatingRowColors(true);
    
    tableContainer->addWidget(tableView);
    mainLayout->addLayout(tableContainer);
    
     
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
    
     
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    
     
    massOpsLabel = new QLabel("Массовые операции:", this);
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
    
     
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddCar);
    connect(btnEditSelected, &QPushButton::clicked, this, &MainWindow::onEditSelected);
    connect(btnDeleteSelected, &QPushButton::clicked, this, &MainWindow::onDeleteSelected);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteByManufacturer);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindow::onUpdatePrice);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::onFilterByPrice);
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportToFile);
    connect(btnShowFile, &QPushButton::clicked, this, &MainWindow::onShowFile);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshTable);
    
     
    connect(actionSortByPrice, &QAction::triggered, this, &MainWindow::onSortByPrice);
    connect(actionSortByBrand, &QAction::triggered, this, &MainWindow::onSortByBrand);
    connect(actionSortByDate, &QAction::triggered, this, &MainWindow::onSortByDate);
    connect(actionSortByMileage, &QAction::triggered, this, &MainWindow::onSortByMileage);
    connect(actionResetSort, &QAction::triggered, this, &MainWindow::onResetSort);
    
    setWindowTitle("Автосалон - Система управления");
    
     
    setMinimumSize(1200, 700);
    resize(1400, 850);
     
    setFixedSize(1400, 850);
    
     
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
    
     
    btnAdd->setObjectName("btnAdd");
    btnDeleteSelected->setObjectName("btnDeleteSelected");
    btnDelete->setObjectName("btnDelete");
    btnRefresh->setObjectName("btnRefresh");
    btnEditSelected->setObjectName("btnEditSelected");
    selectedRowFrame->setObjectName("selectedRowFrame");
}

void MainWindow::setupTableHeader() {
     
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false);
    tableView->setShowGrid(true);
}

void MainWindow::setupContextMenu() {
     
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

void MainWindow::updateFilterButtonText() {
    if (isFilterActive) {
        if (currentMinPrice > 0 && currentMaxPrice > 0) {
            btnFilter->setText(QString("Фильтр: %1-%2 руб").arg(currentMinPrice).arg(currentMaxPrice));
        } else if (currentMaxPrice > 0) {
            btnFilter->setText(QString("Фильтр: до %1 руб").arg(currentMaxPrice));
        } else if (currentMinPrice > 0) {
            btnFilter->setText(QString("Фильтр: от %1 руб").arg(currentMinPrice));
        }
        btnFilter->setStyleSheet("background-color: #e67e22; color: white;");  
    } else {
        btnFilter->setText("Фильтр по цене");
        btnFilter->setStyleSheet("");  
    }
}

void MainWindow::onTableRowClicked(const QModelIndex &index) {
    if (index.isValid()) {
        selectedRow = index.row();
        updateSelectedRowButtons();
    }
}

void MainWindow::onSortByPrice() {
    model->setSort(5, Qt::AscendingOrder);  
    model->select();
}

void MainWindow::onSortByBrand() {
    model->setSort(1, Qt::AscendingOrder);  
    model->select();
}

void MainWindow::onSortByDate() {
    model->setSort(3, Qt::DescendingOrder);  
    model->select();
}

void MainWindow::onSortByMileage() {
    model->setSort(4, Qt::AscendingOrder);  
    model->select();
}

void MainWindow::onResetSort() {
    model->setSort(-1, Qt::AscendingOrder);  
    model->select();
}

void MainWindow::onRefreshTable() {
     
    model->setFilter("");
    
     
    model->setSort(-1, Qt::AscendingOrder);
    
     
    model->select();
    
     
    selectedRow = -1;
    updateSelectedRowButtons();
    
     
    tableView->reset();
}

void MainWindow::onAddCar() {
    if (!PermissionManager::canAddCars(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для добавления автомобилей.");
        return;
    }
    
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
        
         
        if (brand.isEmpty() || manuf.isEmpty() || dateStr.isEmpty() || 
            mileageStr.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
         
         
        QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
        if (!date.isValid()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Дата должна быть в формате ДД.ММ.ГГГГ (например, 15.01.2020).");
            return;
        }
        
         
        if (date.year() > 2025) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Год выпуска не может быть позже 2025.");
            return;
        }
        
         
        if (date > QDate::currentDate()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Дата выпуска не может быть в будущем.");
            return;
        }
        
         
        bool mileageOk, priceOk;
        int mileage = mileageStr.toInt(&mileageOk);
        int price = priceStr.toInt(&priceOk);
        
        if (!mileageOk || !priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Пробег и цена должны быть числами.");
            return;
        }
        
         
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
    if (!PermissionManager::canEditCars(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для редактирования автомобилей.");
        return;
    }
    
    if (selectedRow < 0 || selectedRow >= model->rowCount()) {
        QMessageBox::information(this, "Информация", 
            "Выберите автомобиль в таблице для редактирования.");
        return;
    }
    
     
    int id = model->data(model->index(selectedRow, 0)).toInt();
    QString currentBrand = model->data(model->index(selectedRow, 1)).toString();
    QString currentManuf = model->data(model->index(selectedRow, 2)).toString();
    QDate currentDate = model->data(model->index(selectedRow, 3)).toDate();
    int currentMileage = model->data(model->index(selectedRow, 4)).toInt();
    int currentPrice = model->data(model->index(selectedRow, 5)).toInt();
    
     
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
        
         
        if (brand.isEmpty() || manuf.isEmpty() || dateStr.isEmpty() || 
            mileageStr.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
         
        QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
        if (!date.isValid()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Дата должна быть в формате ДД.ММ.ГГГГ (например, 15.01.2020).");
            return;
        }
        
         
        if (date.year() > 2025) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Год выпуска не может быть позже 2025.");
            return;
        }
        
         
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
    if (!PermissionManager::canDeleteCar(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для удаления автомобилей.");
        return;
    }
    
    if (selectedRow < 0 || selectedRow >= model->rowCount()) {
        QMessageBox::information(this, "Информация", 
            "Выберите автомобиль в таблице для удаления.");
        return;
    }
    
     
    QString brand = model->data(model->index(selectedRow, 1)).toString();
    QString manufacturer = model->data(model->index(selectedRow, 2)).toString();
    int id = model->data(model->index(selectedRow, 0)).toInt();
    
     
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления", 
        QString("Вы уверены, что хотите удалить автомобиль:\n"
                "Производитель: %1\n"
                "Модель: %2\n\n"
                "Это действие нельзя отменить.").arg(brand).arg(manufacturer),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
         
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
    if (!PermissionManager::canMassDelete(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для массового удаления автомобилей.");
        return;
    }
    
    bool ok;
    QString manuf = QInputDialog::getText(this, "Удаление по модели", 
                                         "Введите название модели:", 
                                         QLineEdit::Normal, "", &ok);
    
    if (ok && !manuf.trimmed().isEmpty()) {
        manuf = manuf.trimmed();
        
         
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение удаления", 
            QString("Вы уверены, что хотите удалить все автомобили модели '%1'?\n"
                    "Это действие нельзя отменить.").arg(manuf),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
             
            int deletedCount = Operations::deleteByManufacturer(manuf);
            
            if (deletedCount > 0) {
                 
                QMessageBox::information(this, "Успех", 
                    QString("Удалено записей: %1").arg(deletedCount));
                onRefreshTable();
            } else if (deletedCount == 0) {
                 
                QMessageBox::information(this, "Информация", 
                    QString("Не найдено автомобилей модели '%1'. "
                            "Записи не были удалены.").arg(manuf));
            } else {
                 
                QMessageBox::critical(this, "Ошибка", 
                    "Не удалось выполнить операцию удаления. Проверьте подключение к базе данных.");
            }
        }
    }
}

void MainWindow::onUpdatePrice() {
    if (!PermissionManager::canUpdatePrices(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для изменения цен.");
        return;
    }
    
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
        
         
        if (brand.isEmpty() || priceStr.isEmpty()) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Все поля должны быть заполнены.");
            return;
        }
        
         
        bool priceOk;
        int price = priceStr.toInt(&priceOk);
        
        if (!priceOk) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Цена должна быть числом.");
            return;
        }
        
         
        if (price <= 0) {
            QMessageBox::warning(this, "Ошибка валидации", 
                "Цена должна быть положительным числом.");
            return;
        }
        
         
        int updatedCount = Operations::updatePriceByBrand(brand, price);
        
        if (updatedCount > 0) {
             
            QMessageBox::information(this, "Успех", 
                QString("Обновлено записей: %1\nНовая цена для марки '%2': %3 руб.")
                    .arg(updatedCount).arg(brand).arg(price));
            onRefreshTable();
        } else if (updatedCount == 0) {
             
            QMessageBox::information(this, "Информация", 
                QString("Не найдено автомобилей марки '%1'. "
                        "Записи не были обновлены.").arg(brand));
        } else {
             
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
         
        QString filter = QString("price <= %1").arg(maxPrice);
        model->setFilter(filter);
        model->select();
        
         
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
    if (!PermissionManager::canExportData(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для экспорта данных.");
        return;
    }
    
     
    QString filename = QFileDialog::getSaveFileName(this, 
        "Экспорт данных в файл", 
        "export.txt",
        "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!filename.isEmpty()) {
         
        if (Operations::exportToFile(filename)) {
            QFileInfo fileInfo(filename);
             
            QMessageBox::information(this, "Успех", 
                QString("Данные успешно экспортированы в файл:\n%1\n\n"
                        "Расположение: %2")
                    .arg(fileInfo.fileName())
                    .arg(fileInfo.absoluteFilePath()));
        } else {
             
            QMessageBox::critical(this, "Ошибка экспорта", 
                QString("Не удалось создать или записать файл:\n%1\n\n"
                        "Проверьте права доступа и наличие свободного места на диске.")
                    .arg(filename));
        }
    }
}

void MainWindow::onShowFile() {
    if (!PermissionManager::canViewFiles(currentUser.role)) {
        QMessageBox::warning(this, "Доступ запрещен", 
            "У вас нет прав для просмотра файлов.");
        return;
    }
    
     
    QString filename = QFileDialog::getOpenFileName(this, 
        "Выбрать файл для просмотра", 
        "",
        "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!filename.isEmpty()) {
         
        QString content = Operations::readFileContent(filename);
        
        if (content.isEmpty()) {
             
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

void MainWindow::setupUserInterface() {
    if (userWidget) {
        statusBar()->removeWidget(userWidget);
        delete userWidget;
        userWidget = nullptr;
    }
    
    statusBar()->clearMessage();
    
    userWidget = new QWidget(this);
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(10, 5, 10, 5);
    
    userInfoLabel = new QLabel(this);
    userInfoLabel->setText(QString("Пользователь: %1 (%2)")
                          .arg(currentUser.fullName)
                          .arg(currentUser.getRoleString()));
    userInfoLabel->setStyleSheet("font-weight: bold; color: #2c3e50; font-size: 14px;");
    
    logoutButton = new QPushButton("Выйти", this);
    logoutButton->setToolTip("Выйти из системы");
    logoutButton->setMinimumHeight(30);
    logoutButton->setMinimumWidth(80);
    logoutButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; border-radius: 4px; padding: 5px 15px; font-size: 12px; }");
    
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogout);
    
    userLayout->addWidget(userInfoLabel);
    userLayout->addStretch();
    userLayout->addWidget(logoutButton);
    
    statusBar()->addPermanentWidget(userWidget);
    statusBar()->showMessage(PermissionManager::getRoleDescription(currentUser.role));
}

void MainWindow::updateButtonsForRole() {
    UserRole role = currentUser.role;
    
    btnAdd->setVisible(PermissionManager::canAddCars(role));
    btnEditSelected->setVisible(PermissionManager::canEditCars(role));
    
    btnDeleteSelected->setVisible(PermissionManager::canDeleteCar(role));
    btnDelete->setVisible(PermissionManager::canMassDelete(role));
    
    btnUpdate->setVisible(PermissionManager::canUpdatePrices(role));
    
    bool hasMassOps = PermissionManager::canMassDelete(role) || PermissionManager::canUpdatePrices(role);
    massOpsLabel->setVisible(hasMassOps);
    
    btnExport->setVisible(PermissionManager::canExportData(role));
    btnShowFile->setVisible(PermissionManager::canViewFiles(role));
    
    btnFilter->setVisible(PermissionManager::canFilterData(role));
    btnRefresh->setVisible(true);
    
    setWindowTitle(QString("Автосалон - Система управления [%1]").arg(currentUser.getRoleString()));
}

void MainWindow::onLogout() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Выход из системы", 
        "Вы уверены, что хотите выйти из системы?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
         
        hide();
        
         
        LoginDialog loginDialog;
        if (loginDialog.exec() == QDialog::Accepted) {
             
            User newUser = loginDialog.getAuthenticatedUser();
            if (newUser.isValid()) {
                 
                currentUser = newUser;
                setupUserInterface();
                updateButtonsForRole();
                show();  
            } else {
                QApplication::quit();  
            }
        } else {
            QApplication::quit();  
        }
    }
}