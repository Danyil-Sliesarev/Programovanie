#pragma once
#include "data/datamodels.h"

class TrainingSession {
public:
    void start(const Settings &settings);
    bool isFinished() const;

    const Example &currentExample() const;
    int currentIndex() const { return m_index; }
    int totalCount()   const { return m_session.totalCount; }

    // Returns false if input is invalid (non-numeric)
    bool submitAnswer(const QString &input);
    void nextExample();

    Session finalize(); // call when finished to get the completed Session

private:
    Session  m_session;
    int      m_index = 0;
    QElapsedTimer m_timer;
};
