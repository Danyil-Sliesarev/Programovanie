#pragma once
#include "accountentry.h"
#include <QList>
#include <QString>
#include <functional>

struct BatchCheckResult
{
    int total = 0;
    int checked = 0;
    int compromised = 0;
    int failed = 0;
    QList<QPair<int, QString>> statuses;
};

class BatchChecker
{
public:
    using ProgressCallback = std::function<void(int checked, int total, int id, const QString &status)>;
    static BatchCheckResult checkAll(const QList<AccountEntry> &items, ProgressCallback progress = {});
private:
    static QString checkOneOnline(const QString &password);
};
