#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MathTrainer");
    app.setOrganizationName("MathTrainer");

    MainWindow w;
    w.show();
    return app.exec();
}
