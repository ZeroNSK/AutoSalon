#include "login_dialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
    
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &LoginDialog::onUsernameChanged);
    connect(passwordEdit, &QLineEdit::textChanged, this, &LoginDialog::onUsernameChanged);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    
    usernameEdit->setFocus();
}

void LoginDialog::setupUi() {
    setWindowTitle("Автосалон - Авторизация");
    setModal(true);
    setFixedSize(460, 420);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    
    QLabel *titleLabel = new QLabel("Система управления автосалоном", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; margin-bottom: 5px;");
    mainLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Вход в систему", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-bottom: 10px;");
    mainLayout->addWidget(subtitleLabel);
    
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(10);
    
    QLabel *loginLabel = new QLabel("Логин:", this);
    loginLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(loginLabel);
    
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Введите логин");
    usernameEdit->setMinimumHeight(35);
    usernameEdit->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(usernameEdit);
    
    QLabel *passwordLabel = new QLabel("Пароль:", this);
    passwordLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(passwordLabel);
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);
    passwordEdit->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(passwordEdit);
    
    mainLayout->addLayout(formLayout);
    
    roleInfoLabel = new QLabel(this);
    roleInfoLabel->setWordWrap(true);
    roleInfoLabel->setMinimumHeight(150);
    roleInfoLabel->setStyleSheet("font-size: 11px; color: #5d6d7e; background-color: #f8f9fa; padding: 15px; border-radius: 5px; border: 1px solid #e9ecef; line-height: 1.3;");
    roleInfoLabel->setText("Доступные учетные записи: (Демонстрация только при защите работы для удобства проверяющих)\n\n"
                          "• admin/admin123 - Администратор (полный доступ)\n"
                          "• manager/manager123 - Менеджер (управление инвентарем)\n"
                          "• seller/seller123 - Продавец (просмотр и экспорт)\n"
                          "• guest/guest123 - Гость (только просмотр)");
    mainLayout->addWidget(roleInfoLabel);
    
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    statusLabel->setVisible(false);
    mainLayout->addWidget(statusLabel);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    cancelButton = new QPushButton("Отмена", this);
    cancelButton->setMinimumHeight(40);
    cancelButton->setMinimumWidth(100);
    cancelButton->setStyleSheet("background-color: #95a5a6; color: white; border: none; border-radius: 4px; font-weight: bold;");
    
    loginButton = new QPushButton("Войти", this);
    loginButton->setMinimumHeight(40);
    loginButton->setMinimumWidth(100);
    loginButton->setDefault(true);
    loginButton->setEnabled(false);
    loginButton->setStyleSheet("background-color: #3498db; color: white; border: none; border-radius: 4px; font-weight: bold;");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(loginButton);
    
    mainLayout->addLayout(buttonLayout);
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