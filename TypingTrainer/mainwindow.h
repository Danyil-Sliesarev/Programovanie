#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

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
    void onLessonChanged(int index);
    void startTraining();
    void restartTraining();
    void returnToMain();
    void chooseRandomLesson();
    void reloadLessons();
    void showAbout();

private:
    Ui::MainWindow *ui;

    QString currentLessonPath;
    QString currentLessonText;
    QStringList lessonLines;
    int lineIndex = 0;
    int charIndex = 0;

    QString lessonsDirectoryPath() const;
    void scanLessons(const QString &preferredPath = QString());
    bool loadLesson(const QString &path);
    void updateLessonPreview();
    void updateTrainingText();
    void resetLessonProgress();
    void setKeyboardPlaceholderState();
};

#endif 
