#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

class PasswordLeakChecker : public QObject
{
    Q_OBJECT
public:
    explicit PasswordLeakChecker(QObject *parent = nullptr);
    void checkPassword(const QString &password);
    static QString sha1HexUpper(const QString &password);
    static QString parsePwnedRangeResponse(const QByteArray &payload, const QString &suffix);
signals:
    void checkStarted();
    void checkCompleted(bool found, int count, const QString &message);
    void checkFailed(const QString &message);
private:
    QNetworkAccessManager m_network;
};
