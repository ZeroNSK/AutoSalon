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
#include "../auth/user.h"
#include "../auth/permission_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const User& user, QWidget *parent = nullptr);
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
    void onLogout();

private:
    void setupUi();
    void setupTableHeader();
    void setupContextMenu();
    void updateSelectedRowButtons();
    void updateFilterButtonText();
    void setupUserInterface();
    void updateButtonsForRole();
    
    QSqlTableModel *model;
    QTableView *tableView;
    
     
    QPushButton *btnAdd;
    QPushButton *btnRefresh;
    QPushButton *btnFilter;
    QPushButton *btnExport;
    QPushButton *btnShowFile;
    
     
    QPushButton *btnDeleteSelected;
    QPushButton *btnEditSelected;
    QFrame *selectedRowFrame;
    QLabel *selectedRowLabel;
    
     
    QPushButton *btnDelete;
    QPushButton *btnUpdate;
    QLabel *massOpsLabel;
    
     
    QToolButton *btnSort;
    QMenu *sortMenu;
    QAction *actionSortByPrice;
    QAction *actionSortByBrand;
    QAction *actionSortByDate;
    QAction *actionSortByMileage;
    QAction *actionResetSort;
    
     
    int selectedRow;
    
     
    bool isFilterActive;
    int currentMinPrice;
    int currentMaxPrice;
    
     
    User currentUser;
    QLabel *userInfoLabel;
    QPushButton *logoutButton;
    QWidget *userWidget;
};

#endif  
