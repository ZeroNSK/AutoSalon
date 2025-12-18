#include "login_dialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
    
     
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &LoginDialog::onUsernameChanged);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    
     
    usernameEdit->setFocus();
}

void LoginDialog::setupUi() {
    setWindowTitle("Автосалон - Авторизация");
    setModal(true);
    setFixedSize(500, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(25);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    
     
    QLabel *titleLabel = new QLabel("Система управления автосалоном", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Вход в систему", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 14px; color: #7f8c8d; margin-bottom: 20px;");
    mainLayout->addWidget(subtitleLabel);
    
     
    QFrame *formFrame = new QFrame(this);
    formFrame->setFrameStyle(QFrame::StyledPanel);
    formFrame->setStyleSheet("QFrame { background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 25px; }");
    
    QFormLayout *formLayout = new QFormLayout(formFrame);
    formLayout->setSpacing(18);
    formLayout->setContentsMargins(15, 15, 15, 15);
    
     
    usernameEdit = new QLineEdit(formFrame);
    usernameEdit->setPlaceholderText("Введите логин");
    usernameEdit->setMinimumHeight(40);
    formLayout->addRow("Логин:", usernameEdit);
    
     
    passwordEdit = new QLineEdit(formFrame);
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(40);
    formLayout->addRow("Пароль:", passwordEdit);
    
    mainLayout->addWidget(formFrame);
    
     
    roleInfoLabel = new QLabel(this);
    roleInfoLabel->setWordWrap(true);
    roleInfoLabel->setStyleSheet("font-size: 13px; color: #5d6d7e; background-color: #f8f9fa; padding: 20px; border-radius: 8px; border: 1px solid #e9ecef; line-height: 1.4;");
    roleInfoLabel->setText("Доступные учетные записи:\n\n"
                          "   • admin/admin123 - Администратор (полный доступ)\n"
                          "   • manager/manager123 - Менеджер (управление инвентарем)\n"
                          "   • seller/seller123 - Продавец (просмотр и экспорт)\n"
                          "   • guest/guest123 - Гость (только просмотр)");
    mainLayout->addWidget(roleInfoLabel);
    
     
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    statusLabel->setVisible(false);
    mainLayout->addWidget(statusLabel);
    
     
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    buttonLayout->setContentsMargins(0, 10, 0, 0);
    
    cancelButton = new QPushButton("Отмена", this);
    cancelButton->setMinimumHeight(45);
    cancelButton->setMinimumWidth(130);
    
    loginButton = new QPushButton("Войти", this);
    loginButton->setMinimumHeight(45);
    loginButton->setMinimumWidth(130);
    loginButton->setDefault(true);
    loginButton->setEnabled(false);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(loginButton);
    
    mainLayout->addLayout(buttonLayout);
    
     
    QString style = R"(
        QLineEdit {
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            padding: 12px;
            font-size: 15px;
        }
        
        QLineEdit:focus {
            border-color: #3498db;
            background-color: #fbfcfd;
        }
        
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 12px 25px;
            font-size: 15px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #2980b9;
        }
        
        QPushButton:pressed {
            background-color: #21618c;
        }
        
        QPushButton:disabled {
            background-color: #bdc3c7;
            color: #7f8c8d;
        }
        
        QPushButton#cancelButton {
            background-color: #95a5a6;
        }
        
        QPushButton#cancelButton:hover {
            background-color: #7f8c8d;
        }
        
        QLabel {
            color: #2c3e50;
            font-size: 14px;
        }
        
        QFormLayout QLabel {
            font-weight: bold;
            color: #34495e;
        }
    )";
    
    setStyleSheet(style);
    cancelButton->setObjectName("cancelButton");
}

void LoginDialog::onLoginClicked() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("Заполните все поля");
        statusLabel->setVisible(true);
        return;
    }
    
     
    User user = UserManager::authenticateUser(username, password);
    
    if (user.isValid()) {
        authenticatedUser = user;
        statusLabel->setVisible(false);
        accept();  
    } else {
        statusLabel->setText("Неверный логин или пароль");
        statusLabel->setVisible(true);
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::onCancelClicked() {
    reject();  
}

void LoginDialog::onUsernameChanged() {
    updateLoginButton();
}

void LoginDialog::updateLoginButton() {
    bool hasUsername = !usernameEdit->text().trimmed().isEmpty();
    bool hasPassword = !passwordEdit->text().isEmpty();
    loginButton->setEnabled(hasUsername && hasPassword);
}