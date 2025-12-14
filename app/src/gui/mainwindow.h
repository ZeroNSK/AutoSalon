#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QFrame>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddCar();
    void onEditSelected();
    void onDeleteSelected();
    void onDeleteByManufacturer();
    void onUpdatePrice();
    void onFilterByPrice();
    void onExportToFile();
    void onShowFile();
    void onRefreshTable();
    void onTableRowClicked(const QModelIndex &index);
    void onSortByPrice();
    void onSortByBrand();
    void onSortByDate();
    void onSortByMileage();
    void onResetSort();

private:
    void setupUi();
    void setupTableHeader();
    void setupContextMenu();
    void updateSelectedRowButtons();
    
    QSqlTableModel *model;
    QTableView *tableView;
    
    // Основные кнопки действий
    QPushButton *btnAdd;
    QPushButton *btnRefresh;
    QPushButton *btnFilter;
    QPushButton *btnExport;
    QPushButton *btnShowFile;
    
    // Кнопки для выбранной строки (скрыты по умолчанию)
    QPushButton *btnDeleteSelected;
    QPushButton *btnEditSelected;
    QFrame *selectedRowFrame;
    QLabel *selectedRowLabel;
    
    // Кнопки массовых операций
    QPushButton *btnDelete;
    QPushButton *btnUpdate;
    
    // Меню сортировки
    QToolButton *btnSort;
    QMenu *sortMenu;
    QAction *actionSortByPrice;
    QAction *actionSortByBrand;
    QAction *actionSortByDate;
    QAction *actionSortByMileage;
    QAction *actionResetSort;
    
    // Текущая выбранная строка
    int selectedRow;
};

#endif // MAINWINDOW_H
