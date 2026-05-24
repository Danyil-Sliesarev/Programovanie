#pragma once
#include "data/datamodels.h"
#include <QString>

class StatsCalculator {
public:
    static int    accuracy(const Session &s);   // 0-100 percent
    static QString grade(int accuracy);          // "Відмінно!" etc.
    static double averageAccuracy(const QList<Session> &sessions);
};
