#include "resultswidget.h"
#include "logic/statscalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

ResultsWidget::ResultsWidget(QWidget *parent) : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(60, 30, 60, 30);

    auto *title = new QLabel("📋 Результати сесії", this);
    title->setAlignment(Qt::AlignCenter);
    QFont tf = title->font(); tf.setPointSize(20); tf.setBold(true);
    title->setFont(tf);
    root->addWidget(title);

    root->addSpacing(10);

    m_scoreLabel = new QLabel(this);
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    QFont sf = m_scoreLabel->font(); sf.setPointSize(40); sf.setBold(true);
    m_scoreLabel->setFont(sf);
    root->addWidget(m_scoreLabel);

    m_accuracyLabel = new QLabel(this);
    m_accuracyLabel->setAlignment(Qt::AlignCenter);
    QFont af = m_accuracyLabel->font(); af.setPointSize(16);
    m_accuracyLabel->setFont(af);
    m_accuracyLabel->setStyleSheet("color:#555;");
    root->addWidget(m_accuracyLabel);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setStyleSheet("color:#777; font-size:13px;");
    root->addWidget(m_timeLabel);

    m_gradeLabel = new QLabel(this);
    m_gradeLabel->setAlignment(Qt::AlignCenter);
    QFont gf = m_gradeLabel->font(); gf.setPointSize(16); gf.setBold(true);
    m_gradeLabel->setFont(gf);
    root->addWidget(m_gradeLabel);

    m_savedLabel = new QLabel("✅ Збережено!", this);
    m_savedLabel->setAlignment(Qt::AlignCenter);
    m_savedLabel->setStyleSheet("color:#388E3C; font-size:13px;");
    m_savedLabel->setVisible(false);
    root->addWidget(m_savedLabel);

    root->addStretch();

    // Buttons
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    m_saveBtn = new QPushButton("💾 Зберегти", this);
    m_saveBtn->setFixedHeight(42);
    m_saveBtn->setStyleSheet(
        "QPushButton { background:#2196F3; color:white; border-radius:6px; font-size:14px; }"
        "QPushButton:hover { background:#1976D2; }"
        "QPushButton:disabled { background:#90CAF9; }"
    );
    connect(m_saveBtn, &QPushButton::clicked, this, [this]{
        emit saveRequested(m_session);
    });

    m_retryBtn = new QPushButton("🔄 Повторити", this);
    m_retryBtn->setFixedHeight(42);
    m_retryBtn->setStyleSheet(
        "QPushButton { background:#FF9800; color:white; border-radius:6px; font-size:14px; }"
        "QPushButton:hover { background:#E65100; }"
    );
    connect(m_retryBtn, &QPushButton::clicked, this, [this]{
        if (!m_saved) {
            int ret = QMessageBox::question(this, "Не збережено",
                "Результат не збережено. Все одно повторити?",
                QMessageBox::Yes | QMessageBox::No);
            if (ret != QMessageBox::Yes) return;
        }
        emit retryRequested();
    });

    m_menuBtn = new QPushButton("🏠 В меню", this);
    m_menuBtn->setFixedHeight(42);
    m_menuBtn->setStyleSheet(
        "QPushButton { background:#9E9E9E; color:white; border-radius:6px; font-size:14px; }"
        "QPushButton:hover { background:#616161; }"
    );
    connect(m_menuBtn, &QPushButton::clicked, this, [this]{
        if (!m_saved) {
            int ret = QMessageBox::question(this, "Не збережено",
                "Результат не збережено. Вийти до меню?",
                QMessageBox::Yes | QMessageBox::No);
            if (ret != QMessageBox::Yes) return;
        }
        emit backToMenuRequested();
    });

    btnRow->addWidget(m_saveBtn);
    btnRow->addWidget(m_retryBtn);
    btnRow->addWidget(m_menuBtn);
    root->addLayout(btnRow);
}

void ResultsWidget::showResults(const Session &session)
{
    m_session = session;
    m_saved   = false;
    m_savedLabel->setVisible(false);
    m_saveBtn->setEnabled(true);

    int acc = StatsCalculator::accuracy(session);
    m_scoreLabel->setText(QString("%1 / %2")
                          .arg(session.correctCount).arg(session.totalCount));

    QString color = acc >= 70 ? "#2E7D32" : (acc >= 50 ? "#E65100" : "#c62828");
    m_scoreLabel->setStyleSheet(QString("color:%1;").arg(color));

    m_accuracyLabel->setText(QString("Точність: %1%").arg(acc));

    int min = session.durationSec / 60;
    int sec = session.durationSec % 60;
    m_timeLabel->setText(QString("Час: %1 хв %2 сек").arg(min).arg(sec, 2, 10, QChar('0')));

    m_gradeLabel->setText(StatsCalculator::grade(acc));
}

void ResultsWidget::markSaved()
{
    m_saved = true;
    m_saveBtn->setEnabled(false);
    m_savedLabel->setVisible(true);
}
