#ifndef CONFIG_RADIO_GROUPBOX_H
#define CONFIG_RADIO_GROUPBOX_H

#include <QGroupBox>

namespace Ui {
class ConfigRadioGroupBox;
}

class ConfigRadioGroupBox : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit ConfigRadioGroupBox(QWidget *parent = 0);
    ~ConfigRadioGroupBox();

public slots:

    void onInLoValueChanged(double val);
    void onInHiValueChanged(double val);
    void onDestrValueChanged(double val);
    void onRingModFrequencyValueChanged(double val);
    void onRingModMixValueChanged(double val);
    void onOutLoValueChanged(double val);
    void onOutHiValueChanged(double val);

signals:
    void EnabledSet(QString,bool);
    void InLoFreqSet(QString,double);
    void InHiFreqSet(QString,double);
    void DestructionSet(QString,double);
    void RingModFrequencySet(QString,double);
    void RingModMixSet(QString,double);
    void OutLoFreqSet(QString, double);
    void OutHiFreqSet(QString, double);

private slots:
    void onToggled(bool val);
    void onInLoDialValueChanged(int val);
    void onInHiDialValueChanged(int val);
    void onOutLoDialValueChanged(int val);
    void onOutHiDialValueChanged(int val);
    void onDestrDialValueChanged(int val);
    void onRingModFrequencyDialValueChanged(int val);
    void onRingModMixDialValueChanged(int val);

private:
    Ui::ConfigRadioGroupBox *ui;
};

#endif // CONFIG_RADIO_GROUPBOX_H
