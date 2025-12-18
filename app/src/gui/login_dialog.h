#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QFrame>
#include "../auth/user.h"
#include "../auth/user_manager.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    
    User getAuthenticatedUser() const { return authenticatedUser; }

private slots:
    void onLoginClicked();
    void onCancelClicked();
    void onRegisterClicked();
    void onUsernameChanged();

private:
    void setupUi();
    void updateLoginButton();
    
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QPushButton *registerButton;
    QLabel *statusLabel;
    QLabel *roleInfoLabel;
    
    User authenticatedUser;
};

#endif  