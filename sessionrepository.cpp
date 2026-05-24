#pragma once
#include <QWidget>
#include "data/datamodels.h"

class QLabel;
class QPushButton;

class ResultsWidget : public QWidget {
    Q_OBJECT
public:
    explicit ResultsWidget(QWidget *parent = nullptr);
    void showResults(const Session &session);
    void markSaved();

signals:
    void saveRequested(const Session &session);
    void retryRequested();
    void backToMenuRequested();

private:
    Session     m_session;
    bool        m_saved = false;

    QLabel      *m_scoreLabel;
    QLabel      *m_accuracyLabel;
    QLabel      *m_timeLabel;
    QLabel      *m_gradeLabel;
    QLabel      *m_savedLabel;
    QPushButton *m_saveBtn;
    QPushButton *m_retryBtn;
    QPushButton *m_menuBtn;
};
