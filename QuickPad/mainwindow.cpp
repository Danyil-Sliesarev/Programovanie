#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCursor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentDocumentName("Untitled")
    , modified(false)
{
    ui->setupUi(this);
    setupInitialState();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupInitialState()
{
    setWindowTitle("QuickPad");
    setDocumentTitle("Untitled");
    ui->textEditor->setPlainText("");
    ui->statusLabelPosition->setText("Line 1, Col 1");
    ui->statusLabelModified->setText("Saved");
    statusBar()->showMessage("Ready", 2000);
}

void MainWindow::setupConnections()
{
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openDocumentDialog);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveDocumentDialog);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsDocumentDialog);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::exitApplication);

    connect(ui->actionCut, &QAction::triggered, ui->textEditor, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEditor, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEditor, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->textEditor, &QPlainTextEdit::selectAll);
    connect(ui->actionUndo, &QAction::triggered, ui->textEditor, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEditor, &QPlainTextEdit::redo);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::findDialog);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    connect(ui->textEditor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
    connect(ui->textEditor, &QPlainTextEdit::textChanged, this, &MainWindow::markModified);
}

void MainWindow::setDocumentTitle(const QString &title)
{
    currentDocumentName = title.trimmed().isEmpty() ? "Untitled" : title.trimmed();
    ui->statusLabelDocument->setText(currentDocumentName + (modified ? "*" : ""));
}

void MainWindow::newDocument()
{
    bool ok = false;
    const QString name = QInputDialog::getText(this,
                                               "New document",
                                               "Document name:",
                                               QLineEdit::Normal,
                                               "Untitled",
                                               &ok);

    if (!ok || name.trimmed().isEmpty()) {
        statusBar()->showMessage("New document canceled", 2000);
        return;
    }

    ui->textEditor->clear();
    modified = false;
    setDocumentTitle(name);
    ui->statusLabelModified->setText("Saved");
    statusBar()->showMessage("New document created", 2000);
}

void MainWindow::openDocumentDialog()
{
    const QString path = QFileDialog::getOpenFileName(this,
                                                      "Open file",
                                                      QString(),
                                                      "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        statusBar()->showMessage("Open canceled", 2000);
        return;
    }

    setDocumentTitle(path.section('/', -1));
    modified = false;
    ui->statusLabelModified->setText("Saved");
    statusBar()->showMessage("Selected file: " + path, 3000);
}

void MainWindow::saveDocumentDialog()
{
    modified = false;
    setDocumentTitle(currentDocumentName);
    ui->statusLabelModified->setText("Saved");
    statusBar()->showMessage("Save action prepared", 2000);
}

void MainWindow::saveAsDocumentDialog()
{
    const QString path = QFileDialog::getSaveFileName(this,
                                                      "Save as",
                                                      currentDocumentName + ".txt",
                                                      "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        statusBar()->showMessage("Save As canceled", 2000);
        return;
    }

    setDocumentTitle(path.section('/', -1));
    modified = false;
    ui->statusLabelModified->setText("Saved");
    statusBar()->showMessage("Selected save path: " + path, 3000);
}

void MainWindow::exitApplication()
{
    const QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        "Exit",
        "Close QuickPad?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        close();
    } else {
        statusBar()->showMessage("Exit canceled", 2000);
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this,
                       "About QuickPad",
                       "QuickPad\n\nA simple Qt Widgets text editor UI prototype.");
}

void MainWindow::findDialog()
{
    bool ok = false;
    const QString query = QInputDialog::getText(this,
                                                "Find",
                                                "Text to find:",
                                                QLineEdit::Normal,
                                                QString(),
                                                &ok);
    if (!ok || query.trimmed().isEmpty()) {
        statusBar()->showMessage("Find canceled", 2000);
        return;
    }

    statusBar()->showMessage("Find text: " + query, 3000);
}

void MainWindow::updateCursorPosition()
{
    const QTextCursor cursor = ui->textEditor->textCursor();
    const int line = cursor.blockNumber() + 1;
    const int column = cursor.positionInBlock() + 1;
    ui->statusLabelPosition->setText(QString("Line %1, Col %2").arg(line).arg(column));
}

void MainWindow::markModified()
{
    if (!modified) {
        modified = true;
        ui->statusLabelModified->setText("Modified");
        setDocumentTitle(currentDocumentName);
    }
}
