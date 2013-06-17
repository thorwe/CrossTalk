#ifndef CONFIG_RADIO_H
#define CONFIG_RADIO_H

#include <QDialog>

namespace Ui {
class ConfigRadio;
}

class ConfigRadio : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigRadio(QWidget *parent = 0);
    ~ConfigRadio();

    // For Settings initialization and updating from other sources of interaction
    void UpdateHomeEnabled(bool val);
    void UpdateHomeLowFrequency(double val);
    void UpdateHomeHighFrequency(double val);
    void UpdateHomeDestruction(double val);

    void UpdateWhisperEnabled(bool val);
    void UpdateWhisperLowFrequency(double val);
    void UpdateWhisperHighFrequency(double val);
    void UpdateWhisperDestruction(double val);

    void UpdateOtherEnabled(bool val);
    void UpdateOtherLowFrequency(double val);
    void UpdateOtherHighFrequency(double val);
    void UpdateOtherDestruction(double val);

protected slots:

signals:
    void HomeEnabledSet(bool);
    void HomeLowFrequencySet(double);
    void HomeHighFrequencySet(double);
    void HomeDestructionSet(double);

    void WhisperEnabledSet(bool);
    void WhisperLowFrequencySet(double);
    void WhisperHighFrequencySet(double);
    void WhisperDestructionSet(double);

    void OtherEnabledSet(bool);
    void OtherLowFrequencySet(double);
    void OtherHighFrequencySet(double);
    void OtherDestructionSet(double);

private:
    Ui::ConfigRadio *ui;
};

#endif // CONFIG_RADIO_H
