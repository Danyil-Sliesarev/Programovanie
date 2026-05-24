#include "answerchecker.h"

bool AnswerChecker::check(Example &e, const QString &input)
{
    bool ok = false;
    int answer = input.trimmed().toInt(&ok);
    if (!ok)
        return false;
    e.userAnswer = answer;
    e.isCorrect  = (answer == e.correctAnswer);
    return true;
}
