#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QMap>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

class QAction;
class QComboBox;
class QGridLayout;
class QLabel;
class QProgressBar;
class QStackedWidget;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void startTraining();
    void restartTraining();
    void returnToMain();
    void showAbout();
    void scanLessons();
    void loadSelectedLesson();
    void chooseRandomLesson();
    void onTimerTick();
    void onSpeedMetricChanged(int index);

private:
    enum ScreenIndex { StartScreen = 0, TrainingScreen = 1, ResultsScreen = 2 };

    void buildUi();
    void buildMenu();
    QWidget *buildStartPage();
    QWidget *buildTrainingPage();
    QWidget *buildResultsPage();
    QWidget *buildKeyboard();

    QString lessonsDirectory() const;
    bool loadLessonFromPath(const QString &path);
    void resetSession();
    void finishSession();
    void handleTypedText(const QString &text);
    void handleBackspace();
    void handleEnter();

    void updateTrainingView();
    void updateStatsLabels();
    void updateResultsView();
    void highlightVirtualKey(const QString &keyText);
    void clearVirtualKeyHighlight();

    QString formatTime(qint64 milliseconds) const;
    double currentSpeed() const;
    double currentAccuracy() const;
    QString speedUnit() const;
    QString currentExpectedChar() const;

    QString htmlEscapedWithSpaces(const QString &text) const;
    QString styleSpan(const QString &text, const QString &style) const;

    void loadSettings();
    void saveSettings();
    void restoreLastLessonSelection();

    QStackedWidget *stackScreens = nullptr;

    QWidget *pageStart = nullptr;
    QWidget *pageTraining = nullptr;
    QWidget *pageResults = nullptr;

    QComboBox *comboLessons = nullptr;
    QComboBox *comboSpeedMetric = nullptr;
    QLabel *labelLessonDescription = nullptr;
    QPushButton *btnStart = nullptr;
    QPushButton *btnRandomLesson = nullptr;
    QPushButton *btnReloadLessons = nullptr;

    QLabel *labelTime = nullptr;
    QLabel *labelSpeed = nullptr;
    QLabel *labelAccuracy = nullptr;
    QProgressBar *progressAccuracy = nullptr;
    QLabel *labelPreviousLine = nullptr;
    QLabel *labelCurrentLine = nullptr;
    QLabel *labelNextLine = nullptr;
    QWidget *keyboardWidget = nullptr;
    QPushButton *btnRestartTraining = nullptr;
    QPushButton *btnReturnToMain = nullptr;
    QPushButton *btnFinishForTest = nullptr;

    QLabel *labelResultTime = nullptr;
    QLabel *labelResultSpeed = nullptr;
    QLabel *labelResultAccuracy = nullptr;
    QPushButton *btnResultsRestart = nullptr;
    QPushButton *btnResultsReturn = nullptr;

    QAction *actionExit = nullptr;
    QAction *actionAbout = nullptr;
    QAction *actionRandomLesson = nullptr;
    QAction *actionReloadLessons = nullptr;

    QTimer sessionTimer;
    QElapsedTimer elapsedTimer;

    QString currentLessonPath;
    QString currentLessonTitle;
    QString fullText;
    QStringList lines;
    int lineIndex = 0;
    int charIndex = 0;
    QVector<QVector<int>> charStates; // 0 untouched, 1 correct, 2 error

    int totalTyped = 0;
    int correctTyped = 0;
    qint64 finalElapsedMs = 0;
    bool sessionActive = false;

    QMap<QString, QPushButton*> keyButtons;
    QPushButton *activeKeyButton = nullptr;
};

#endif // MAINWINDOW_H
