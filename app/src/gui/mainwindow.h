#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddCar();
    void onDeleteSelected();
    void onDeleteByManufacturer();
    void onUpdatePrice();
    void onFilterByPrice();
    void onExportToFile();
    void onShowFile();
    void onRefreshTable();

private:
    void setupUi();
    
    QSqlTableModel *model;
    QTableView *tableView;
    
    QPushButton *btnAdd;
    QPushButton *btnDeleteSelected;
    QPushButton *btnDelete;
    QPushButton *btnUpdate;
    QPushButton *btnFilter;
    QPushButton *btnExport;
    QPushButton *btnShowFile;
    QPushButton *btnRefresh;
};

#endif // MAINWINDOW_H
