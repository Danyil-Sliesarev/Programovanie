#include "mainwindow.h"
#include "mainmenuwidget.h"
#include "trainingwidget.h"
#include "resultswidget.h"
#include "historywidget.h"
#include "data/sessionrepository.h"
#include "data/settingsmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Math Trainer");
    setMinimumSize(520, 420);
    resize(600, 480);

    // Data paths
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    m_repo     = new SessionRepository(dataDir + "/sessions.json");
    m_settings = new SettingsManager(dataDir + "/settings.json");

    m_lastSettings = m_settings->load();

    // Widgets
    m_menuWidget     = new MainMenuWidget(this);
    m_trainingWidget = new TrainingWidget(this);
    m_resultsWidget  = new ResultsWidget(this);
    m_historyWidget  = new HistoryWidget(this);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_menuWidget);
    m_stack->addWidget(m_trainingWidget);
    m_stack->addWidget(m_resultsWidget);
    m_stack->addWidget(m_historyWidget);
    setCentralWidget(m_stack);

    // Connections
    connect(m_menuWidget, &MainMenuWidget::startTraining,
            this, &MainWindow::onStartTraining);
    connect(m_menuWidget, &MainMenuWidget::showHistory,
            this, &MainWindow::onShowHistory);

    connect(m_trainingWidget, &TrainingWidget::trainingFinished,
            this, &MainWindow::onTrainingFinished);
    connect(m_trainingWidget, &TrainingWidget::cancelRequested,
            this, &MainWindow::onBackToMenu);

    connect(m_resultsWidget, &ResultsWidget::saveRequested,
            this, &MainWindow::onSaveSession);
    connect(m_resultsWidget, &ResultsWidget::retryRequested,
            this, &MainWindow::onRetryTraining);
    connect(m_resultsWidget, &ResultsWidget::backToMenuRequested,
            this, &MainWindow::onBackToMenu);

    connect(m_historyWidget, &HistoryWidget::deleteRequested,
            this, &MainWindow::onDeleteSession);
    connect(m_historyWidget, &HistoryWidget::backRequested,
            this, &MainWindow::onBackToMenu);

    m_menuWidget->applySettings(m_lastSettings);
    showPage(m_menuWidget);
}

MainWindow::~MainWindow()
{
    delete m_repo;
    delete m_settings;
}

void MainWindow::onStartTraining(const Settings &settings)
{
    m_lastSettings = settings;
    m_settings->save(settings);
    m_trainingWidget->startSession(settings);
    showPage(m_trainingWidget);
}

void MainWindow::onTrainingFinished(const Session &session)
{
    m_lastSession = session;
    m_resultsWidget->showResults(session);
    showPage(m_resultsWidget);
}

void MainWindow::onShowHistory()
{
    auto sessions = m_repo->loadAll();
    m_historyWidget->setSessions(sessions);
    showPage(m_historyWidget);
}

void MainWindow::onSaveSession(const Session &session)
{
    Session s = session;
    if (!m_repo->add(s)) {
        QMessageBox::warning(this, "Помилка", "Не вдалося зберегти сесію.");
        return;
    }
    m_lastSession = s;
    m_resultsWidget->markSaved();
}

void MainWindow::onRetryTraining()
{
    m_trainingWidget->startSession(m_lastSettings);
    showPage(m_trainingWidget);
}

void MainWindow::onBackToMenu()
{
    m_menuWidget->applySettings(m_lastSettings);
    showPage(m_menuWidget);
}

void MainWindow::onDeleteSession(int id)
{
    if (!m_repo->remove(id)) {
        QMessageBox::warning(this, "Помилка", "Не вдалося видалити запис.");
        return;
    }
    auto sessions = m_repo->loadAll();
    m_historyWidget->setSessions(sessions);
}

void MainWindow::showPage(QWidget *w)
{
    m_stack->setCurrentWidget(w);
}
