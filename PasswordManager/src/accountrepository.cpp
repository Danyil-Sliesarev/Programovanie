#include "accountrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>

AccountRepository::AccountRepository(const QSqlDatabase &db) : m_db(db) {}
void AccountRepository::setDatabase(const QSqlDatabase &db) { m_db = db; }

QList<AccountEntry> AccountRepository::loadAll() const
{
    QList<AccountEntry> items;
    QSqlQuery query(m_db);
    if (!query.exec("SELECT id,title,username,password,website,category,notes,updated_at,leak_status FROM accounts ORDER BY id")) {
        m_lastError = query.lastError().text();
        return items;
    }
    while (query.next()) {
        AccountEntry e;
        e.id = query.value(0).toInt();
        e.title = query.value(1).toString();
        e.username = query.value(2).toString();
        e.password = query.value(3).toString();
        e.website = query.value(4).toString();
        e.category = query.value(5).toString();
        e.notes = query.value(6).toString();
        e.updatedAt = query.value(7).toString();
        e.leakStatus = query.value(8).toString();
        items.append(e);
    }
    return items;
}

bool AccountRepository::insert(AccountEntry &entry)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO accounts(title,username,password,website,category,notes,updated_at,leak_status) "
                  "VALUES(:title,:username,:password,:website,:category,:notes,:updated,:leak)");
    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    query.bindValue(":title", entry.title.isEmpty() ? "New entry" : entry.title);
    query.bindValue(":username", entry.username);
    query.bindValue(":password", entry.password);
    query.bindValue(":website", entry.website);
    query.bindValue(":category", entry.category.isEmpty() ? "General" : entry.category);
    query.bindValue(":notes", entry.notes);
    query.bindValue(":updated", now);
    query.bindValue(":leak", entry.leakStatus.isEmpty() ? "Not checked" : entry.leakStatus);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    entry.id = query.lastInsertId().toInt();
    entry.updatedAt = now;
    if (entry.title.isEmpty()) entry.title = "New entry";
    if (entry.category.isEmpty()) entry.category = "General";
    if (entry.leakStatus.isEmpty()) entry.leakStatus = "Not checked";
    return true;
}

bool AccountRepository::update(const AccountEntry &entry)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE accounts SET title=:title, username=:username, password=:password, website=:website, "
                  "category=:category, notes=:notes, updated_at=:updated, leak_status=:leak WHERE id=:id");
    query.bindValue(":title", entry.title);
    query.bindValue(":username", entry.username);
    query.bindValue(":password", entry.password);
    query.bindValue(":website", entry.website);
    query.bindValue(":category", entry.category);
    query.bindValue(":notes", entry.notes);
    query.bindValue(":updated", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":leak", entry.leakStatus);
    query.bindValue(":id", entry.id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool AccountRepository::remove(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM accounts WHERE id=:id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool AccountRepository::updateLeakStatus(int id, const QString &status)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE accounts SET leak_status=:status, updated_at=:updated WHERE id=:id");
    query.bindValue(":status", status);
    query.bindValue(":updated", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":id", id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

QString AccountRepository::lastError() const { return m_lastError; }
