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
    void onCFDialValueChanged(int val);
    void onCFValueChanged(double val);

    void onBWDialValueChanged(int val);
    void onBWValueChanged(double val);

    void onDestrDialValueChanged(int val);
    void onDestrValueChanged(double val);

signals:
    void CenterFrequencySet(double);
    void BandWidthSet(double);
    void DestructionSet(double);

private:
    Ui::ConfigRadioGroupBox *ui;
};

#endif // CONFIG_RADIO_GROUPBOX_H
