#pragma once
#include <QWidget>
#include "data/datamodels.h"
#include "logic/trainingsession.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QProgressBar;

class TrainingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrainingWidget(QWidget *parent = nullptr);
    void startSession(const Settings &settings);

signals:
    void trainingFinished(const Session &session);
    void cancelRequested();

private slots:
    void onSubmit();
    void onNext();
    void onCancel();

private:
    void showCurrentExample();
    void setAnswerState(bool answered, bool correct, int correctAnswer);

    TrainingSession m_session;
    bool m_answered = false;

    QLabel       *m_progressLabel;
    QProgressBar *m_progressBar;
    QLabel       *m_exampleLabel;
    QLineEdit    *m_answerEdit;
    QLabel       *m_feedbackLabel;
    QPushButton  *m_submitBtn;
    QPushButton  *m_nextBtn;
    QPushButton  *m_cancelBtn;
};
