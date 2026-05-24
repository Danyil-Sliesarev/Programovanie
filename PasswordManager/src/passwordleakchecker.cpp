#include "passwordleakchecker.h"
#include <QCryptographicHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <chrono>
#endif

PasswordLeakChecker::PasswordLeakChecker(QObject *parent) : QObject(parent) {}

QString PasswordLeakChecker::sha1HexUpper(const QString &password)
{
    return QString::fromLatin1(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex().toUpper());
}

QString PasswordLeakChecker::parsePwnedRangeResponse(const QByteArray &payload, const QString &suffix)
{
    const QList<QByteArray> lines = payload.split('\n');
    const QByteArray expected = suffix.toLatin1().toUpper();
    for (QByteArray line : lines) {
        line = line.trimmed();
        const int colon = line.indexOf(':');
        if (colon <= 0) continue;
        const QByteArray apiSuffix = line.left(colon).toUpper();
        if (apiSuffix == expected) {
            return QString::fromLatin1(line.mid(colon + 1));
        }
    }
    return "0";
}

void PasswordLeakChecker::checkPassword(const QString &password)
{
    if (password.isEmpty()) {
        emit checkFailed("Password is empty.");
        return;
    }

    emit checkStarted();
    const QString hash = sha1HexUpper(password);
    const QString prefix = hash.left(5);
    const QString suffix = hash.mid(5);

    QNetworkRequest request(QUrl("https://api.pwnedpasswords.com/range/" + prefix));
    request.setRawHeader("User-Agent", "PasswordManagerCourse/1.0");
    request.setRawHeader("Add-Padding", "true");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    request.setTransferTimeout(std::chrono::seconds(10));
#endif

    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, suffix]() {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray body = reply->readAll();
        const auto error = reply->error();
        const QString errorString = reply->errorString();
        reply->deleteLater();

        if (error != QNetworkReply::NoError) {
            emit checkFailed(errorString);
            return;
        }
        if (status != 200) {
            emit checkFailed("HTTP error: " + QString::number(status));
            return;
        }

        bool ok = false;
        const int count = parsePwnedRangeResponse(body, suffix).toInt(&ok);
        if (!ok) {
            emit checkFailed("Could not parse server response.");
            return;
        }
        if (count > 0) {
            emit checkCompleted(true, count, "Pwned: found " + QString::number(count) + " times");
        } else {
            emit checkCompleted(false, 0, "Safe: not found in known leaks");
        }
    });
}
