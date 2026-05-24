#include "examplegenerator.h"
#include <QRandomGenerator>

Example ExampleGenerator::generate(const QString &operationType, int difficulty)
{
    Example e;
    e.operation = pickOperation(operationType);
    int maxVal  = rangeMax(difficulty);

    if (e.operation == '+' || e.operation == '-') {
        e.operandA = QRandomGenerator::global()->bounded(1, maxVal + 1);
        e.operandB = QRandomGenerator::global()->bounded(1, maxVal + 1);
        if (e.operation == '-' && e.operandA < e.operandB)
            std::swap(e.operandA, e.operandB);
        e.correctAnswer = (e.operation == '+') ? e.operandA + e.operandB
                                               : e.operandA - e.operandB;
    } else if (e.operation == '*') {
        int half = qMax(2, maxVal / 4);
        e.operandA = QRandomGenerator::global()->bounded(2, half + 1);
        e.operandB = QRandomGenerator::global()->bounded(2, half + 1);
        e.correctAnswer = e.operandA * e.operandB;
    } else { // '/'
        int divisor = QRandomGenerator::global()->bounded(2, qMax(3, maxVal / 4) + 1);
        int result  = QRandomGenerator::global()->bounded(2, qMax(3, maxVal / 4) + 1);
        e.operandA  = divisor * result;
        e.operandB  = divisor;
        e.correctAnswer = result;
    }
    return e;
}

QChar ExampleGenerator::pickOperation(const QString &operationType)
{
    if (operationType == "mix") {
        static const QChar ops[] = {'+', '-', '*', '/'};
        return ops[QRandomGenerator::global()->bounded(4)];
    }
    return operationType.isEmpty() ? QChar('+') : operationType.at(0);
}

int ExampleGenerator::rangeMax(int difficulty)
{
    switch (difficulty) {
        case 1:  return 20;
        case 2:  return 50;
        default: return 100;
    }
}
