#pragma once

#include "volume/volumes.h"

#include "core/definitions.h"
#include "core/module_qt.h"
#include "core/plugin_base.h"

#include "teamspeak/public_definitions.h"

#include <QtCore/QObject>
#include <QtCore/QSet>

namespace thorwe
{

using namespace com::teamspeak;

class ChannelMuter : public Module_Qt, public InfoDataInterface, public ContextMenuInterface
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

    int ParseCommand(uint64 serverConnectionHandlerID, const QString &cmd, const QStringList &args);

    void on_connect_status_changed(connection_id_t, int new_status, uint32_t error_number);

  public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID,
                            PluginMenuType type,
                            int menuItemID,
                            uint64 selectedItemID);

  private:
    thorwe::volume::Volumes<> m_volumes;

    QSet<QPair<uint64, uint64>> MutedChannels;
    QSet<QPair<uint64, anyID>> ClientWhiteList;

    int m_ContextMenuIdToggleChannelMute = -1;
    int m_ContextMenuToggleClientWhitelisted = -1;
};

}  // namespace thorwe
