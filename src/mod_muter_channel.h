#pragma once

#include <QtCore/QObject>
#include <QtCore/QSet>
#include "teamspeak/public_definitions.h"
#include "core/module.h"
#include "volume/volumes.h"
#include "core/plugin_base.h"

class ChannelMuter : public Module, public InfoDataInterface, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(InfoDataInterface ContextMenuInterface)

protected:
    void onRunningStateChanged(bool value);

public:
    ChannelMuter(Plugin_Base& plugin);

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

public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID);

private:
    Volumes* vols;

    QSet<QPair<uint64,uint64> > MutedChannels;
    QSet<QPair<uint64,anyID> > ClientWhiteList;

    int m_ContextMenuIdToggleChannelMute = -1;
    int m_ContextMenuToggleClientWhitelisted = -1;
};
