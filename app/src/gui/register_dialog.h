#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include "../auth/user.h"
#include "../auth/user_manager.h"

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    
    User getRegisteredUser() const { return registeredUser; }

private slots:
    void onRegisterClicked();
    void onCancelClicked();
    void onFieldChanged();

private:
    void setupUi();
    void updateRegisterButton();
    bool validateInput();
    
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLineEdit *fullNameEdit;
    QComboBox *roleComboBox;
    QPushButton *registerButton;
    QPushButton *cancelButton;
    QLabel *statusLabel;
    
    User registeredUser;
};

#endif