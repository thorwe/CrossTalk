#include "plugin_qt.h"

#include "config.h"
#include "mod_agmu.h"
#include "mod_muter_channel.h"
#include "mod_position_spread.h"
#include "positional_audio/mod_positionalaudio.h"
#include "positional_audio/settings_positionalaudio.h"
#include "settings_position_spread.h"
#include "snt.h"

#include "plugin.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"
#include "core/ts_logging_qt.h"
#include "core/ts_serversinfo.h"

#include "teamspeak/public_definitions.h"

using namespace com::teamspeak;
using namespace com::teamspeak::pluginsdk;

namespace thorwe
{

const char *Plugin::kPluginName = "CrossTalk";
const char *Plugin::kPluginVersion = "1.8.1";
const char *Plugin::kPluginAuthor = "Thorsten Weinz";
const char *Plugin::kPluginDescription =
"Features:\n\nPositional Audio, Stereo Position Spread, Switch\'n\'Talk (Cross Server Tab PTT)\nFor "
"information on the modules, use the \'?\' button on the topper right of the plugin settings window or visit "
"the Github wiki by clicking on the banner.";

Plugin::Plugin(const char *plugin_id, QObject *parent)
    : Plugin_Base(plugin_id, parent)
    , m_servers_info(new TSServersInfo(this))
    , m_channel_muter(new ChannelMuter(*this))
    , m_position_spread(new PositionSpread(*this))
    , m_settings_position_spread(new SettingsPositionSpread(this))
    , m_agmu(new Agmu(*this))
    , m_positional_audio(new thorwe::PositionalAudio(*this))
    , m_settings_positional_audio(new SettingsPositionalAudio(*m_servers_info, this))
    , m_snt(new SnT(*m_servers_info, this))
{}

/* required functions */

int Plugin::initialize()
{
    context_menu().setMainIcon("ct_16x16.png");
    translator().update();

    m_agmu->setEnabled(false);

    m_settings_position_spread->Init(m_position_spread);

    {
        QSettings cfg(TSHelpers::GetPath(teamspeak::plugin::Path::PluginIni), QSettings::IniFormat);
        m_websocket_server = new ServerThreaded();
        bool ok;
        const auto kPort = cfg.value("server_port", 64734).toUInt(&ok);
        if (!ok)
            TSLogging::Error("Could not read websocket server port from settings");
        else
        {
            m_websocket_server->setPort(kPort);
            m_websocket_server->setEnabled(cfg.value("server_enabled", false).toBool());
        }
    }

    m_settings_positional_audio->Init(m_positional_audio);
    m_channel_muter->setEnabled(true);

    return 0;
}

void Plugin::shutdown()
{
    m_settings_positional_audio->shutdown();
    m_settings_position_spread->shutdown();
}

void Plugin::configure(void * handle, void * qParentWidget)
{
    Q_UNUSED(handle);

    auto config = new Config((QWidget*)qParentWidget);
    config->connect(config, &Config::serverEnabledToggled, m_websocket_server, &ServerThreaded::setEnabled, Qt::UniqueConnection);
    config->connect(config, &Config::serverPortChanged, m_websocket_server, &ServerThreaded::setPort, Qt::UniqueConnection);
    config->exec();
}

/* optional functions */

int Plugin::process_command(uint64 connection_id, const QString &command, QStringList &args)
{
    auto ret = m_channel_muter->ParseCommand(connection_id, command, args);
    if (ret != 0)
    {
        ret = m_position_spread->ParseCommand(connection_id, command, args);
        if (ret != 0)
        {
            m_snt->ParseCommand(connection_id, command, args);
            ret = 1;    // ToDo: Suffer through snt.ParseCommand to return an int
        }
    }
    return ret;
}

void Plugin::on_current_server_connection_changed(uint64 connection_id)
{
    m_position_spread->setHomeId(connection_id);
}

void Plugin::on_connect_status_changed(uint64 connection_id, int new_status, unsigned int error_number)
{
    m_servers_info->onConnectStatusChangeEvent(connection_id, new_status, error_number);
    m_channel_muter->on_connect_status_changed(connection_id, new_status, error_number);
    m_positional_audio->on_connect_status_changed(connection_id, new_status, error_number);
}

void Plugin::on_client_move(uint64 connection_id,
                            anyID client_id,
                            uint64 old_channel_id,
                            uint64 new_channel_id,
                            int visibility,
                            anyID my_id,
                            const char *move_message)
{
    Q_UNUSED(move_message);
    m_channel_muter->onClientMoveEvent(connection_id, client_id, old_channel_id, new_channel_id, visibility,
                                       my_id);
    m_positional_audio->onClientMoveEvent(connection_id, client_id, old_channel_id, new_channel_id,
                                          visibility, my_id);
}

void Plugin::on_client_move_timeout(
uint64 connection_id, anyID client_id, uint64 old_channel_id, anyID my_id, const char *timeout_message)
{
    Q_UNUSED(timeout_message);
    m_channel_muter->onClientMoveEvent(connection_id, client_id, old_channel_id, 0, LEAVE_VISIBILITY, my_id);
    m_positional_audio->onClientMoveEvent(connection_id, client_id, old_channel_id, 0, LEAVE_VISIBILITY,
                                          my_id);
}

void Plugin::on_client_move_moved(uint64 connection_id,
                                  anyID client_id,
                                  uint64 old_channel_id,
                                  uint64 new_channel_id,
                                  int visibility,
                                  anyID my_id,
                                  anyID mover_id,
                                  const char *mover_name,
                                  const char *mover_unique_id,
                                  const char *move_message)
{
    Q_UNUSED(mover_id);
    Q_UNUSED(mover_name);
    Q_UNUSED(mover_unique_id);
    Q_UNUSED(move_message);
    m_channel_muter->onClientMoveEvent(connection_id, client_id, old_channel_id, new_channel_id, visibility,
                                       my_id);
    m_positional_audio->onClientMoveEvent(connection_id, client_id, old_channel_id, new_channel_id,
                                          visibility, my_id);
}

int Plugin::on_server_error(connection_id_t connection_id,
                            std::string_view error_message,
                            ts_errc error,
                            std::string_view return_code,
                            std::string_view extra_message)
{
    //TSLogging::Print(QString("onServerErrorEvent: %1 %2 %3").arg((returnCode ? returnCode : "")).arg(error).arg(errorMessage),serverConnectionHandlerID,LogLevel_DEBUG);
    if (!return_code.empty())
    {
        //TSLogging::Print("have returnCode");
        //        /* A plugin could now check the returnCode with previously (when calling a function) remembered returnCodes and react accordingly */
        //        /* In case of using a a plugin return code, the plugin can return:
        //         * 0: Client will continue handling this error (print to chat tab)
        //         * 1: Client will ignore this error, the plugin announces it has handled it */

        if (m_snt->getReturnCode() == return_code)
            m_snt->onServerError(connection_id, error_message, error, return_code, extra_message);

        return 1;
    }


    if (m_positional_audio->running())
    {
        if (ts_errc::client_is_flooding == error)
        {
            const auto kSUId = m_servers_info->get_server_info(connection_id)->getUniqueId();
            m_settings_positional_audio->SetServerBlock(kSUId, true, connection_id);
        }
    }
    return 0;
}

void Plugin::on_talk_status_changed(
uint64 connection_id, int status, int is_received_whisper, anyID client_id, bool is_me)
{
    if (m_channel_muter->onTalkStatusChanged(connection_id, status, is_received_whisper, client_id, is_me))
        return; //Client is muted;

    m_agmu->onTalkStatusChanged(connection_id, status, is_received_whisper, client_id, is_me);
    m_position_spread->onTalkStatusChanged(connection_id, status, is_received_whisper, client_id, is_me);
}

void Plugin::on_playback_pre_process(
uint64 connection_id, anyID client_id, short *samples, int frame_count, int channels)
{
    if (m_channel_muter->onEditPlaybackVoiceDataEvent(connection_id, client_id, samples, frame_count,
                                                      channels))
        return; //Client is muted;

    m_agmu->onEditPlaybackVoiceDataEvent(connection_id, client_id, samples, frame_count, channels);
}

void Plugin::on_playback_post_process(uint64 connection_id,
                                      anyID client_id,
                                      gsl::span<int16_t> samples,
                                      int channels,
                                      const unsigned int *channel_speaker_array,
                                      unsigned int *channel_fill_mask)
{
    if (m_positional_audio->isPositioned(client_id))
        return;

    m_position_spread->on_playback_post_process(connection_id, client_id, samples, channels,
                                                channel_speaker_array, channel_fill_mask);
}

void Plugin::on_custom_3d_rolloff_calculation(uint64 connection_id,
                                              anyID client_id,
                                              float distance,
                                              float *volume)
{
    //    TSLogging::Print(QString("Distance: %1 Volume: %2").arg(distance).arg(*volume));
    m_positional_audio->onCustom3dRolloffCalculationClientEvent(connection_id, client_id, distance, volume);
}

void Plugin::on_client_self_variable_update(uint64 connection_id,
                                            int flag,
                                            const char *old_value,
                                            const char *new_value)
{
    m_snt->onClientSelfVariableUpdateEvent(connection_id, flag, old_value, new_value);

    // seems to be equal to a ts3plugin_currentServerConnectionChanged when initiated by the user
    // except that this version will fire when returning from a cross server ptt event
    // (ts3plugin_currentServerConnectionChanged is not even fired when ptt-switching for whatever reason)
    // Furtunately, we can make use of that difference.
}

void Plugin::on_server_group_list(
uint64 connection_id, uint64 server_group_id, const char *name, int type, int icon_id, int save_db)
{
    m_servers_info->onServerGroupListEvent(connection_id, server_group_id, name, type, icon_id, save_db);
}

void Plugin::on_server_group_list_finished(uint64 connection_id)
{
    m_servers_info->onServerGroupListFinishedEvent(connection_id);
}

void Plugin::on_channel_group_list(
uint64 connection_id, uint64 channel_group_id, const char *name, int type, int icon_id, int save_db)
{
    m_servers_info->onChannelGroupListEvent(connection_id, channel_group_id, name, type, icon_id, save_db);
}

void Plugin::on_channel_group_list_finished(uint64 connection_id)
{
    m_servers_info->onChannelGroupListFinishedEvent(connection_id);
}

void Plugin::on_plugin_command(uint64 connection_id,
                               const char *plugin_name,
                               const char *plugin_command,
                               anyID invoker_client_id,
                               const char *invoker_name,
                               const char *invoker_uid)
{
    // pluginName is the dll name minus _someOS, not ts3plugin_name();
    // if the user for whatever reason renames the dll, this breaks the code
    {
        const auto kPluginNameInc = QString(plugin_name);
        const auto kPluginNameLower = QString(kPluginName).toLower();
        if (kPluginNameInc != kPluginNameLower && kPluginNameInc != kPluginNameLower + "_plugin")
            return;
    }

    const auto [error_my_id, my_id] = funcs::get_client_id(connection_id);
    if (error_my_id)
    {
        TSLogging::Error("(ts3plugin_onPluginCommandEvent)", connection_id, error_my_id);
        return;
    }

    QString cmd_qs(plugin_command);
    QTextStream args_qs(&cmd_qs);

    QString cmd;
    args_qs >> cmd;

    // note: args_qs leading white space if streaming to QByteArray or using readAll()
    if (!m_positional_audio->onPluginCommand(connection_id, invoker_client_id, (invoker_client_id == my_id),
                                             cmd, args_qs))
        TSLogging::Error("Error on plugin command", connection_id, ts_errc::ok, true);
}

}  // namespace thorwe
