#ifndef CONFIG_POSITION_SPREAD_H
#define CONFIG_POSITION_SPREAD_H

#include <QDialog>

namespace Ui {
class ConfigPositionSpread;
}

class ConfigPositionSpread : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigPositionSpread(QWidget *parent = 0);
    ~ConfigPositionSpread();

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
    Ui::ConfigPositionSpread *ui;
};

#endif // CONFIG_POSITION_SPREAD_H
