#pragma once
#include <QWidget>
#include "data/datamodels.h"

class QTableWidget;
class QLabel;
class QPushButton;

class HistoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    void setSessions(const QList<Session> &sessions);

signals:
    void deleteRequested(int sessionId);
    void backRequested();

private slots:
    void onDelete();
    void onSelectionChanged();

private:
    QList<Session> m_sessions;
    QTableWidget  *m_table;
    QLabel        *m_avgLabel;
    QLabel        *m_emptyLabel;
    QPushButton   *m_deleteBtn;
};
