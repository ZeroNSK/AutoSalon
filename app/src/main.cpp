#include <QApplication>
#include <QMessageBox>
#include "database.h"
#include "gui/mainwindow.h"
#include "gui/login_dialog.h"
#include "auth/user_manager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

     
    if (!Database::connect()) {
         
        QMessageBox::critical(
            nullptr,
            "Ошибка подключения к базе данных",
            "Не удалось подключиться к базе данных PostgreSQL.\n"
            "Проверьте настройки подключения и убедитесь, что сервер базы данных запущен.\n\n"
            "Приложение будет закрыто."
        );
        return -1;
    }

     
    if (!Database::initTable()) {
         
        QMessageBox::critical(
            nullptr,
            "Ошибка инициализации базы данных",
            "Не удалось создать таблицу в базе данных.\n"
            "Проверьте права доступа и целостность базы данных.\n\n"
            "Приложение будет закрыто."
        );
        return -1;
    }

     
    if (!UserManager::initializeUserTable()) {
        QMessageBox::critical(
            nullptr,
            "Ошибка инициализации системы пользователей",
            "Не удалось создать таблицу пользователей.\n"
            "Проверьте права доступа к базе данных.\n\n"
            "Приложение будет закрыто."
        );
        return -1;
    }

     
    UserManager::createDefaultUsers();

     
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
         
        return 0;
    }

     
    User currentUser = loginDialog.getAuthenticatedUser();
    if (!currentUser.isValid()) {
        QMessageBox::critical(
            nullptr,
            "Ошибка авторизации",
            "Не удалось получить данные пользователя.\n\n"
            "Приложение будет закрыто."
        );
        return -1;
    }

     
    MainWindow w(currentUser);
    w.show();

    return app.exec();
}
