#include "batchchecker.h"
#include "passwordleakchecker.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <chrono>
#endif

QString BatchChecker::checkOneOnline(const QString &password)
{
    if (password.isEmpty()) return "Skipped: empty password";

    const QString hash = PasswordLeakChecker::sha1HexUpper(password);
    const QString prefix = hash.left(5);
    const QString suffix = hash.mid(5);

    QNetworkAccessManager network;
    QNetworkRequest request(QUrl("https://api.pwnedpasswords.com/range/" + prefix));
    request.setRawHeader("User-Agent", "PasswordManagerCourse/1.0");
    request.setRawHeader("Add-Padding", "true");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    request.setTransferTimeout(std::chrono::seconds(10));
#endif
    QNetworkReply *reply = network.get(request);

    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeout, &QTimer::timeout, [&]() {
        if (!reply->isFinished()) reply->abort();
        loop.quit();
    });
    timeout.start(10000);
    loop.exec();

    const auto error = reply->error();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();
    const QString errorString = reply->errorString();
    reply->deleteLater();

    if (error != QNetworkReply::NoError) return "Error: " + errorString;
    if (status != 200) return "Error: HTTP " + QString::number(status);

    bool ok = false;
    const int count = PasswordLeakChecker::parsePwnedRangeResponse(body, suffix).toInt(&ok);
    if (!ok) return "Error: parse failed";
    if (count > 0) return "Pwned: " + QString::number(count);
    return "Safe";
}

BatchCheckResult BatchChecker::checkAll(const QList<AccountEntry> &items, ProgressCallback progress)
{
    BatchCheckResult result;
    result.total = items.size();

    for (const AccountEntry &entry : items) {
        QString status = checkOneOnline(entry.password);
        ++result.checked;
        if (status.startsWith("Pwned", Qt::CaseInsensitive)) ++result.compromised;
        if (status.startsWith("Error", Qt::CaseInsensitive)) ++result.failed;
        result.statuses.append(qMakePair(entry.id, status));
        if (progress) progress(result.checked, result.total, entry.id, status);
    }
    return result;
}
