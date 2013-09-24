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

    void onCFValueChanged(double val);
    void onBWValueChanged(double val);
    void onDestrValueChanged(double val);
    void onRingModFrequencyValueChanged(double val);

signals:
    void EnabledSet(QString,bool);
    void CenterFrequencySet(QString,double);
    void BandWidthSet(QString,double);
    void DestructionSet(QString,double);
    void RingModFrequencySet(QString,double);

private slots:
    void onToggled(bool val);
    void onCFDialValueChanged(int val);
    void onBWDialValueChanged(int val);
    void onDestrDialValueChanged(int val);
    void onRingModFrequencyDialValueChanged(int val);

private:
    Ui::ConfigRadioGroupBox *ui;
};

#endif // CONFIG_RADIO_GROUPBOX_H
