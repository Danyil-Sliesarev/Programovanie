#pragma once
#include "datamodels.h"
#include <QString>

class SettingsManager {
public:
    explicit SettingsManager(const QString &filePath);
    Settings load();
    void save(const Settings &s);

private:
    QString m_filePath;
};
