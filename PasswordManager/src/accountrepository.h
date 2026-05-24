#pragma once
#include "accountentry.h"
#include <QList>
#include <QSqlDatabase>
#include <QString>

class AccountRepository
{
public:
    explicit AccountRepository(const QSqlDatabase &db = QSqlDatabase());
    void setDatabase(const QSqlDatabase &db);

    QList<AccountEntry> loadAll() const;
    bool insert(AccountEntry &entry);
    bool update(const AccountEntry &entry);
    bool remove(int id);
    bool updateLeakStatus(int id, const QString &status);
    QString lastError() const;
private:
    QSqlDatabase m_db;
    mutable QString m_lastError;
};
