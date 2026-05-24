#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStatusBar>
#include <QTextStream>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
    buildMenu();

    connect(&sessionTimer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    sessionTimer.setInterval(1000);

    scanLessons();
    loadSettings();
    restoreLastLessonSelection();
    loadSelectedLesson();

    statusBar()->showMessage("Ready");
    resize(920, 620);
}

void MainWindow::buildUi()
{
    setObjectName("mainWindow");
    setWindowTitle("TypingTrainer");

    stackScreens = new QStackedWidget(this);
    stackScreens->setObjectName("stackScreens");
    setCentralWidget(stackScreens);

    pageStart = buildStartPage();
    pageTraining = buildTrainingPage();
    pageResults = buildResultsPage();

    stackScreens->addWidget(pageStart);
    stackScreens->addWidget(pageTraining);
    stackScreens->addWidget(pageResults);
    stackScreens->setCurrentIndex(StartScreen);

    pageTraining->installEventFilter(this);
    pageTraining->setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::buildMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QMenu *lessonMenu = menuBar()->addMenu("&Lesson");
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    actionRandomLesson = new QAction("Random lesson", this);
    actionRandomLesson->setObjectName("actionRandomLesson");
    actionRandomLesson->setShortcut(QKeySequence("Ctrl+R"));
    actionRandomLesson->setStatusTip("Choose a random lesson");
    connect(actionRandomLesson, &QAction::triggered, this, &MainWindow::chooseRandomLesson);

    actionReloadLessons = new QAction("Reload lessons", this);
    actionReloadLessons->setObjectName("actionReloadLessons");
    actionReloadLessons->setShortcut(QKeySequence("F5"));
    actionReloadLessons->setStatusTip("Reload lessons from folder");
    connect(actionReloadLessons, &QAction::triggered, this, &MainWindow::scanLessons);

    actionExit = new QAction("E&xit", this);
    actionExit->setObjectName("actionExit");
    actionExit->setShortcut(QKeySequence("Ctrl+Q"));
    actionExit->setStatusTip("Exit application");
    connect(actionExit, &QAction::triggered, this, &QWidget::close);

    actionAbout = new QAction("&About", this);
    actionAbout->setObjectName("actionAbout");
    actionAbout->setStatusTip("About TypingTrainer");
    connect(actionAbout, &QAction::triggered, this, &MainWindow::showAbout);

    fileMenu->addAction(actionExit);
    lessonMenu->addAction(actionRandomLesson);
    lessonMenu->addAction(actionReloadLessons);
    helpMenu->addAction(actionAbout);
}

QWidget *MainWindow::buildStartPage()
{
    QWidget *page = new QWidget(this);
    page->setObjectName("pageStart");

    auto *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(36, 30, 36, 30);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("Typing Trainer", page);
    title->setObjectName("labelStartTitle");
    QFont titleFont = title->font();
    titleFont.setPointSize(22);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    QGroupBox *group = new QGroupBox("Select text", page);
    group->setObjectName("groupSelectText");
    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setSpacing(12);

    comboLessons = new QComboBox(group);
    comboLessons->setObjectName("comboLessons");
    comboLessons->setMinimumHeight(32);
    connect(comboLessons, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::loadSelectedLesson);

    labelLessonDescription = new QLabel("Choose a lesson to start training.", group);
    labelLessonDescription->setObjectName("labelLessonDescription");
    labelLessonDescription->setWordWrap(true);

    comboSpeedMetric = new QComboBox(group);
    comboSpeedMetric->setObjectName("comboSpeedMetric");
    comboSpeedMetric->addItem("CPM", "CPM");
    comboSpeedMetric->addItem("WPM", "WPM");
    connect(comboSpeedMetric, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSpeedMetricChanged);

    auto *metricRow = new QHBoxLayout();
    QLabel *metricLabel = new QLabel("Speed metric:", group);
    metricLabel->setObjectName("labelSpeedMetricTitle");
    metricRow->addWidget(metricLabel);
    metricRow->addWidget(comboSpeedMetric);
    metricRow->addStretch();

    auto *buttonRow = new QHBoxLayout();
    btnStart = new QPushButton("Start training", group);
    btnStart->setObjectName("btnStartTraining");
    btnStart->setMinimumHeight(34);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::startTraining);

    btnRandomLesson = new QPushButton("Random lesson", group);
    btnRandomLesson->setObjectName("btnRandomLesson");
    connect(btnRandomLesson, &QPushButton::clicked, this, &MainWindow::chooseRandomLesson);

    btnReloadLessons = new QPushButton("Reload lessons", group);
    btnReloadLessons->setObjectName("btnReloadLessons");
    connect(btnReloadLessons, &QPushButton::clicked, this, &MainWindow::scanLessons);

    buttonRow->addWidget(btnStart);
    buttonRow->addWidget(btnRandomLesson);
    buttonRow->addWidget(btnReloadLessons);

    groupLayout->addWidget(comboLessons);
    groupLayout->addWidget(labelLessonDescription);
    groupLayout->addLayout(metricRow);
    groupLayout->addLayout(buttonRow);

    mainLayout->addStretch();
    mainLayout->addWidget(title);
    mainLayout->addWidget(group);
    mainLayout->addStretch();

    return page;
}

QWidget *MainWindow::buildTrainingPage()
{
    QWidget *page = new QWidget(this);
    page->setObjectName("pageTraining");

    auto *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(12);

    auto *statsLayout = new QHBoxLayout();
    labelTime = new QLabel("Time: 00:00", page);
    labelTime->setObjectName("labelTime");
    labelSpeed = new QLabel("Speed: 0 CPM", page);
    labelSpeed->setObjectName("labelSpeed");
    labelAccuracy = new QLabel("Accuracy: 100%", page);
    labelAccuracy->setObjectName("labelAccuracy");
    progressAccuracy = new QProgressBar(page);
    progressAccuracy->setObjectName("progressAccuracy");
    progressAccuracy->setRange(0, 100);
    progressAccuracy->setValue(100);
    progressAccuracy->setMaximumWidth(180);

    statsLayout->addWidget(labelTime);
    statsLayout->addStretch();
    statsLayout->addWidget(labelSpeed);
    statsLayout->addStretch();
    statsLayout->addWidget(labelAccuracy);
    statsLayout->addWidget(progressAccuracy);

    QGroupBox *textGroup = new QGroupBox("Text", page);
    textGroup->setObjectName("groupTrainingText");
    auto *textLayout = new QVBoxLayout(textGroup);
    textLayout->setSpacing(8);

    labelPreviousLine = new QLabel("", textGroup);
    labelPreviousLine->setObjectName("labelPreviousLine");
    labelCurrentLine = new QLabel("", textGroup);
    labelCurrentLine->setObjectName("labelCurrentLine");
    labelNextLine = new QLabel("", textGroup);
    labelNextLine->setObjectName("labelNextLine");

    QFont mono("Consolas");
    mono.setStyleHint(QFont::Monospace);
    mono.setPointSize(15);
    labelPreviousLine->setFont(mono);
    labelCurrentLine->setFont(mono);
    labelNextLine->setFont(mono);
    labelPreviousLine->setTextFormat(Qt::RichText);
    labelCurrentLine->setTextFormat(Qt::RichText);
    labelNextLine->setTextFormat(Qt::RichText);
    labelPreviousLine->setWordWrap(true);
    labelCurrentLine->setWordWrap(true);
    labelNextLine->setWordWrap(true);
    labelCurrentLine->setMinimumHeight(56);

    textLayout->addWidget(labelPreviousLine);
    textLayout->addWidget(labelCurrentLine);
    textLayout->addWidget(labelNextLine);

    keyboardWidget = buildKeyboard();
    keyboardWidget->setObjectName("keyboardWidget");

    auto *buttonRow = new QHBoxLayout();
    btnRestartTraining = new QPushButton("Restart training", page);
    btnRestartTraining->setObjectName("btnRestartTraining");
    connect(btnRestartTraining, &QPushButton::clicked, this, &MainWindow::restartTraining);

    btnReturnToMain = new QPushButton("Return to main", page);
    btnReturnToMain->setObjectName("btnReturnToMain");
    connect(btnReturnToMain, &QPushButton::clicked, this, &MainWindow::returnToMain);

    btnFinishForTest = new QPushButton("Finish", page);
    btnFinishForTest->setObjectName("btnFinishForTest");
    connect(btnFinishForTest, &QPushButton::clicked, this, &MainWindow::finishSession);

    buttonRow->addWidget(btnRestartTraining);
    buttonRow->addStretch();
    buttonRow->addWidget(btnFinishForTest);
    buttonRow->addWidget(btnReturnToMain);

    mainLayout->addLayout(statsLayout);
    mainLayout->addWidget(textGroup, 1);
    mainLayout->addWidget(keyboardWidget);
    mainLayout->addLayout(buttonRow);

    return page;
}

QWidget *MainWindow::buildResultsPage()
{
    QWidget *page = new QWidget(this);
    page->setObjectName("pageResults");

    auto *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(36, 30, 36, 30);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("Results", page);
    title->setObjectName("labelResultsTitle");
    QFont titleFont = title->font();
    titleFont.setPointSize(22);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    QGroupBox *group = new QGroupBox("Session summary", page);
    group->setObjectName("groupResults");
    auto *grid = new QGridLayout(group);

    labelResultTime = new QLabel("00:00", group);
    labelResultTime->setObjectName("labelResultTime");
    labelResultSpeed = new QLabel("0 CPM", group);
    labelResultSpeed->setObjectName("labelResultSpeed");
    labelResultAccuracy = new QLabel("100%", group);
    labelResultAccuracy->setObjectName("labelResultAccuracy");

    grid->addWidget(new QLabel("Time:", group), 0, 0);
    grid->addWidget(labelResultTime, 0, 1);
    grid->addWidget(new QLabel("Speed:", group), 1, 0);
    grid->addWidget(labelResultSpeed, 1, 1);
    grid->addWidget(new QLabel("Accuracy:", group), 2, 0);
    grid->addWidget(labelResultAccuracy, 2, 1);

    auto *buttonRow = new QHBoxLayout();
    btnResultsRestart = new QPushButton("Restart training", page);
    btnResultsRestart->setObjectName("btnResultsRestart");
    connect(btnResultsRestart, &QPushButton::clicked, this, &MainWindow::restartTraining);
    btnResultsReturn = new QPushButton("Return to main", page);
    btnResultsReturn->setObjectName("btnResultsReturn");
    connect(btnResultsReturn, &QPushButton::clicked, this, &MainWindow::returnToMain);
    buttonRow->addWidget(btnResultsRestart);
    buttonRow->addWidget(btnResultsReturn);

    mainLayout->addStretch();
    mainLayout->addWidget(title);
    mainLayout->addWidget(group);
    mainLayout->addLayout(buttonRow);
    mainLayout->addStretch();

    return page;
}

QWidget *MainWindow::buildKeyboard()
{
    QWidget *widget = new QWidget(this);
    auto *grid = new QGridLayout(widget);
    grid->setSpacing(5);

    const QStringList rows = {
        "Q W E R T Y U I O P",
        "A S D F G H J K L",
        "Z X C V B N M"
    };

    int rowNumber = 0;
    for (const QString &row : rows) {
        const QStringList keys = row.split(' ', Qt::SkipEmptyParts);
        int offset = rowNumber;
        for (int i = 0; i < keys.size(); ++i) {
            QString key = keys[i];
            QPushButton *button = new QPushButton(key, widget);
            button->setObjectName("key_" + key);
            button->setFocusPolicy(Qt::NoFocus);
            button->setMinimumHeight(34);
            grid->addWidget(button, rowNumber, i + offset);
            keyButtons.insert(key, button);
        }
        rowNumber++;
    }

    QPushButton *space = new QPushButton("Space", widget);
    space->setObjectName("key_SPACE");
    space->setFocusPolicy(Qt::NoFocus);
    space->setMinimumHeight(34);
    grid->addWidget(space, rowNumber, 3, 1, 5);
    keyButtons.insert(" ", space);
    keyButtons.insert("SPACE", space);

    return widget;
}

QString MainWindow::lessonsDirectory() const
{
    const QString appLessons = QCoreApplication::applicationDirPath() + "/lessons";
    if (QDir(appLessons).exists()) {
        return appLessons;
    }
    return QDir::currentPath() + "/lessons";
}

void MainWindow::scanLessons()
{
    const QString previousPath = comboLessons ? comboLessons->currentData().toString() : QString();

    QSignalBlocker blocker(comboLessons);
    comboLessons->clear();

    QDir dir(lessonsDirectory());
    if (!dir.exists()) {
        labelLessonDescription->setText("Folder lessons/ was not found. Create it near the executable or in project root.");
        btnStart->setEnabled(false);
        btnRandomLesson->setEnabled(false);
        actionRandomLesson->setEnabled(false);
        statusBar()->showMessage("No lessons folder", 3000);
        return;
    }

    const QStringList files = dir.entryList(QStringList() << "*.txt", QDir::Files, QDir::Name);
    for (const QString &fileName : files) {
        const QString path = dir.filePath(fileName);
        QFileInfo info(path);
        const QString title = info.baseName();
        comboLessons->addItem(title, info.absoluteFilePath());
        comboLessons->setItemData(comboLessons->count() - 1,
                                  QString("File: %1, size: %2 bytes").arg(info.fileName()).arg(info.size()),
                                  Qt::ToolTipRole);
    }

    bool hasLessons = comboLessons->count() > 0;
    btnStart->setEnabled(hasLessons);
    btnRandomLesson->setEnabled(hasLessons);
    actionRandomLesson->setEnabled(hasLessons);

    if (!hasLessons) {
        labelLessonDescription->setText("No .txt lessons found in lessons/.");
        statusBar()->showMessage("No lessons found", 3000);
        return;
    }

    int restoreIndex = comboLessons->findData(previousPath);
    if (restoreIndex < 0 && !currentLessonPath.isEmpty()) {
        restoreIndex = comboLessons->findData(currentLessonPath);
    }
    comboLessons->setCurrentIndex(restoreIndex >= 0 ? restoreIndex : 0);
    loadSelectedLesson();
    statusBar()->showMessage(QString("Loaded list: %1 lessons").arg(comboLessons->count()), 2500);
}

void MainWindow::loadSelectedLesson()
{
    if (!comboLessons || comboLessons->currentIndex() < 0) {
        return;
    }
    const QString path = comboLessons->currentData().toString();
    if (!loadLessonFromPath(path)) {
        return;
    }
    saveSettings();
}

bool MainWindow::loadLessonFromPath(const QString &path)
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
    text.replace("\r", "\n");

    currentLessonPath = QFileInfo(path).absoluteFilePath();
    currentLessonTitle = QFileInfo(path).baseName();
    fullText = text;
    lines = text.split('\n');
    if (lines.isEmpty()) {
        lines << QString();
    }

    resetSession();
    labelLessonDescription->setText(QString("Selected: %1 (%2 lines)").arg(currentLessonTitle).arg(lines.size()));
    return true;
}

void MainWindow::chooseRandomLesson()
{
    const int n = comboLessons->count();
    if (n <= 0) {
        statusBar()->showMessage("No lessons for random choice", 2500);
        return;
    }
    const int index = QRandomGenerator::global()->bounded(n);
    comboLessons->setCurrentIndex(index);
    statusBar()->showMessage("Random lesson selected", 2000);
}

void MainWindow::startTraining()
{
    if (comboLessons->count() <= 0 || fullText.isEmpty()) {
        QMessageBox::information(this, "No lesson", "Choose or reload a non-empty lesson first.");
        return;
    }

    resetSession();
    sessionActive = true;
    elapsedTimer.start();
    sessionTimer.start();
    stackScreens->setCurrentIndex(TrainingScreen);
    updateTrainingView();
    updateStatsLabels();
    pageTraining->setFocus(Qt::OtherFocusReason);
    statusBar()->showMessage("Training started", 2000);
}

void MainWindow::restartTraining()
{
    sessionTimer.stop();
    resetSession();
    sessionActive = true;
    elapsedTimer.start();
    sessionTimer.start();
    stackScreens->setCurrentIndex(TrainingScreen);
    updateTrainingView();
    updateStatsLabels();
    pageTraining->setFocus(Qt::OtherFocusReason);
    statusBar()->showMessage("Training restarted", 2000);
}

void MainWindow::returnToMain()
{
    sessionTimer.stop();
    sessionActive = false;
    clearVirtualKeyHighlight();
    stackScreens->setCurrentIndex(StartScreen);
    statusBar()->showMessage("Returned to main screen", 2000);
}

void MainWindow::resetSession()
{
    lineIndex = 0;
    charIndex = 0;
    totalTyped = 0;
    correctTyped = 0;
    finalElapsedMs = 0;
    sessionActive = false;
    clearVirtualKeyHighlight();

    charStates.clear();
    for (const QString &line : lines) {
        charStates.append(QVector<int>(line.size(), 0));
    }
    if (charStates.isEmpty()) {
        charStates.append(QVector<int>());
    }

    updateTrainingView();
    updateStatsLabels();
}

void MainWindow::finishSession()
{
    if (!sessionActive && stackScreens->currentIndex() == ResultsScreen) {
        return;
    }
    finalElapsedMs = elapsedTimer.isValid() ? elapsedTimer.elapsed() : finalElapsedMs;
    sessionTimer.stop();
    sessionActive = false;
    updateResultsView();
    clearVirtualKeyHighlight();
    stackScreens->setCurrentIndex(ResultsScreen);
    statusBar()->showMessage("Session finished", 2500);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == pageTraining && event->type() == QEvent::KeyPress && sessionActive) {
        auto *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Backspace) {
            handleBackspace();
            return true;
        }
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            handleEnter();
            return true;
        }

        const QString typed = keyEvent->text();
        if (typed.isEmpty()) {
            return true;
        }
        handleTypedText(typed.left(1));
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::handleTypedText(const QString &text)
{
    if (lines.isEmpty() || lineIndex >= lines.size()) {
        finishSession();
        return;
    }

    const QString expected = currentExpectedChar();
    if (expected.isEmpty()) {
        if (text == "\n") {
            handleEnter();
        }
        return;
    }

    const bool correct = (text == expected);
    if (lineIndex >= 0 && lineIndex < charStates.size() && charIndex >= 0 && charIndex < charStates[lineIndex].size()) {
        charStates[lineIndex][charIndex] = correct ? 1 : 2;
    }

    totalTyped++;
    if (correct) {
        correctTyped++;
    }

    highlightVirtualKey(text);
    charIndex++;

    if (lineIndex < lines.size() && charIndex >= lines[lineIndex].size()) {
        // User may press Enter, or we move automatically if this is the final line.
        if (lineIndex == lines.size() - 1) {
            updateTrainingView();
            updateStatsLabels();
            finishSession();
            return;
        }
    }

    updateTrainingView();
    updateStatsLabels();
}

void MainWindow::handleBackspace()
{
    if (lineIndex == 0 && charIndex == 0) {
        return;
    }

    if (charIndex > 0) {
        charIndex--;
    } else if (lineIndex > 0) {
        lineIndex--;
        charIndex = lines[lineIndex].size();
    }

    if (lineIndex >= 0 && lineIndex < charStates.size() && charIndex >= 0 && charIndex < charStates[lineIndex].size()) {
        int previousState = charStates[lineIndex][charIndex];
        if (previousState != 0 && totalTyped > 0) {
            totalTyped--;
            if (previousState == 1 && correctTyped > 0) {
                correctTyped--;
            }
        }
        charStates[lineIndex][charIndex] = 0;
    }

    highlightVirtualKey("BACKSPACE");
    updateTrainingView();
    updateStatsLabels();
}

void MainWindow::handleEnter()
{
    highlightVirtualKey("ENTER");
    if (lines.isEmpty()) {
        return;
    }

    if (lineIndex < lines.size() && charIndex >= lines[lineIndex].size()) {
        if (lineIndex < lines.size() - 1) {
            lineIndex++;
            charIndex = 0;
        } else {
            finishSession();
            return;
        }
    }

    updateTrainingView();
    updateStatsLabels();
}

QString MainWindow::currentExpectedChar() const
{
    if (lineIndex < 0 || lineIndex >= lines.size()) {
        return QString();
    }
    const QString line = lines[lineIndex];
    if (charIndex < 0 || charIndex >= line.size()) {
        return QString();
    }
    return line.mid(charIndex, 1);
}

void MainWindow::updateTrainingView()
{
    QString previous;
    if (lineIndex > 0 && lineIndex - 1 < lines.size()) {
        previous = styleSpan(lines[lineIndex - 1], "color:#888888;");
    } else {
        previous = styleSpan("Previous line will appear here", "color:#aaaaaa;");
    }
    labelPreviousLine->setText(previous);

    if (lines.isEmpty() || lineIndex >= lines.size()) {
        labelCurrentLine->setText(styleSpan("No text loaded", "color:#cc0000;"));
        labelNextLine->setText("");
        return;
    }

    const QString line = lines[lineIndex];
    QString html;
    for (int i = 0; i < line.size(); ++i) {
        const QString ch = line.mid(i, 1);
        int state = 0;
        if (lineIndex < charStates.size() && i < charStates[lineIndex].size()) {
            state = charStates[lineIndex][i];
        }

        if (i == charIndex) {
            html += styleSpan(ch.isEmpty() ? " " : ch, "background:#ffe08a; color:#000000; border-bottom:2px solid #000;");
        } else if (state == 1) {
            html += styleSpan(ch, "background:#c8f7c5; color:#0b5d1e;");
        } else if (state == 2) {
            html += styleSpan(ch, "background:#ffb3b3; color:#8a0000; text-decoration:underline;");
        } else {
            html += htmlEscapedWithSpaces(ch);
        }
    }

    if (charIndex >= line.size()) {
        html += styleSpan(" ↵", "background:#e8e8e8; color:#333333;");
    }
    labelCurrentLine->setText(html);

    if (lineIndex + 1 < lines.size()) {
        labelNextLine->setText(styleSpan(lines[lineIndex + 1], "color:#777777;"));
    } else {
        labelNextLine->setText(styleSpan("End of lesson", "color:#aaaaaa;"));
    }
}

QString MainWindow::htmlEscapedWithSpaces(const QString &text) const
{
    QString escaped = text.toHtmlEscaped();
    escaped.replace(" ", "&nbsp;");
    if (escaped.isEmpty()) {
        escaped = "&nbsp;";
    }
    return escaped;
}

QString MainWindow::styleSpan(const QString &text, const QString &style) const
{
    return "<span style='" + style + "'>" + htmlEscapedWithSpaces(text) + "</span>";
}

void MainWindow::highlightVirtualKey(const QString &keyText)
{
    clearVirtualKeyHighlight();

    QString key = keyText;
    if (key == " ") {
        key = "SPACE";
    } else {
        key = key.toUpper();
    }

    QPushButton *button = keyButtons.value(key, nullptr);
    if (!button && key.size() == 1) {
        button = keyButtons.value(key.left(1), nullptr);
    }
    if (!button) {
        return;
    }

    activeKeyButton = button;
    activeKeyButton->setStyleSheet("background-color: #ffe08a; font-weight: bold; border: 2px solid #cc9900;");
}

void MainWindow::clearVirtualKeyHighlight()
{
    if (activeKeyButton) {
        activeKeyButton->setStyleSheet("");
        activeKeyButton = nullptr;
    }
}

void MainWindow::onTimerTick()
{
    updateStatsLabels();
}

QString MainWindow::formatTime(qint64 milliseconds) const
{
    qint64 totalSeconds = milliseconds / 1000;
    qint64 minutes = totalSeconds / 60;
    qint64 seconds = totalSeconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

double MainWindow::currentSpeed() const
{
    qint64 ms = sessionActive && elapsedTimer.isValid() ? elapsedTimer.elapsed() : finalElapsedMs;
    if (ms <= 0 || totalTyped <= 0) {
        return 0.0;
    }

    const double minutes = static_cast<double>(ms) / 60000.0;
    double cpm = static_cast<double>(totalTyped) / minutes;
    if (comboSpeedMetric && comboSpeedMetric->currentData().toString() == "WPM") {
        return cpm / 5.0;
    }
    return cpm;
}

double MainWindow::currentAccuracy() const
{
    if (totalTyped <= 0) {
        return 100.0;
    }
    return (static_cast<double>(correctTyped) / static_cast<double>(totalTyped)) * 100.0;
}

QString MainWindow::speedUnit() const
{
    return comboSpeedMetric && comboSpeedMetric->currentData().toString() == "WPM" ? "WPM" : "CPM";
}

void MainWindow::updateStatsLabels()
{
    qint64 ms = sessionActive && elapsedTimer.isValid() ? elapsedTimer.elapsed() : finalElapsedMs;
    labelTime->setText("Time: " + formatTime(ms));

    double speed = currentSpeed();
    double accuracy = currentAccuracy();
    labelSpeed->setText(QString("Speed: %1 %2").arg(QString::number(speed, 'f', 1), speedUnit()));
    labelAccuracy->setText(QString("Accuracy: %1%").arg(QString::number(accuracy, 'f', 1)));
    progressAccuracy->setValue(qBound(0, static_cast<int>(accuracy + 0.5), 100));
}

void MainWindow::updateResultsView()
{
    labelResultTime->setText(formatTime(finalElapsedMs));
    labelResultSpeed->setText(QString("%1 %2").arg(QString::number(currentSpeed(), 'f', 1), speedUnit()));
    labelResultAccuracy->setText(QString("%1%").arg(QString::number(currentAccuracy(), 'f', 1)));
}

void MainWindow::onSpeedMetricChanged(int)
{
    updateStatsLabels();
    saveSettings();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About TypingTrainer",
                       "TypingTrainer\n\nKeyboard events, text highlighting, virtual keyboard, QTimer metrics and QSettings.");
    if (stackScreens->currentIndex() == TrainingScreen) {
        pageTraining->setFocus(Qt::OtherFocusReason);
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    const QString metric = settings.value("ui/speedMetric", "CPM").toString();
    int metricIndex = comboSpeedMetric->findData(metric);
    if (metricIndex >= 0) {
        comboSpeedMetric->setCurrentIndex(metricIndex);
    }
    currentLessonPath = settings.value("lesson/lastPath", QString()).toString();
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("ui/speedMetric", comboSpeedMetric->currentData().toString());
    settings.setValue("lesson/lastPath", currentLessonPath);
}

void MainWindow::restoreLastLessonSelection()
{
    if (currentLessonPath.isEmpty() || comboLessons->count() == 0) {
        return;
    }
    const int index = comboLessons->findData(currentLessonPath);
    if (index >= 0) {
        comboLessons->setCurrentIndex(index);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sessionTimer.stop();
    saveSettings();
    event->accept();
}
