#include "trainingsession.h"
#include "examplegenerator.h"
#include "answerchecker.h"
#include <QElapsedTimer>

void TrainingSession::start(const Settings &settings)
{
    m_session = Session();
    m_session.date          = QDateTime::currentDateTime();
    m_session.operationType = settings.operationType;
    m_session.difficulty    = settings.difficulty;
    m_session.totalCount    = settings.examplesCount;
    m_session.correctCount  = 0;
    m_index = 0;

    m_session.examples.clear();
    for (int i = 0; i < settings.examplesCount; ++i) {
        Example e = ExampleGenerator::generate(settings.operationType, settings.difficulty);
        e.id        = i + 1;
        e.sessionId = 0; // will be set after save
        m_session.examples.append(e);
    }

    m_timer.restart();
}

bool TrainingSession::isFinished() const
{
    return m_index >= m_session.totalCount;
}

const Example &TrainingSession::currentExample() const
{
    return m_session.examples.at(m_index);
}

bool TrainingSession::submitAnswer(const QString &input)
{
    if (m_index >= m_session.totalCount)
        return false;

    Example &e = m_session.examples[m_index];
    if (!AnswerChecker::check(e, input))
        return false;

    if (e.isCorrect)
        m_session.correctCount++;

    return true;
}

void TrainingSession::nextExample()
{
    if (m_index < m_session.totalCount)
        m_index++;
}

Session TrainingSession::finalize()
{
    m_session.durationSec = static_cast<int>(m_timer.elapsed() / 1000);
    return m_session;
}
