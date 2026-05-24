#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>

bool DatabaseManager::open(const QString &filePath)
{
    const QString connectionName = "password_manager_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_db.setDatabaseName(filePath);
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::initializeSchema()
{
    QSqlQuery query(m_db);
    const bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL DEFAULT '',"
        "username TEXT DEFAULT '',"
        "password TEXT DEFAULT '',"
        "website TEXT DEFAULT '',"
        "category TEXT DEFAULT '',"
        "notes TEXT DEFAULT '',"
        "updated_at TEXT DEFAULT '',"
        "leak_status TEXT DEFAULT 'Not checked'"
        ")"
    );
    if (!ok) {
        m_lastError = query.lastError().text();
    }
    return ok;
}

QSqlDatabase DatabaseManager::database() const { return m_db; }
QString DatabaseManager::lastError() const { return m_lastError; }
