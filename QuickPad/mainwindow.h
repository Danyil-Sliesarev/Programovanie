#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newDocument();
    void openDocumentDialog();
    void saveDocumentDialog();
    void saveAsDocumentDialog();
    void exitApplication();
    void showAboutDialog();
    void updateCursorPosition();
    void markModified();
    void findDialog();

private:
    void setupConnections();
    void setupInitialState();
    void setDocumentTitle(const QString &title);

    Ui::MainWindow *ui;
    QString currentDocumentName;
    bool modified;
};

#endif 
