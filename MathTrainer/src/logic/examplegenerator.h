#pragma once
#include "data/datamodels.h"

class ExampleGenerator {
public:
    // Returns a generated Example (sessionId and id not set here)
    static Example generate(const QString &operationType, int difficulty);

private:
    static QChar pickOperation(const QString &operationType);
    static int   rangeMax(int difficulty);
};
