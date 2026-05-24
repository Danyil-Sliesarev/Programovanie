#pragma once
#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    bool open(const QString &filePath);
    bool initializeSchema();
    QSqlDatabase database() const;
    QString lastError() const;
private:
    QSqlDatabase m_db;
    QString m_lastError;
};
