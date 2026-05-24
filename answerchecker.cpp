#include "sessionrepository.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSaveFile>

SessionRepository::SessionRepository(const QString &filePath)
    : m_filePath(filePath) {}

QList<Session> SessionRepository::loadAll()
{
    m_sessions.clear();
    m_nextId = 1;

    QFile file(m_filePath);
    if (!file.exists())
        return m_sessions;

    if (!file.open(QIODevice::ReadOnly))
        return m_sessions;

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return m_sessions;

    QJsonObject root = doc.object();
    m_nextId = root.value("nextId").toInt(1);

    QJsonArray arr = root.value("sessions").toArray();
    for (const QJsonValue &v : arr)
        m_sessions.append(sessionFromJson(v.toObject()));

    return m_sessions;
}

bool SessionRepository::saveAll(const QList<Session> &sessions)
{
    m_sessions = sessions;
    return writeToFile();
}

bool SessionRepository::add(Session &session)
{
    session.id = m_nextId++;
    m_sessions.append(session);
    return writeToFile();
}

bool SessionRepository::remove(int id)
{
    for (int i = 0; i < m_sessions.size(); ++i) {
        if (m_sessions[i].id == id) {
            m_sessions.removeAt(i);
            return writeToFile();
        }
    }
    return false;
}

bool SessionRepository::writeToFile()
{
    QJsonArray arr;
    for (const Session &s : m_sessions)
        arr.append(sessionToJson(s));

    QJsonObject root;
    root["nextId"] = m_nextId;
    root["sessions"] = arr;

    QSaveFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return file.commit();
}

Session SessionRepository::sessionFromJson(const QJsonObject &obj) const
{
    Session s;
    s.id           = obj["id"].toInt();
    s.date         = QDateTime::fromString(obj["date"].toString(), Qt::ISODate);
    s.operationType= obj["operationType"].toString();
    s.difficulty   = obj["difficulty"].toInt(1);
    s.totalCount   = obj["totalCount"].toInt();
    s.correctCount = obj["correctCount"].toInt();
    s.durationSec  = obj["durationSec"].toInt();

    QJsonArray exArr = obj["examples"].toArray();
    for (const QJsonValue &v : exArr) {
        QJsonObject eo = v.toObject();
        Example e;
        e.id            = eo["id"].toInt();
        e.sessionId     = eo["sessionId"].toInt();
        e.operandA      = eo["operandA"].toInt();
        e.operandB      = eo["operandB"].toInt();
        e.operation     = eo["operation"].toString().isEmpty()
                          ? QChar('+') : eo["operation"].toString().at(0);
        e.correctAnswer = eo["correctAnswer"].toInt();
        e.userAnswer    = eo["userAnswer"].toInt();
        e.isCorrect     = eo["isCorrect"].toBool();
        s.examples.append(e);
    }
    return s;
}

QJsonObject SessionRepository::sessionToJson(const Session &s) const
{
    QJsonObject obj;
    obj["id"]            = s.id;
    obj["date"]          = s.date.toString(Qt::ISODate);
    obj["operationType"] = s.operationType;
    obj["difficulty"]    = s.difficulty;
    obj["totalCount"]    = s.totalCount;
    obj["correctCount"]  = s.correctCount;
    obj["durationSec"]   = s.durationSec;

    QJsonArray exArr;
    for (const Example &e : s.examples) {
        QJsonObject eo;
        eo["id"]            = e.id;
        eo["sessionId"]     = e.sessionId;
        eo["operandA"]      = e.operandA;
        eo["operandB"]      = e.operandB;
        eo["operation"]     = QString(e.operation);
        eo["correctAnswer"] = e.correctAnswer;
        eo["userAnswer"]    = e.userAnswer;
        eo["isCorrect"]     = e.isCorrect;
        exArr.append(eo);
    }
    obj["examples"] = exArr;
    return obj;
}
