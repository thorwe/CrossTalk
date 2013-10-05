#ifndef MOD_MUTER_CHANNEL_H
#define MOD_MUTER_CHANNEL_H

#include <QObject>
#include "public_definitions.h"
#include "module.h"
#include "volumes.h"
//#include "simple_volume.h"
#include "ts_infodata_qt.h"
#include "ts_context_menu_qt.h"

class ChannelMuter : public Module, public InfoDataInterface, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(InfoDataInterface ContextMenuInterface)
protected:
    void onRunningStateChanged(bool value);
public:
    explicit ChannelMuter(QObject *parent = 0);

    // user interaction
    bool toggleChannelMute(uint64 serverConnectionHandlerID,uint64 channelID);
    bool isChannelMuted(uint64 serverConnectionHandlerID, uint64 channelID);
    bool toggleClientWhitelisted(uint64 serverConnectionHandlerID, anyID clientID);
    bool isClientWhitelisted(uint64 serverConnectionHandlerID, anyID clientID);

    // events forwarded from plugin.cpp
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID);
    bool onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

    bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe);

    bool onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, uint64 mine, QTextStream &data);

    int ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args);

signals:
    
public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID);

private:
    Volumes* vols;

    QSet<QPair<uint64,uint64> > MutedChannels;
    QSet<QPair<uint64,anyID> > ClientWhiteList;

    int m_ContextMenuIdToggleChannelMute;
    int m_ContextMenuToggleClientWhitelisted;
};

#endif // MOD_MUTER_CHANNEL_H
