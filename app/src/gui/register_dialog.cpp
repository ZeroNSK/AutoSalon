#include "register_dialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
    
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterDialog::onCancelClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &RegisterDialog::onFieldChanged);
    connect(passwordEdit, &QLineEdit::textChanged, this, &RegisterDialog::onFieldChanged);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &RegisterDialog::onFieldChanged);
    connect(fullNameEdit, &QLineEdit::textChanged, this, &RegisterDialog::onFieldChanged);
    
    usernameEdit->setFocus();
}

void RegisterDialog::setupUi() {
    setWindowTitle("Автосалон - Регистрация");
    setModal(true);
    setFixedSize(480, 520);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    
    QLabel *titleLabel = new QLabel("Регистрация нового пользователя", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("Заполните все поля для создания учетной записи", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-bottom: 15px;");
    mainLayout->addWidget(subtitleLabel);
    
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(12);
    
    QLabel *usernameLabel = new QLabel("Логин:", this);
    usernameLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(usernameLabel);
    
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Введите уникальный логин");
    usernameEdit->setMinimumHeight(35);
    usernameEdit->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(usernameEdit);
    
    QLabel *fullNameLabel = new QLabel("Полное имя:", this);
    fullNameLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(fullNameLabel);
    
    fullNameEdit = new QLineEdit(this);
    fullNameEdit->setPlaceholderText("Введите ваше полное имя");
    fullNameEdit->setMinimumHeight(35);
    fullNameEdit->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(fullNameEdit);
    
    QLabel *roleLabel = new QLabel("Роль:", this);
    roleLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(roleLabel);
    
    roleComboBox = new QComboBox(this);
    roleComboBox->addItem("Гость", static_cast<int>(UserRole::Guest));
    roleComboBox->addItem("Продавец", static_cast<int>(UserRole::Seller));
    roleComboBox->addItem("Менеджер", static_cast<int>(UserRole::Manager));
    roleComboBox->addItem("Администратор", static_cast<int>(UserRole::Admin));
    roleComboBox->setCurrentIndex(0);
    roleComboBox->setMinimumHeight(35);
    roleComboBox->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(roleComboBox);
    
    QLabel *passwordLabel = new QLabel("Пароль:", this);
    passwordLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(passwordLabel);
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);
    passwordEdit->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(passwordEdit);
    
    QLabel *confirmPasswordLabel = new QLabel("Подтверждение пароля:", this);
    confirmPasswordLabel->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(confirmPasswordLabel);
    
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("Повторите пароль");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setMinimumHeight(35);
    confirmPasswordEdit->setStyleSheet("border: 2px solid #bdc3c7; border-radius: 4px; padding: 8px; font-size: 14px;");
    formLayout->addWidget(confirmPasswordEdit);
    
    mainLayout->addLayout(formLayout);
    
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    statusLabel->setVisible(false);
    mainLayout->addWidget(statusLabel);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    
    cancelButton = new QPushButton("Отмена", this);
    cancelButton->setMinimumHeight(40);
    cancelButton->setMinimumWidth(120);
    cancelButton->setStyleSheet("background-color: #95a5a6; color: white; border: none; border-radius: 4px; font-weight: bold;");
    
    registerButton = new QPushButton("Зарегистрироваться", this);
    registerButton->setMinimumHeight(40);
    registerButton->setMinimumWidth(160);
    registerButton->setDefault(true);
    registerButton->setEnabled(false);
    registerButton->setStyleSheet("background-color: #27ae60; color: white; border: none; border-radius: 4px; font-weight: bold;");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(registerButton);
    
    mainLayout->addLayout(buttonLayout);
}

void RegisterDialog::onRegisterClicked() {
    if (!validateInput()) {
        return;
    }
    
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString fullName = fullNameEdit->text().trimmed();
    UserRole role = static_cast<UserRole>(roleComboBox->currentData().toInt());
    
    if (UserManager::registerUser(username, password, fullName, role)) {
        registeredUser = UserManager::authenticateUser(username, password);
        if (registeredUser.isValid()) {
            QMessageBox::information(this, "Успех", 
                QString("Пользователь '%1' успешно зарегистрирован!").arg(fullName));
            accept();
        } else {
            statusLabel->setText("Ошибка при создании пользователя");
            statusLabel->setVisible(true);
        }
    } else {
        statusLabel->setText("Пользователь с таким логином уже существует");
        statusLabel->setVisible(true);
    }
}

void RegisterDialog::onCancelClicked() {
    reject();
}

void RegisterDialog::onFieldChanged() {
    updateRegisterButton();
    statusLabel->setVisible(false);
}

void RegisterDialog::updateRegisterButton() {
    bool hasUsername = !usernameEdit->text().trimmed().isEmpty();
    bool hasPassword = !passwordEdit->text().isEmpty();
    bool hasConfirmPassword = !confirmPasswordEdit->text().isEmpty();
    bool hasFullName = !fullNameEdit->text().trimmed().isEmpty();
    
    registerButton->setEnabled(hasUsername && hasPassword && hasConfirmPassword && hasFullName);
}

bool RegisterDialog::validateInput() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    QString fullName = fullNameEdit->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() || fullName.isEmpty()) {
        statusLabel->setText("Заполните все поля");
        statusLabel->setVisible(true);
        return false;
    }
    
    if (username.length() < 3) {
        statusLabel->setText("Логин должен содержать минимум 3 символа");
        statusLabel->setVisible(true);
        return false;
    }
    
    if (password.length() < 6) {
        statusLabel->setText("Пароль должен содержать минимум 6 символов");
        statusLabel->setVisible(true);
        return false;
    }
    
    if (password != confirmPassword) {
        statusLabel->setText("Пароли не совпадают");
        statusLabel->setVisible(true);
        return false;
    }
    
    if (fullName.length() < 2) {
        statusLabel->setText("Введите корректное полное имя");
        statusLabel->setVisible(true);
        return false;
    }
    
    return true;
}