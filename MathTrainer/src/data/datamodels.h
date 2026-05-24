#pragma once
#include <QString>
#include <QList>
#include <QDateTime>

struct Example {
    int id = 0;
    int sessionId = 0;
    int operandA = 0;
    int operandB = 0;
    QChar operation = '+';
    int correctAnswer = 0;
    int userAnswer = 0;
    bool isCorrect = false;
};

struct Session {
    int id = 0;
    QDateTime date;
    QString operationType; // "+", "-", "*", "/", "mix"
    int difficulty = 1;    // 1=easy, 2=medium, 3=hard
    int totalCount = 0;
    int correctCount = 0;
    int durationSec = 0;
    QList<Example> examples;
};

struct Settings {
    QString operationType = "+";
    int difficulty = 1;
    int examplesCount = 10;
    int timeLimit = 0; // 0 = disabled
};
