#include "mainmenuwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

MainMenuWidget::MainMenuWidget(QWidget *parent) : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(18);
    root->setContentsMargins(60, 40, 60, 40);

    // Title
    auto *title = new QLabel("🧮 Math Trainer", this);
    title->setAlignment(Qt::AlignCenter);
    QFont tf = title->font();
    tf.setPointSize(26);
    tf.setBold(true);
    title->setFont(tf);
    root->addWidget(title);

    auto *sub = new QLabel("Тренуй математику щодня!", this);
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color: #666;");
    root->addWidget(sub);

    root->addSpacing(10);

    // Form
    auto *form = new QFormLayout();
    form->setSpacing(12);

    m_opCombo = new QComboBox(this);
    m_opCombo->addItem("Додавання (+)", "+");
    m_opCombo->addItem("Віднімання (−)", "-");
    m_opCombo->addItem("Множення (×)", "*");
    m_opCombo->addItem("Ділення (÷)", "/");
    m_opCombo->addItem("Змішано", "mix");
    form->addRow("Операція:", m_opCombo);

    m_diffCombo = new QComboBox(this);
    m_diffCombo->addItem("Легкий (до 20)", 1);
    m_diffCombo->addItem("Середній (до 50)", 2);
    m_diffCombo->addItem("Важкий (до 100)", 3);
    form->addRow("Складність:", m_diffCombo);

    m_countSpin = new QSpinBox(this);
    m_countSpin->setRange(5, 50);
    m_countSpin->setValue(10);
    m_countSpin->setSuffix(" прикладів");
    form->addRow("Кількість:", m_countSpin);

    root->addLayout(form);
    root->addSpacing(10);

    // Buttons
    auto *btnLayout = new QHBoxLayout();

    auto *startBtn = new QPushButton("▶  Розпочати", this);
    startBtn->setFixedHeight(44);
    startBtn->setStyleSheet(
        "QPushButton { background:#2196F3; color:white; border-radius:6px; font-size:15px; font-weight:bold; }"
        "QPushButton:hover { background:#1976D2; }"
    );
    connect(startBtn, &QPushButton::clicked, this, &MainMenuWidget::onStartClicked);

    auto *histBtn = new QPushButton("📊  Статистика", this);
    histBtn->setFixedHeight(44);
    histBtn->setStyleSheet(
        "QPushButton { background:#4CAF50; color:white; border-radius:6px; font-size:14px; }"
        "QPushButton:hover { background:#388E3C; }"
    );
    connect(histBtn, &QPushButton::clicked, this, &MainMenuWidget::showHistory);

    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(histBtn);
    root->addLayout(btnLayout);
    root->addStretch();
}

void MainMenuWidget::applySettings(const Settings &s)
{
    for (int i = 0; i < m_opCombo->count(); ++i)
        if (m_opCombo->itemData(i).toString() == s.operationType) {
            m_opCombo->setCurrentIndex(i);
            break;
        }
    m_diffCombo->setCurrentIndex(qBound(0, s.difficulty - 1, 2));
    m_countSpin->setValue(qBound(5, s.examplesCount, 50));
}

void MainMenuWidget::onStartClicked()
{
    Settings s;
    s.operationType = m_opCombo->currentData().toString();
    s.difficulty    = m_diffCombo->currentData().toInt();
    s.examplesCount = m_countSpin->value();
    emit startTraining(s);
}
