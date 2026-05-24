#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QSignalBlocker>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(ui->actionRandomLesson, &QAction::triggered, this, &MainWindow::chooseRandomLesson);
    connect(ui->actionReloadLessons, &QAction::triggered, this, &MainWindow::reloadLessons);

    connect(ui->comboLesson, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLessonChanged);
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::startTraining);
    connect(ui->btnRandom, &QPushButton::clicked, this, &MainWindow::chooseRandomLesson);
    connect(ui->btnReload, &QPushButton::clicked, this, &MainWindow::reloadLessons);
    connect(ui->btnRestart, &QPushButton::clicked, this, &MainWindow::restartTraining);
    connect(ui->btnReturn, &QPushButton::clicked, this, &MainWindow::returnToMain);
    connect(ui->btnFinishDemo, &QPushButton::clicked, [this]() {
        ui->stackScreens->setCurrentWidget(ui->pageResults);
    });

    ui->stackScreens->setCurrentWidget(ui->pageStart);
    setKeyboardPlaceholderState();
    scanLessons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::lessonsDirectoryPath() const
{
    const QString appLessons = QCoreApplication::applicationDirPath() + "/lessons";
    if (QDir(appLessons).exists()) {
        return appLessons;
    }
    return QDir::current().filePath("lessons");
}

void MainWindow::scanLessons(const QString &preferredPath)
{
    const QString previousPath = preferredPath.isEmpty() ? ui->comboLesson->currentData().toString() : preferredPath;
    const QString dirPath = lessonsDirectoryPath();
    QDir dir(dirPath);

    QSignalBlocker blocker(ui->comboLesson);
    ui->comboLesson->clear();

    if (!dir.exists()) {
        ui->lblDescription->setText("Lessons folder was not found: " + dirPath);
        ui->btnStart->setEnabled(false);
        ui->btnRandom->setEnabled(false);
        ui->actionRandomLesson->setEnabled(false);
        currentLessonPath.clear();
        currentLessonText.clear();
        lessonLines.clear();
        updateTrainingText();
        return;
    }

    const QStringList files = dir.entryList(QStringList() << "*.txt", QDir::Files, QDir::Name);
    int preferredIndex = -1;

    for (const QString &fileName : files) {
        const QString path = dir.filePath(fileName);
        QFileInfo info(path);
        QString title = info.baseName();
        title.replace('_', ' ');
        const QString visibleText = title + "  (" + QString::number(info.size()) + " B)";
        ui->comboLesson->addItem(visibleText, info.absoluteFilePath());
        if (info.absoluteFilePath() == previousPath) {
            preferredIndex = ui->comboLesson->count() - 1;
        }
    }

    const bool hasLessons = ui->comboLesson->count() > 0;
    ui->btnStart->setEnabled(hasLessons);
    ui->btnRandom->setEnabled(hasLessons);
    ui->actionRandomLesson->setEnabled(hasLessons);

    if (!hasLessons) {
        ui->lblDescription->setText("No .txt lesson files found in: " + dirPath);
        currentLessonPath.clear();
        currentLessonText.clear();
        lessonLines.clear();
        updateTrainingText();
        return;
    }

    ui->comboLesson->setCurrentIndex(preferredIndex >= 0 ? preferredIndex : 0);
    blocker.unblock();
    onLessonChanged(ui->comboLesson->currentIndex());
}

void MainWindow::reloadLessons()
{
    scanLessons(ui->comboLesson->currentData().toString());
}

void MainWindow::onLessonChanged(int index)
{
    if (index < 0) {
        return;
    }
    const QString path = ui->comboLesson->itemData(index).toString();
    loadLesson(path);
}

bool MainWindow::loadLesson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Lesson error", "Cannot open lesson:\n" + path + "\n\n" + file.errorString());
        return false;
    }

    QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");
#endif
    QString text = in.readAll();
    text.replace("\r\n", "\n");
    text.replace('\r', '\n');

    currentLessonPath = path;
    currentLessonText = text.trimmed();
    lessonLines = currentLessonText.split('\n', Qt::SkipEmptyParts);
    resetLessonProgress();
    updateLessonPreview();
    updateTrainingText();
    return true;
}

void MainWindow::resetLessonProgress()
{
    lineIndex = 0;
    charIndex = 0;
}

void MainWindow::updateLessonPreview()
{
    QFileInfo info(currentLessonPath);
    ui->lblDescription->setText(
        "Selected lesson: " + info.fileName() +
        "\nLines: " + QString::number(lessonLines.size()) +
        "\nPath: " + info.absoluteFilePath()
    );
}

void MainWindow::updateTrainingText()
{
    QString html;
    for (int i = 0; i < lessonLines.size(); ++i) {
        QString line = lessonLines.at(i).toHtmlEscaped();
        if (i == lineIndex) {
            html += "<div style='background:#fff2b8; font-weight:bold; padding:4px;'>" + line + "</div>";
        } else if (i < lineIndex) {
            html += "<div style='background:#e8f5e9; padding:4px;'>" + line + "</div>";
        } else {
            html += "<div style='padding:4px;'>" + line + "</div>";
        }
    }
    if (html.isEmpty()) {
        html = "<i>No lesson loaded</i>";
    }
    ui->textDisplay->setHtml(html);
}

void MainWindow::startTraining()
{
    if (ui->comboLesson->count() == 0) {
        QMessageBox::information(this, "TypingTrainer", "No lessons available. Add .txt files to the lessons folder.");
        return;
    }
    if (currentLessonText.isEmpty()) {
        onLessonChanged(ui->comboLesson->currentIndex());
    }
    resetLessonProgress();
    updateTrainingText();
    ui->stackScreens->setCurrentWidget(ui->pageTraining);
}

void MainWindow::restartTraining()
{
    resetLessonProgress();
    updateTrainingText();
    ui->stackScreens->setCurrentWidget(ui->pageTraining);
}

void MainWindow::returnToMain()
{
    ui->stackScreens->setCurrentWidget(ui->pageStart);
}

void MainWindow::chooseRandomLesson()
{
    const int count = ui->comboLesson->count();
    if (count <= 0) {
        return;
    }
    const int index = QRandomGenerator::global()->bounded(count);
    ui->comboLesson->setCurrentIndex(index);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About TypingTrainer",
                       "TypingTrainer\n\nQt Widgets training project.\nLessons are loaded from text files in the lessons folder.");
}

void MainWindow::setKeyboardPlaceholderState()
{
    const QList<QPushButton *> keys = ui->keyboardWidget->findChildren<QPushButton *>();
    for (QPushButton *key : keys) {
        key->setFocusPolicy(Qt::NoFocus);
        key->setMinimumHeight(28);
    }
}
