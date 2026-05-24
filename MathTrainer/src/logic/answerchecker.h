#pragma once
#include "data/datamodels.h"

class AnswerChecker {
public:
    // Returns true and fills e.isCorrect / e.userAnswer
    static bool check(Example &e, const QString &input);
};
