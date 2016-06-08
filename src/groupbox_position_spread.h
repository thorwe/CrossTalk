#pragma once

#include <QGroupBox>

namespace Ui {
class GroupBoxPositionSpread;
}

class GroupBoxPositionSpread : public QGroupBox
{
    Q_OBJECT

public:
    explicit GroupBoxPositionSpread(QWidget *parent = 0);
    ~GroupBoxPositionSpread();

    // For Settings initialization and updating from other sources of interaction
    void UpdateEnabledSet(bool val);
    void UpdateSpreadWidth(float val);
    void UpdateExpertEnabledSet(bool val);
    void UpdateExpertHomeValueChanged(int val);
    void UpdateExpertWhisperValueChanged(int val);
    void UpdateExpertOtherValueChanged(int val);

signals:
    void EnabledSet(bool);
    void ValueChanged(float);
    void ExpertEnabledSet(bool);
    void ExpertHomeValueChanged(int);
    void ExpertWhisperValueChanged(int);
    void ExpertOtherValueChanged(int);

private slots:
    void onDialValueChanged(int val);
    void onSpinBoxValueChanged(double val);

private:
    Ui::GroupBoxPositionSpread *ui;
};
