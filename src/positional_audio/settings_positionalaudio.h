#ifndef SETTINGS_POSITIONALAUDIO_H
#define SETTINGS_POSITIONALAUDIO_H

#include <QObject>

#include "ts_context_menu_qt.h"
#include "config_positionalaudio.h"
#include "mod_positionalaudio.h"

#include <QMessageBox>

class SettingsPositionalAudio : public QObject, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(ContextMenuInterface)

public:
    static SettingsPositionalAudio* instance()
    {
        static QMutex mutex;
        if(!m_Instance)
        {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new SettingsPositionalAudio;

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

    void Init(PositionalAudio *positionalAudio);

signals:
    void EnabledSet(bool);
    void UseCameraSet(bool);

    void ServerSettingsAdd(QString,QString);
    void ServerSettingsRemove(QString,QString);
    void ServerEnabledSet(QString,bool);
    void ServerBlockSet(QString,bool);
    void ServerSendIntervalChange(QString,float);
    void ServerSendIntervalSilentIncChange(QString,float);

    void settingsSave();

    // to UI
    void UpdateUIEnabledSet(bool);
    void UpdateUIUseCameraSet(bool);

    void UpdateUIServerSettingsAdd(QString,QString);
    void UpdateUIServerSettingsRemove(QString,QString);
    void UpdateUIServerEnabled(QString,bool);
    void UpdateUIServerBlock(QString,bool);
    void UpdateUIServerSendInterval(QString,float);
    void UpdateUIServerSendIntervalSilentInc(QString,float);

    void UpdateUIServerSelect(QString);

    void UpdateUIStatusSelfName(QString);
    void UpdateUIStatusSelfGame(QString);

public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID);
    void onMenusInitialized();

    void AddCustomServerSettingForCurrentTab();
    void AddCustomServerSetting(uint64 serverConnectionHandlerID);
    void RemoveCustomServerSettingByServerUniqueId(QString serverUniqueId);
    void SetServerBlock(QString serverUniqueId, bool val, uint64 serverConnectionHandlerID = (uint64)NULL);

private slots:
    void saveSettings(int r);

    void serverBlockMsgBoxClosed(QAbstractButton* button);

private:
    explicit SettingsPositionalAudio();
    ~SettingsPositionalAudio();
    static SettingsPositionalAudio* m_Instance;
    SettingsPositionalAudio(const SettingsPositionalAudio &);
    SettingsPositionalAudio& operator=(const SettingsPositionalAudio &);

    int m_ContextMenuUi;
    QPointer<ConfigPositionalAudio> config;

    QPointer<PositionalAudio> mP_positionalAudio;

    QPointer<QMessageBox> mP_serverBlockMsgBox;
};

#endif // SETTINGS_POSITIONALAUDIO_H
