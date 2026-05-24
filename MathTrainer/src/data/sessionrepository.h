#pragma once
#include "datamodels.h"
#include <QString>

class SessionRepository {
public:
    explicit SessionRepository(const QString &filePath);

    QList<Session> loadAll();
    bool saveAll(const QList<Session> &sessions);
    bool add(Session &session); // sets session.id
    bool remove(int id);

private:
    QString m_filePath;
    QList<Session> m_sessions;
    int m_nextId = 1;

    bool writeToFile();
    Session sessionFromJson(const QJsonObject &obj) const;
    QJsonObject sessionToJson(const Session &s) const;
};
