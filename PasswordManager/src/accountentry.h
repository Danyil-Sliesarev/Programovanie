#pragma once
#include <QString>

struct AccountEntry
{
    int id = 0;
    QString title;
    QString username;
    QString password;
    QString website;
    QString category;
    QString notes;
    QString updatedAt;
    QString leakStatus;
};
