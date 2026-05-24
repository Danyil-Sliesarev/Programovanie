#pragma once
#include "databasemanager.h"
#include "accountrepository.h"
#include "accounttablemodel.h"
#include "accountfilterproxymodel.h"
#include "passwordleakchecker.h"
#include "batchchecker.h"

#include <QMainWindow>
#include <QFutureWatcher>

class QAction;
class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QTableView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void newEntry();
    void editEntry();
    void deleteEntry();
    void saveData();
    void checkSelectedPassword();
    void checkAllPasswords();
    void copyUsername();
    void copyPassword();
    void updateUiState();
    void reloadTable();

private:
    void buildUi();
    void setupActions();
    void setupDatabase();
    void setupConnections();
    QModelIndex currentSourceIndex() const;
    AccountEntry currentEntry() const;
    void updateCategoryFilterItems();
    void updateCounts();
    void setBusy(bool busy);
    void showError(const QString &title, const QString &message);

    QTableView *m_table = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QComboBox *m_categoryCombo = nullptr;
    QLabel *m_emptyLabel = nullptr;
    QLabel *m_statusLeft = nullptr;
    QLabel *m_statusRight = nullptr;
    QProgressBar *m_progressBar = nullptr;

    QAction *m_actionNew = nullptr;
    QAction *m_actionEdit = nullptr;
    QAction *m_actionDelete = nullptr;
    QAction *m_actionSave = nullptr;
    QAction *m_actionExit = nullptr;
    QAction *m_actionCopyUsername = nullptr;
    QAction *m_actionCopyPassword = nullptr;
    QAction *m_actionCheckPassword = nullptr;
    QAction *m_actionCheckAll = nullptr;
    QAction *m_actionAbout = nullptr;

    DatabaseManager m_dbManager;
    AccountRepository m_repository;
    AccountTableModel *m_model = nullptr;
    AccountFilterProxyModel *m_proxy = nullptr;
    PasswordLeakChecker *m_checker = nullptr;
    QFutureWatcher<BatchCheckResult> *m_batchWatcher = nullptr;
};
