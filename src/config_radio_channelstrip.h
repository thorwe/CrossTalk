#ifndef CONFIG_RADIO_CHANNELSTRIP_H
#define CONFIG_RADIO_CHANNELSTRIP_H

#include <QWidget>

namespace Ui {
class ConfigRadioChannelStrip;
}

class ConfigRadioChannelStrip : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConfigRadioChannelStrip(QWidget *parent = 0);
    ~ConfigRadioChannelStrip();
    
private:
    Ui::ConfigRadioChannelStrip *ui;
};

#endif // CONFIG_RADIO_CHANNELSTRIP_H
