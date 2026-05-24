#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("StudentProject");
    QApplication::setApplicationName("TypingTrainer");

    MainWindow w;
    w.show();
    return app.exec();
}
