#include "settingsmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

SettingsManager::SettingsManager(const QString &filePath)
    : m_filePath(filePath) {}

Settings SettingsManager::load()
{
    Settings s;
    QFile file(m_filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
        return s;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return s;

    QJsonObject obj = doc.object();
    s.operationType  = obj.value("operationType").toString("+");
    s.difficulty     = obj.value("difficulty").toInt(1);
    s.examplesCount  = obj.value("examplesCount").toInt(10);
    s.timeLimit      = obj.value("timeLimit").toInt(0);
    return s;
}

void SettingsManager::save(const Settings &s)
{
    QJsonObject obj;
    obj["operationType"] = s.operationType;
    obj["difficulty"]    = s.difficulty;
    obj["examplesCount"] = s.examplesCount;
    obj["timeLimit"]     = s.timeLimit;

    QSaveFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.commit();
}
