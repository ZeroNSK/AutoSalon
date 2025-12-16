#include <QApplication>
#include <QMessageBox>
#include "database.h"
#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Подключение к БД
    if (!Database::connect()) {
        // Обработка ошибки подключения 
        QMessageBox::critical(
            nullptr,
            "Ошибка подключения к базе данных",
            "Не удалось подключиться к базе данных PostgreSQL.\n"
            "Проверьте настройки подключения и убедитесь, что сервер базы данных запущен.\n\n"
            "Приложение будет закрыто."
        );
        return -1;
    }

    // Инициализация таблицы 
    if (!Database::initTable()) {
        // Обработка ошибки инициализации таблицы
        QMessageBox::critical(
            nullptr,
            "Ошибка инициализации базы данных",
            "Не удалось создать таблицу в базе данных.\n"
            "Проверьте права доступа и целостность базы данных.\n\n"
            "Приложение будет закрыто."
        );
        return -1;
    }

    // Создание и показ главного окна
    MainWindow w;
    w.show();

    return app.exec();
}
