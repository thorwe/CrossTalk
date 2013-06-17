#ifndef SETTINGS_RADIO_H
#define SETTINGS_RADIO_H

#include <QObject>

#include "ts_context_menu_qt.h"
#include "config_radio.h"
#include "mod_radio.h"

class SettingsRadio : public QObject, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(ContextMenuInterface)

public:
    static SettingsRadio* instance()
    {
        static QMutex mutex;
        if(!m_Instance)
        {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new SettingsRadio;

            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }
    
    void Init(Radio *radio);

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

    void settingsSave();

public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID);
    void onMenusInitialized();

private slots:
    void saveSettings(int);

private:
    explicit SettingsRadio();
    ~SettingsRadio();
    static SettingsRadio* m_Instance;
    SettingsRadio(const SettingsRadio &);
    SettingsRadio& operator=(const SettingsRadio &);

    int m_ContextMenuUi;
    QPointer<ConfigRadio> config;

    QPointer<Radio> mP_radio;

};

#endif // SETTINGS_RADIO_H
