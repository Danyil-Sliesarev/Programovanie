#include "historywidget.h"
#include "logic/statscalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(20, 20, 20, 20);

    auto *title = new QLabel("📊 Статистика сесій", this);
    title->setAlignment(Qt::AlignCenter);
    QFont tf = title->font(); tf.setPointSize(18); tf.setBold(true);
    title->setFont(tf);
    root->addWidget(title);

    // Empty state label
    m_emptyLabel = new QLabel("Немає збережених сесій.\nРозпочніть тренування!", this);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet("color:#9e9e9e; font-size:15px;");
    m_emptyLabel->setVisible(false);
    root->addWidget(m_emptyLabel);

    // Table
    m_table = new QTableWidget(0, 6, this);
    m_table->setHorizontalHeaderLabels({"Дата", "Операція", "Складність", "Правильно", "Всього", "Точність"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    root->addWidget(m_table);

    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &HistoryWidget::onSelectionChanged);

    // Average label
    m_avgLabel = new QLabel("Середній результат: —", this);
    m_avgLabel->setAlignment(Qt::AlignCenter);
    m_avgLabel->setStyleSheet("color:#555; font-size:13px;");
    root->addWidget(m_avgLabel);

    // Buttons
    auto *btnRow = new QHBoxLayout();
    m_deleteBtn = new QPushButton("🗑 Видалити вибране", this);
    m_deleteBtn->setEnabled(false);
    m_deleteBtn->setFixedHeight(38);
    m_deleteBtn->setStyleSheet(
        "QPushButton { background:#f44336; color:white; border-radius:5px; }"
        "QPushButton:hover { background:#c62828; }"
        "QPushButton:disabled { background:#FFCDD2; color:#aaa; }"
    );
    connect(m_deleteBtn, &QPushButton::clicked, this, &HistoryWidget::onDelete);

    auto *backBtn = new QPushButton("← Назад", this);
    backBtn->setFixedHeight(38);
    backBtn->setStyleSheet(
        "QPushButton { background:#9E9E9E; color:white; border-radius:5px; }"
        "QPushButton:hover { background:#616161; }"
    );
    connect(backBtn, &QPushButton::clicked, this, &HistoryWidget::backRequested);

    btnRow->addWidget(m_deleteBtn);
    btnRow->addStretch();
    btnRow->addWidget(backBtn);
    root->addLayout(btnRow);
}

void HistoryWidget::setSessions(const QList<Session> &sessions)
{
    m_sessions = sessions;
    m_table->setRowCount(0);

    bool empty = sessions.isEmpty();
    m_emptyLabel->setVisible(empty);
    m_table->setVisible(!empty);

    static const QMap<QString,QString> opName = {
        {"+","Додавання"},{"-","Віднімання"},{"*","Множення"},{"/","Ділення"},{"mix","Змішано"}
    };
    static const QStringList diffName = {"", "Легкий", "Середній", "Важкий"};

    for (const Session &s : sessions) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(s.date.toString("dd.MM.yyyy HH:mm")));
        m_table->setItem(row, 1, new QTableWidgetItem(opName.value(s.operationType, s.operationType)));
        m_table->setItem(row, 2, new QTableWidgetItem(diffName.value(qBound(0,s.difficulty,3))));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(s.correctCount)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(s.totalCount)));

        int acc = StatsCalculator::accuracy(s);
        auto *accItem = new QTableWidgetItem(QString("%1%").arg(acc));
        accItem->setForeground(acc >= 70 ? QColor("#2E7D32") : (acc >= 50 ? QColor("#E65100") : QColor("#c62828")));
        m_table->setItem(row, 5, accItem);

        // Store session id in first column's UserRole
        m_table->item(row, 0)->setData(Qt::UserRole, s.id);
    }

    if (!empty) {
        double avg = StatsCalculator::averageAccuracy(sessions);
        m_avgLabel->setText(QString("Середній результат за всі сесії: %1%").arg(static_cast<int>(avg)));
    } else {
        m_avgLabel->setText("Середній результат: —");
    }

    m_deleteBtn->setEnabled(false);
}

void HistoryWidget::onSelectionChanged()
{
    m_deleteBtn->setEnabled(!m_table->selectedItems().isEmpty());
}

void HistoryWidget::onDelete()
{
    int row = m_table->currentRow();
    if (row < 0) return;

    int id = m_table->item(row, 0)->data(Qt::UserRole).toInt();
    int ret = QMessageBox::question(this, "Видалити запис",
        "Видалити вибрану сесію з історії?",
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        emit deleteRequested(id);
}
