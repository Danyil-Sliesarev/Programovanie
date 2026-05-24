#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("PasswordManager");
    QApplication::setOrganizationName("CourseProject");

    MainWindow window;
    window.show();
    return app.exec();
}
