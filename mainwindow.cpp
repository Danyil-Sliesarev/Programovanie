#include "trainingwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QKeyEvent>

TrainingWidget::TrainingWidget(QWidget *parent) : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(14);
    root->setContentsMargins(50, 30, 50, 30);

    // Progress
    auto *topRow = new QHBoxLayout();
    m_progressLabel = new QLabel("Приклад 1 / 10", this);
    m_progressLabel->setStyleSheet("color:#555; font-size:13px;");
    topRow->addWidget(m_progressLabel);
    topRow->addStretch();

    m_cancelBtn = new QPushButton("✕ Скасувати", this);
    m_cancelBtn->setStyleSheet(
        "QPushButton { background:#f44336; color:white; border-radius:5px; padding:4px 10px; }"
        "QPushButton:hover { background:#c62828; }"
    );
    connect(m_cancelBtn, &QPushButton::clicked, this, &TrainingWidget::onCancel);
    topRow->addWidget(m_cancelBtn);
    root->addLayout(topRow);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 1);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(8);
    m_progressBar->setStyleSheet(
        "QProgressBar { border-radius:4px; background:#e0e0e0; }"
        "QProgressBar::chunk { border-radius:4px; background:#2196F3; }"
    );
    root->addWidget(m_progressBar);

    root->addStretch();

    // Example label
    m_exampleLabel = new QLabel("", this);
    m_exampleLabel->setAlignment(Qt::AlignCenter);
    QFont ef = m_exampleLabel->font();
    ef.setPointSize(36);
    ef.setBold(true);
    m_exampleLabel->setFont(ef);
    root->addWidget(m_exampleLabel);

    root->addSpacing(6);

    // Answer input
    m_answerEdit = new QLineEdit(this);
    m_answerEdit->setAlignment(Qt::AlignCenter);
    m_answerEdit->setPlaceholderText("Введи відповідь...");
    m_answerEdit->setFixedHeight(46);
    QFont af = m_answerEdit->font();
    af.setPointSize(18);
    m_answerEdit->setFont(af);
    m_answerEdit->setStyleSheet("border:2px solid #bbb; border-radius:8px; padding:4px;");
    connect(m_answerEdit, &QLineEdit::returnPressed, this, &TrainingWidget::onSubmit);
    root->addWidget(m_answerEdit);

    // Feedback
    m_feedbackLabel = new QLabel("", this);
    m_feedbackLabel->setAlignment(Qt::AlignCenter);
    QFont ff = m_feedbackLabel->font();
    ff.setPointSize(14);
    m_feedbackLabel->setFont(ff);
    m_feedbackLabel->setFixedHeight(28);
    root->addWidget(m_feedbackLabel);

    root->addStretch();

    // Buttons
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    m_submitBtn = new QPushButton("✓  Підтвердити", this);
    m_submitBtn->setFixedHeight(42);
    m_submitBtn->setStyleSheet(
        "QPushButton { background:#2196F3; color:white; border-radius:6px; font-size:14px; font-weight:bold; }"
        "QPushButton:hover { background:#1976D2; }"
        "QPushButton:disabled { background:#90CAF9; }"
    );
    connect(m_submitBtn, &QPushButton::clicked, this, &TrainingWidget::onSubmit);

    m_nextBtn = new QPushButton("→  Наступний", this);
    m_nextBtn->setFixedHeight(42);
    m_nextBtn->setVisible(false);
    m_nextBtn->setStyleSheet(
        "QPushButton { background:#4CAF50; color:white; border-radius:6px; font-size:14px; }"
        "QPushButton:hover { background:#388E3C; }"
    );
    connect(m_nextBtn, &QPushButton::clicked, this, &TrainingWidget::onNext);

    btnRow->addWidget(m_submitBtn);
    btnRow->addWidget(m_nextBtn);
    root->addLayout(btnRow);
}

void TrainingWidget::startSession(const Settings &settings)
{
    m_session.start(settings);
    m_answered = false;
    showCurrentExample();
    m_answerEdit->setFocus();
}

void TrainingWidget::showCurrentExample()
{
    const Example &e = m_session.currentExample();
    QString opStr;
    if      (e.operation == '+') opStr = "+";
    else if (e.operation == '-') opStr = "−";
    else if (e.operation == '*') opStr = "×";
    else                         opStr = "÷";

    m_exampleLabel->setText(QString("%1  %2  %3  =  ?")
                            .arg(e.operandA).arg(opStr).arg(e.operandB));

    int idx = m_session.currentIndex();
    int total = m_session.totalCount();
    m_progressLabel->setText(QString("Приклад %1 / %2").arg(idx + 1).arg(total));
    m_progressBar->setRange(0, total);
    m_progressBar->setValue(idx);

    m_answerEdit->clear();
    m_answerEdit->setStyleSheet("border:2px solid #bbb; border-radius:8px; padding:4px;");
    m_feedbackLabel->setText("");
    m_submitBtn->setVisible(true);
    m_nextBtn->setVisible(false);
    m_answered = false;
    m_answerEdit->setEnabled(true);
    m_answerEdit->setFocus();
}

void TrainingWidget::onSubmit()
{
    if (m_answered) return;

    QString input = m_answerEdit->text().trimmed();
    if (input.isEmpty()) {
        m_answerEdit->setStyleSheet("border:2px solid #f44336; border-radius:8px; padding:4px;");
        return;
    }

    // Copy current example, submit
    Example e = m_session.currentExample();
    bool valid = m_session.submitAnswer(input);
    if (!valid) {
        m_answerEdit->setStyleSheet("border:2px solid #f44336; border-radius:8px; padding:4px;");
        m_feedbackLabel->setText("⚠ Введи ціле число!");
        m_feedbackLabel->setStyleSheet("color:#f44336;");
        return;
    }

    // Re-read after submit
    e = m_session.currentExample();
    setAnswerState(true, e.isCorrect, e.correctAnswer);
    m_answered = true;
}

void TrainingWidget::setAnswerState(bool /*answered*/, bool correct, int correctAnswer)
{
    if (correct) {
        m_feedbackLabel->setText("✅ Правильно!");
        m_feedbackLabel->setStyleSheet("color:#388E3C; font-weight:bold;");
        m_answerEdit->setStyleSheet("border:2px solid #4CAF50; border-radius:8px; padding:4px; background:#E8F5E9;");
    } else {
        m_feedbackLabel->setText(QString("❌ Неправильно. Відповідь: %1").arg(correctAnswer));
        m_feedbackLabel->setStyleSheet("color:#c62828; font-weight:bold;");
        m_answerEdit->setStyleSheet("border:2px solid #f44336; border-radius:8px; padding:4px; background:#FFEBEE;");
    }
    m_answerEdit->setEnabled(false);
    m_submitBtn->setVisible(false);
    m_nextBtn->setVisible(true);
    m_nextBtn->setFocus();
}

void TrainingWidget::onNext()
{
    m_session.nextExample();
    if (m_session.isFinished()) {
        Session finished = m_session.finalize();
        emit trainingFinished(finished);
    } else {
        showCurrentExample();
    }
}

void TrainingWidget::onCancel()
{
    int ret = QMessageBox::question(this, "Скасувати тренування",
        "Ви впевнені, що хочете вийти? Прогрес не буде збережено.",
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        emit cancelRequested();
}
