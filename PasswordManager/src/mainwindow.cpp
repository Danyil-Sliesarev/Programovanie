#include "mainwindow.h"
#include <QtConcurrent>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeySequence>
#include <QAbstractItemView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    buildUi();
    setupActions();
    setupDatabase();
    setupConnections();
    reloadTable();
}

void MainWindow::buildUi()
{
    setWindowTitle("Password Manager");
    resize(950, 560);

    QWidget *central = new QWidget(this);
    central->setObjectName("centralWidget");
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    auto *filterLayout = new QHBoxLayout;
    auto *categoryLabel = new QLabel("Category:", central);
    m_categoryCombo = new QComboBox(central);
    m_categoryCombo->setObjectName("comboCategoryFilter");
    m_categoryCombo->setMinimumWidth(180);
    m_searchEdit = new QLineEdit(central);
    m_searchEdit->setObjectName("editSearch");
    m_searchEdit->setPlaceholderText("Search...");
    QPushButton *clearButton = new QPushButton("Clear", central);
    clearButton->setObjectName("btnClearSearch");
    filterLayout->addWidget(categoryLabel);
    filterLayout->addWidget(m_categoryCombo);
    filterLayout->addWidget(m_searchEdit, 1);
    filterLayout->addWidget(clearButton);
    mainLayout->addLayout(filterLayout);

    m_table = new QTableView(central);
    m_table->setObjectName("tableViewAccounts");
    m_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    m_table->setAlternatingRowColors(true);
    m_table->setSortingEnabled(true);
    mainLayout->addWidget(m_table, 1);

    m_emptyLabel = new QLabel("No records found", central);
    m_emptyLabel->setObjectName("labelEmptyState");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setVisible(false);
    mainLayout->addWidget(m_emptyLabel);

    setCentralWidget(central);

    m_statusLeft = new QLabel("Ready", this);
    m_statusRight = new QLabel("Total: 0    Filtered: 0", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("progressBatchCheck");
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(180);
    statusBar()->addWidget(m_statusLeft, 1);
    statusBar()->addPermanentWidget(m_progressBar);
    statusBar()->addPermanentWidget(m_statusRight);

    connect(clearButton, &QPushButton::clicked, this, [this]() { m_searchEdit->clear(); });
}

void MainWindow::setupActions()
{
    m_actionNew = new QAction("New Entry", this);
    m_actionNew->setObjectName("actionNew");
    m_actionNew->setShortcut(QKeySequence::New);

    m_actionEdit = new QAction("Edit", this);
    m_actionEdit->setObjectName("actionEdit");
    m_actionEdit->setShortcut(Qt::Key_F2);

    m_actionDelete = new QAction("Delete", this);
    m_actionDelete->setObjectName("actionDelete");
    m_actionDelete->setShortcut(QKeySequence::Delete);

    m_actionSave = new QAction("Save", this);
    m_actionSave->setObjectName("actionSave");
    m_actionSave->setShortcut(QKeySequence::Save);

    m_actionExit = new QAction("Exit", this);
    m_actionExit->setObjectName("actionExit");
    m_actionExit->setShortcut(QKeySequence::Quit);

    m_actionCopyUsername = new QAction("Copy Username", this);
    m_actionCopyUsername->setObjectName("actionCopyUsername");

    m_actionCopyPassword = new QAction("Copy Password", this);
    m_actionCopyPassword->setObjectName("actionCopyPassword");

    m_actionCheckPassword = new QAction("Check Password", this);
    m_actionCheckPassword->setObjectName("actionCheckPassword");

    m_actionCheckAll = new QAction("Check All Passwords", this);
    m_actionCheckAll->setObjectName("actionCheckAllPasswords");

    m_actionAbout = new QAction("About", this);
    m_actionAbout->setObjectName("actionAbout");

    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(m_actionNew);
    fileMenu->addAction(m_actionSave);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionExit);

    QMenu *entryMenu = menuBar()->addMenu("Entry");
    entryMenu->addAction(m_actionEdit);
    entryMenu->addAction(m_actionDelete);
    entryMenu->addSeparator();
    entryMenu->addAction(m_actionCopyUsername);
    entryMenu->addAction(m_actionCopyPassword);
    entryMenu->addSeparator();
    entryMenu->addAction(m_actionCheckPassword);
    entryMenu->addAction(m_actionCheckAll);

    QMenu *toolsMenu = menuBar()->addMenu("Tools");
    toolsMenu->addAction(m_actionCheckAll);

    QMenu *helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(m_actionAbout);

    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->setObjectName("toolBarMain");
    toolbar->addAction(m_actionNew);
    toolbar->addAction(m_actionEdit);
    toolbar->addAction(m_actionDelete);
    toolbar->addSeparator();
    toolbar->addAction(m_actionCopyUsername);
    toolbar->addAction(m_actionCopyPassword);
    toolbar->addSeparator();
    toolbar->addAction(m_actionCheckPassword);
    toolbar->addAction(m_actionCheckAll);
}

void MainWindow::setupDatabase()
{
    const QString dbPath = QCoreApplication::applicationDirPath() + QDir::separator() + "password_manager.sqlite";
    if (!m_dbManager.open(dbPath) || !m_dbManager.initializeSchema()) {
        showError("Database error", m_dbManager.lastError());
    }
    m_repository.setDatabase(m_dbManager.database());

    m_model = new AccountTableModel(this);
    m_model->setObjectName("modelAccounts");
    m_model->setRepository(&m_repository);

    m_proxy = new AccountFilterProxyModel(this);
    m_proxy->setObjectName("proxyAccounts");
    m_proxy->setSourceModel(m_model);

    m_table->setModel(m_proxy);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->setColumnHidden(AccountTableModel::Id, true);

    m_checker = new PasswordLeakChecker(this);
    m_batchWatcher = new QFutureWatcher<BatchCheckResult>(this);
}

void MainWindow::setupConnections()
{
    connect(m_actionNew, &QAction::triggered, this, &MainWindow::newEntry);
    connect(m_actionEdit, &QAction::triggered, this, &MainWindow::editEntry);
    connect(m_actionDelete, &QAction::triggered, this, &MainWindow::deleteEntry);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::saveData);
    connect(m_actionExit, &QAction::triggered, this, &QWidget::close);
    connect(m_actionCopyUsername, &QAction::triggered, this, &MainWindow::copyUsername);
    connect(m_actionCopyPassword, &QAction::triggered, this, &MainWindow::copyPassword);
    connect(m_actionCheckPassword, &QAction::triggered, this, &MainWindow::checkSelectedPassword);
    connect(m_actionCheckAll, &QAction::triggered, this, &MainWindow::checkAllPasswords);
    connect(m_actionAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About Password Manager",
            "Password Manager\n\nQt Widgets training project: QTableView, SQLite, filtering, network checks and background processing.");
    });

    connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_proxy->setTextFilter(text);
        updateCounts();
    });
    connect(m_categoryCombo, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        m_proxy->setCategoryFilter(text);
        updateCounts();
    });
    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateUiState);
    connect(m_proxy, &QAbstractItemModel::rowsInserted, this, &MainWindow::updateCounts);
    connect(m_proxy, &QAbstractItemModel::rowsRemoved, this, &MainWindow::updateCounts);
    connect(m_proxy, &QAbstractItemModel::modelReset, this, &MainWindow::updateCounts);
    connect(m_proxy, &QAbstractItemModel::layoutChanged, this, &MainWindow::updateCounts);

    connect(m_checker, &PasswordLeakChecker::checkStarted, this, [this]() {
        m_statusLeft->setText("Checking selected password...");
        setBusy(true);
    });
    connect(m_checker, &PasswordLeakChecker::checkCompleted, this, [this](bool, int, const QString &message) {
        const AccountEntry entry = currentEntry();
        if (entry.id > 0) {
            m_repository.updateLeakStatus(entry.id, message);
            m_model->updateLeakStatusById(entry.id, message);
        }
        m_statusLeft->setText(message);
        setBusy(false);
    });
    connect(m_checker, &PasswordLeakChecker::checkFailed, this, [this](const QString &message) {
        m_statusLeft->setText("Check failed");
        setBusy(false);
        showError("Password check failed", message);
    });

    connect(m_batchWatcher, &QFutureWatcher<BatchCheckResult>::finished, this, [this]() {
        const BatchCheckResult result = m_batchWatcher->result();
        for (const auto &pair : result.statuses) {
            m_repository.updateLeakStatus(pair.first, pair.second);
            m_model->updateLeakStatusById(pair.first, pair.second);
        }
        m_progressBar->setVisible(false);
        setBusy(false);
        m_statusLeft->setText(QString("Batch finished: %1 checked, %2 compromised, %3 failed")
                              .arg(result.checked).arg(result.compromised).arg(result.failed));
        reloadTable();
    });
}

void MainWindow::reloadTable()
{
    m_model->setItems(m_repository.loadAll());
    updateCategoryFilterItems();
    updateCounts();
    updateUiState();
}

void MainWindow::updateCategoryFilterItems()
{
    const QString current = m_categoryCombo->currentText();
    QStringList categories;
    for (const auto &item : m_model->items()) {
        if (!item.category.trimmed().isEmpty() && !categories.contains(item.category, Qt::CaseInsensitive)) {
            categories.append(item.category);
        }
    }
    categories.sort(Qt::CaseInsensitive);

    m_categoryCombo->blockSignals(true);
    m_categoryCombo->clear();
    m_categoryCombo->addItem("All");
    m_categoryCombo->addItems(categories);
    const int idx = m_categoryCombo->findText(current);
    m_categoryCombo->setCurrentIndex(idx >= 0 ? idx : 0);
    m_categoryCombo->blockSignals(false);
    m_proxy->setCategoryFilter(m_categoryCombo->currentText());
}

QModelIndex MainWindow::currentSourceIndex() const
{
    const QModelIndex proxyIndex = m_table->currentIndex();
    if (!proxyIndex.isValid()) return {};
    return m_proxy->mapToSource(proxyIndex);
}

AccountEntry MainWindow::currentEntry() const
{
    const QModelIndex source = currentSourceIndex();
    if (!source.isValid()) return {};
    return m_model->itemAt(source.row());
}

void MainWindow::newEntry()
{
    AccountEntry entry;
    entry.title = "New entry";
    entry.category = "General";
    entry.leakStatus = "Not checked";
    if (!m_repository.insert(entry)) {
        showError("Insert failed", m_repository.lastError());
        return;
    }
    reloadTable();
    for (int row = 0; row < m_proxy->rowCount(); ++row) {
        const QModelIndex sourceIdx = m_proxy->mapToSource(m_proxy->index(row, 0));
        if (m_model->idAt(sourceIdx.row()) == entry.id) {
            const QModelIndex editProxy = m_proxy->index(row, AccountTableModel::Title);
            m_table->selectRow(row);
            m_table->setCurrentIndex(editProxy);
            m_table->edit(editProxy);
            break;
        }
    }
    m_statusLeft->setText("New entry added");
}

void MainWindow::editEntry()
{
    QModelIndex idx = m_table->currentIndex();
    if (!idx.isValid()) return;
    if (idx.column() == AccountTableModel::Id || idx.column() == AccountTableModel::UpdatedAt || idx.column() == AccountTableModel::LeakStatus) {
        idx = m_proxy->index(idx.row(), AccountTableModel::Title);
    }
    m_table->setCurrentIndex(idx);
    m_table->edit(idx);
}

void MainWindow::deleteEntry()
{
    const AccountEntry entry = currentEntry();
    if (entry.id <= 0) return;
    const auto answer = QMessageBox::question(this, "Delete record", "Delete selected record?",
                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;
    if (!m_repository.remove(entry.id)) {
        showError("Delete failed", m_repository.lastError());
        return;
    }
    reloadTable();
    m_statusLeft->setText("Record deleted");
}

void MainWindow::saveData()
{
    reloadTable();
    m_statusLeft->setText("Saved and reloaded from SQLite");
}

void MainWindow::copyUsername()
{
    QApplication::clipboard()->setText(currentEntry().username);
    m_statusLeft->setText("Username copied");
}

void MainWindow::copyPassword()
{
    QApplication::clipboard()->setText(currentEntry().password);
    m_statusLeft->setText("Password copied");
}

void MainWindow::checkSelectedPassword()
{
    const AccountEntry entry = currentEntry();
    if (entry.id <= 0) return;
    m_checker->checkPassword(entry.password);
}

void MainWindow::checkAllPasswords()
{
    if (m_batchWatcher->isRunning()) return;
    const QList<AccountEntry> items = m_model->items();
    if (items.isEmpty()) {
        m_statusLeft->setText("No records to check");
        return;
    }

    setBusy(true);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, items.size());
    m_progressBar->setValue(0);
    m_statusLeft->setText("Batch checking passwords...");

    auto progress = [this](int checked, int total, int id, const QString &status) {
        QMetaObject::invokeMethod(this, [this, checked, total, id, status]() {
            m_progressBar->setRange(0, total);
            m_progressBar->setValue(checked);
            m_model->updateLeakStatusById(id, status);
            m_statusLeft->setText(QString("Checking passwords: %1/%2").arg(checked).arg(total));
        }, Qt::QueuedConnection);
    };

    m_batchWatcher->setFuture(QtConcurrent::run([items, progress]() {
        return BatchChecker::checkAll(items, progress);
    }));
}

void MainWindow::updateUiState()
{
    const bool hasSelection = currentSourceIndex().isValid();
    m_actionEdit->setEnabled(hasSelection);
    m_actionDelete->setEnabled(hasSelection);
    m_actionCopyUsername->setEnabled(hasSelection);
    m_actionCopyPassword->setEnabled(hasSelection);
    m_actionCheckPassword->setEnabled(hasSelection);
}

void MainWindow::updateCounts()
{
    const int total = m_model ? m_model->rowCount() : 0;
    const int filtered = m_proxy ? m_proxy->rowCount() : 0;
    m_statusRight->setText(QString("Total: %1    Filtered: %2").arg(total).arg(filtered));
    m_emptyLabel->setVisible(filtered == 0);
    updateUiState();
}

void MainWindow::setBusy(bool busy)
{
    m_actionCheckPassword->setEnabled(!busy && currentSourceIndex().isValid());
    m_actionCheckAll->setEnabled(!busy);
}

void MainWindow::showError(const QString &title, const QString &message)
{
    QMessageBox::warning(this, title, message.isEmpty() ? "Unknown error" : message);
}
