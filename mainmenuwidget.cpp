#pragma once
#include <QWidget>
#include "data/datamodels.h"

class QComboBox;
class QSpinBox;
class QLabel;

class MainMenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
    void applySettings(const Settings &s);

signals:
    void startTraining(const Settings &settings);
    void showHistory();

private slots:
    void onStartClicked();

private:
    QComboBox *m_opCombo;
    QComboBox *m_diffCombo;
    QSpinBox  *m_countSpin;
};
