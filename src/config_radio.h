#ifndef CONFIG_RADIO_H
#define CONFIG_RADIO_H

#include <QDialog>
#include <QSignalMapper>
#include <QMap>

#include "config_radio_groupbox.h"

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
    void UpdateEnabled(QString name, bool val);
    void UpdateBandpassLowFrequency(QString name, double val);
    void UpdateBandpassHighFrequency(QString name, double val);
    void UpdateDestruction(QString name, double val);
    void UpdateRingModFrequency(QString name, double val);

protected slots:

signals:
    void EnabledSet(QString,bool);
    void LowFrequencySet(QString,double);
    void HighFrequencySet(QString,double);
    void DestructionSet(QString,double);
    void RingModFrequencySet(QString,double);

private:
    Ui::ConfigRadio *ui;
    QMap<QString,ConfigRadioGroupBox*> m_ChannelStripMap;
    ConfigRadioGroupBox* GetChannelStrip(QString name);
};

#endif // CONFIG_RADIO_H
