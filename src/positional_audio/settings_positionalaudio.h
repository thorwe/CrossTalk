#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QMessageBox>

#include "core/definitions.h"
#include "core/ts_context_menu_qt.h"
#include "core/ts_serversinfo.h"

#include "config_positionalaudio.h"
#include "mod_positionalaudio.h"

namespace thorwe
{

using namespace com::teamspeak;

class SettingsPositionalAudio : public QObject, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(ContextMenuInterface)

public:
    explicit SettingsPositionalAudio(TSServersInfo& servers_info, QObject* parent = nullptr);

    void Init(PositionalAudio *positionalAudio);
    void shutdown();

signals:
    void EnabledSet(bool);
    void UseCameraSet(bool);
    void UseAttenuationSet(bool);
    void AttenuationSet(bool);
    void DistanceMinChanged(int);
    void DistanceMaxChanged(int);
    void RollOffChanged(float);
    void RollOffMaxChanged(float);

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
    void UpdateUIUseAttenuationSet(bool);
    void UpdateUIDistanceMin(int);
    void UpdateUIDistanceMax(int);
    void UpdateUIRollOff(float);
    void UpdateUIRollOffMax(float);

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
    TSServersInfo& m_servers_info;

    int m_ContextMenuUi;
#ifdef Q_OS_WIN
    int m_ContextMenuGW2Map{0};
#endif
    QPointer<ConfigPositionalAudio> config;

    QPointer<PositionalAudio> mP_positionalAudio;

    QPointer<QMessageBox> mP_serverBlockMsgBox;
};

}  // namespace thorwe
