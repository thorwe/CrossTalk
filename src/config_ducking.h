#ifndef CONFIG_DUCKING_H
#define CONFIG_DUCKING_H

#include <QDialog>

namespace Ui {
class ConfigDucking;
}

class ConfigDucking : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigDucking(QWidget *parent = 0);
    ~ConfigDucking();

    // For Settings initialization and updating from other sources of interaction
    void UpdateGlobalDuckerEnabled(bool);
    void UpdateGlobalDuckerValue(float);
    void UpdateChannelDuckerEnabled(bool);
    void UpdateChannelDuckerValue(float);
    void UpdateChannelDuckerReverse(bool);
    void UpdateChannelDuckerDuckPSEnabled(bool);

signals:
    void globalDuckerEnabledSet(bool);
    void globalDuckerValueChanged(float);

    void channelDuckerEnabledSet(bool);
    void channelDuckerValueChanged(float);
    void channelDuckerReverseSet(bool);
    void channelDuckerDuckPSEnabledSet(bool);

private slots:
    void onChannelDuckingDuckModeSliderValueChanged(int val);

private:
    Ui::ConfigDucking *ui;
};

#endif // CONFIG_DUCKING_H
