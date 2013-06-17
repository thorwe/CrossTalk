#ifndef SETTINGS_DUCK_H
#define SETTINGS_DUCK_H

#include <QObject>
#include <QtCore>

#include "ts_context_menu_qt.h"
#include "config_ducking.h"
#include "mod_ducker_global.h"
#include "mod_ducker_channel.h"

class SettingsDuck : public QObject, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(ContextMenuInterface)

public:
    static SettingsDuck* instance()
    {
        static QMutex mutex;
        if(!m_Instance)
        {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new SettingsDuck;

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

    void Init(Ducker_Global *ducker_G, Ducker_Channel *ducker_C);

signals:
    void globalDuckerEnabledSet(bool);
    void globalDuckerValueChanged(float);
    void channelDuckerEnabledSet(bool);
    void channelDuckerValueChanged(float);
    void channelDuckerReverseSet(bool);

    void settingsSave();

public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID);
    void onMenusInitialized();

//    void loadSettings();

private slots:
    void saveSettings(int);

private:
    explicit SettingsDuck();
    ~SettingsDuck();
    static SettingsDuck* m_Instance;
    SettingsDuck(const SettingsDuck &);
    SettingsDuck& operator=(const SettingsDuck &);

    int m_ContextMenuUi;
    QPointer<ConfigDucking> config;

    QPointer<Ducker_Global> mP_ducker_G;
    QPointer<Ducker_Channel> mP_ducker_C;
};

#endif // SETTINGS_DUCK_H
