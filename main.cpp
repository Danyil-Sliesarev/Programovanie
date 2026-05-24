#include "statscalculator.h"

int StatsCalculator::accuracy(const Session &s)
{
    if (s.totalCount == 0) return 0;
    return static_cast<int>(100.0 * s.correctCount / s.totalCount);
}

QString StatsCalculator::grade(int acc)
{
    if (acc >= 90) return "Відмінно! 🏆";
    if (acc >= 70) return "Добре! 👍";
    if (acc >= 50) return "Непогано, але є куди рости.";
    return "Треба більше практики!";
}

double StatsCalculator::averageAccuracy(const QList<Session> &sessions)
{
    if (sessions.isEmpty()) return 0.0;
    double sum = 0;
    for (const Session &s : sessions)
        sum += accuracy(s);
    return sum / sessions.size();
}
