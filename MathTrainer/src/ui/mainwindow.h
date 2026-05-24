#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "data/datamodels.h"

class MainMenuWidget;
class TrainingWidget;
class ResultsWidget;
class HistoryWidget;
class SessionRepository;
class SettingsManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartTraining(const Settings &settings);
    void onTrainingFinished(const Session &session);
    void onShowHistory();
    void onSaveSession(const Session &session);
    void onRetryTraining();
    void onBackToMenu();
    void onDeleteSession(int id);

private:
    void showPage(QWidget *w);

    QStackedWidget   *m_stack;
    MainMenuWidget   *m_menuWidget;
    TrainingWidget   *m_trainingWidget;
    ResultsWidget    *m_resultsWidget;
    HistoryWidget    *m_historyWidget;

    SessionRepository *m_repo;
    SettingsManager   *m_settings;

    Settings m_lastSettings;
    Session  m_lastSession;
};
