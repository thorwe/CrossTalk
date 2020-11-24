#include "mod_muter_channel.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"

#include "plugin.h"

#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"

namespace thorwe
{

ChannelMuter::ChannelMuter(Plugin_Base &plugin)
{
    m_isPrintEnabled = false;
    this->setParent(&plugin);
    this->setObjectName(QStringLiteral("ChannelMuter"));
}

// User Setting interaction

void ChannelMuter::onRunningStateChanged(bool value)
{
    if (!value)  // this module is always active, used solely as init function
        return;

    auto *plugin = qobject_cast<Plugin_Base *>(parent());
    auto &context_menu = plugin->context_menu();
    if (m_ContextMenuIdToggleChannelMute == -1)
    {
        m_ContextMenuIdToggleChannelMute =
        context_menu.Register(this, PLUGIN_MENU_TYPE_CHANNEL, "Toggle Channel Mute [temp]", "");
        connect(&context_menu, &TSContextMenu::FireContextMenuEvent, this, &ChannelMuter::onContextMenuEvent,
                Qt::AutoConnection);
    }

    if (m_ContextMenuToggleClientWhitelisted == -1)
        m_ContextMenuToggleClientWhitelisted =
        context_menu.Register(this, PLUGIN_MENU_TYPE_CLIENT, "Toggle ChannelMuter Whitelisting [temp]", "");

    auto &info_data = plugin->info_data();
    info_data.Register(this, value, 1);

    Log(QString("enabled: %1").arg((value) ? QStringLiteral("true") : QStringLiteral("false")));
}

//! Toggles a channels mute status on a server tab
/*!
 * \brief ChannelMuter::toggleChannelMute Toggles a channels mute status on a server tab
 * \param connection_id the server tab
 * \param channel_id the channel id
 * \return true if the toggling results in active muting, otherwise false
 */
auto ChannelMuter::toggleChannelMute(uint64 connection_id, uint64 channel_id) -> bool
{

    if (!connection_id)
    {
        connection_id = TSHelpers::GetActiveServerConnectionHandlerID();
        if (!connection_id)
            return false;
    }

    const auto [error_my_id, my_id] = pluginsdk::funcs::get_client_id(connection_id);
    if (error_my_id)
        Error("(toggleChannelMute) Error getting my client id", connection_id, error_my_id);
    else
    {
        const auto [error_my_channel, my_channel_id] =
        pluginsdk::funcs::get_channel_of_client(connection_id, my_id);
        if (error_my_channel)
            Error("(toggleChannelMute) Error getting Client Channel Id", connection_id, error_my_channel);
        else
        {
            const auto target_channel_id = channel_id ? channel_id : my_channel_id;

            auto newPair = qMakePair(connection_id, target_channel_id);
            if (!(MutedChannels.contains(newPair)))
                MutedChannels.insert(newPair);
            else
                MutedChannels.remove(newPair);

            // only if it's my current channel / hotkey immediate action is necessary
            if (target_channel_id == my_channel_id)
            {
                // Get Channel Client List
                const auto [error_channel_clients, my_channel_clients] =
                pluginsdk::funcs::get_channel_client_ids(connection_id, target_channel_id);
                if (error_channel_clients)
                {
                    Error("(toggleChannelMute) Error getting Client Channel List", connection_id,
                          error_channel_clients);
                }
                else
                {
                    for (const auto client_id : my_channel_clients)
                    {
                        // Iterate and push fake onTalkStatusChanged events to the module
                        if (client_id == my_id)
                            continue;

                        int talk_status = STATUS_NOT_TALKING;
                        int is_received_whisper = 0;
                        const auto error_talk_status =
                        TSHelpers::GetTalkStatus(connection_id, client_id, talk_status, is_received_whisper);
                        if (error_talk_status)
                        {
                            Error("(toggleChannelMute) Error receiving client talk status", connection_id,
                                  error_talk_status);
                        }
                        else
                            onTalkStatusChanged(connection_id, talk_status, (is_received_whisper == 1),
                                                client_id, false);
                    }

                    // for info update on hotkey
                    if (!channel_id)
                    {
                        auto *plugin = qobject_cast<Plugin_Base *>(parent());
                        auto &info_data = plugin->info_data();
                        info_data.RequestUpdate(connection_id, target_channel_id, PLUGIN_CHANNEL);
                    }
                }
            }
            return (MutedChannels.contains(newPair));
        }
    }
    return false;
}

//! Is a channel muted on the server tab?
/*!
 * \brief ChannelMuter::isChannelMuted Is a channel muted on the server tab?
 * \param connection_id the server tab
 * \param channel_id the channel id
 * \return true if muted
 */
auto ChannelMuter::isChannelMuted(uint64 connection_id, uint64 channel_id) -> bool
{
    auto newPair = qMakePair(connection_id, channel_id);
    return (MutedChannels.contains(newPair));
}

//! toggles a client whitelist status, receiver of some user interaction
/*!
 * \brief ChannelMuter::toggleClientWhitelisted toggles a client whitelist status, receiver of some user
 * interaction \param connection_id ther server tab \param client_id the client id \return true if the action
 * results in whitelisting, otherwise false
 */
auto ChannelMuter::toggleClientWhitelisted(uint64 connection_id, anyID client_id) -> bool
{
    // Log(QString("(toggleClientWhitelisted) %1").arg(client_id),connection_id,LogLevel_DEBUG);
    QPair<uint64, anyID> newPair = qMakePair(connection_id, client_id);
    if (!(ClientWhiteList.contains(newPair)))
        ClientWhiteList.insert(newPair);
    else
        ClientWhiteList.remove(newPair);

    int talk_status = TalkStatus::STATUS_NOT_TALKING;
    ;
    auto is_received_whisper = int{0};
    const auto error_talk_status =
    TSHelpers::GetTalkStatus(connection_id, client_id, talk_status, is_received_whisper);
    if (error_talk_status)
    {
        Error("(toggleClientWhitelisted) Error receiving client talk status", connection_id,
              error_talk_status);
    }
    else
        onTalkStatusChanged(connection_id, talk_status, (is_received_whisper == 1), client_id, false);

    return (ClientWhiteList.contains(newPair));
}

//! Is a client whitelisted on a connection_id?
/*!
 * \brief ChannelMuter::isClientWhitelisted Is a client whitelisted on a connection_id?
 * \param connection_id the server tab
 * \param client_id the client id
 * \return true if whitelisted
 */
auto ChannelMuter::isClientWhitelisted(uint64 connection_id, anyID client_id) -> bool
{
    auto newPair = qMakePair(connection_id, client_id);
    return (ClientWhiteList.contains(newPair));
}

// ts event handlers

//! Book keeping of volume objects when clients switch channels etc.
/*!
 * \brief ChannelMuter::onClientMoveEvent Book keeping of volume objects when clients switch channels etc.
 * \param connection_id the server tab
 * \param client_id the client
 * \param old_channel_id the...old channel
 * \param new_channel_id the...new channel!
 * \param visibility unused here
 */
void ChannelMuter::onClientMoveEvent(uint64 connection_id,
                                     anyID client_id,
                                     uint64 old_channel_id,
                                     uint64 new_channel_id,
                                     int visibility,
                                     anyID my_id)
{
    Q_UNUSED(visibility);

    if (client_id == my_id)  // I have switched channels
    {
        m_volumes.delete_items(connection_id);

        // Get Channel Client List
        const auto [error_channel_clients, channel_client_ids] =
        pluginsdk::funcs::get_channel_client_ids(connection_id, new_channel_id);
        if (error_channel_clients)
        {
            Error("(onClientMoveEvent): Error getting Channel Client List", connection_id,
                  error_channel_clients);
        }
        else
        {
            // for every client insert volume
            for (const auto channel_client_id : channel_client_ids)
            {
                if (channel_client_id == my_id)
                    continue;

                m_volumes.add_volume(connection_id, channel_client_id);
            }
        }
    }
    else  // Someone else has...
    {
        // Get My channel on this handler
        const auto [error_my_channel, my_channel_id] =
        pluginsdk::funcs::get_channel_of_client(connection_id, my_id);
        if (error_my_channel)
            Error("(onClientMoveEvent) Error getting Client Channel Id", connection_id, error_my_channel);
        else
        {
            if (my_channel_id == old_channel_id)  // left
                m_volumes.delete_item(connection_id, client_id);
            else if (my_channel_id == new_channel_id)  // joined
                m_volumes.add_volume(connection_id, client_id);
        }
    }
}

//! Set the status of volume object based on the talk status
/*!
 * \brief ChannelMuter::onTalkStatusChanged Set the status of volume object based on the talk status
 * \param connection_id the server tab
 * \param status -> enum TalkStatus
 * \param is_received_whisper is it a whisper?
 * \param client_id the client changing its talk status
 */
auto ChannelMuter::onTalkStatusChanged(
uint64 connection_id, int status, bool is_received_whisper, anyID client_id, bool isMe) -> bool
{
    if (!running())
        return false;

    if (is_received_whisper)
        return false;

    if (isMe)
        return false;

    if ((status == STATUS_TALKING) || (status == STATUS_NOT_TALKING))
    {
        const auto [error_channel, channel_id] =
        pluginsdk::funcs::get_channel_of_client(connection_id, client_id);
        if (error_channel)
        {
            if (ts_errc::not_connected != error_channel)
            {
                Error("(onTalkStatusChanged) Error getting Client Channel Id", connection_id, error_channel);
            }
        }
        else
        {
            auto channelPair = qMakePair(connection_id, channel_id);
            auto clientPair = qMakePair(connection_id, client_id);
            const bool mute = MutedChannels.contains(channelPair) && !(ClientWhiteList.contains(clientPair));
            const bool set_processing = status == STATUS_TALKING;
            m_volumes.do_for(
            [mute, set_processing](DspVolume *volume) {
                if (!volume)
                    return;

                volume->set_muted(mute);
                volume->set_processing(set_processing);
            },
            connection_id, client_id);
            return mute;
        }
    }
    return false;
}

auto ChannelMuter::onInfoDataChanged(
uint64 connection_id, uint64 id, PluginItemType type, uint64 mine, QTextStream &data) -> bool
{
    auto isDirty = false;
    if (type == PLUGIN_CLIENT)
    {
        if (m_ContextMenuToggleClientWhitelisted != -1)
        {
            auto *plugin = qobject_cast<Plugin_Base *>(parent());
            pluginsdk::funcs::set_plugin_menu_enabled(plugin->id(), m_ContextMenuToggleClientWhitelisted,
                                                      id != mine);
        }

        if ((id != mine) && isClientWhitelisted(connection_id, (anyID) id))
        {
            data << this->objectName() << ":";
            isDirty = true;
            data << "whitelisted [temp]";
        }
    }
    else if (type == PLUGIN_CHANNEL)
    {
        if (isChannelMuted(connection_id, id))
        {
            data << this->objectName() << ":";
            isDirty = true;
            data << "channel muted [temp]";
        }
    }
    return isDirty;
}

void ChannelMuter::onContextMenuEvent(uint64 connection_id,
                                      PluginMenuType type,
                                      int menuItemID,
                                      uint64 selectedItemID)
{
    Q_UNUSED(type);
    if (menuItemID == m_ContextMenuIdToggleChannelMute)
        toggleChannelMute(connection_id, selectedItemID);
    else if (menuItemID == m_ContextMenuToggleClientWhitelisted)
        toggleClientWhitelisted(connection_id, (anyID) selectedItemID);
}

//! Routes the arguments of the event to the corresponding volume object
/*!
 * \brief ChannelMuter::onEditPlaybackVoiceDataEvent pre-processing voice event
 * \param connection_id the connection id of the server
 * \param client_id the client-side runtime-id of the sender
 * \param samples the sample array to manipulate
 * \param sampleCount amount of samples in the array
 * \param channels amount of channels
 * \return true if the stream is muted, so succeeding modules need not bother
 */
auto ChannelMuter::onEditPlaybackVoiceDataEvent(
uint64 connection_id, anyID client_id, short *samples, int sampleCount, int channels) -> bool
{
    if (!running())
        return false;

    return m_volumes.do_for(
    [samples, sampleCount, channels](DspVolume *volume) {
        if (!volume)
            return false;

        volume->process(gsl::span<int16_t>{samples, static_cast<size_t>(sampleCount * channels)}, channels);
        return volume->muted();
    },
    connection_id, client_id);
}

auto ChannelMuter::ParseCommand(uint64 connection_id, const QString &cmd, const QStringList &args) -> int
{
    if ((cmd.compare("CHANNEL_MUTER", Qt::CaseInsensitive)) != 0)
        return 1;

    auto channel_id = channel_id_t{0};

    if (!args.isEmpty())
    {
        // TODO: first argument toggle/whitelist etc.
        // second argument server name
        // third argument channel name
    }

    toggleChannelMute(connection_id, channel_id);
    return 0;
}

void ChannelMuter::on_connect_status_changed(connection_id_t connection_id,
                                             int new_status,
                                             uint32_t error_number)
{
    m_volumes.onConnectStatusChanged(connection_id, new_status, error_number);
}

}  // namespace thorwe
