#include "snt.h"

#include "ts_ptt_qt.h"

#include "plugin.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"
#include "core/ts_logging_qt.h"
#include "core/ts_serverinfo_qt.h"

#include "plugin_qt.h"

#include "teamspeak/public_rare_definitions.h"

using namespace com::teamspeak;
using namespace com::teamspeak::pluginsdk;

//! Constructor
/*!
 * \brief SnT::SnT Creates an instance of this class
 * \param parent optional Qt Object
 */
SnT::SnT(TSServersInfo &servers_info, QObject *parent)
    : QObject(parent)
    , m_servers_info(servers_info)
{
}

//! When the input hardware gets activated, check if there's a Ptt activation scheduled and if so, turn Ptt on
/*!
 * \brief SnT::onClientSelfVariableUpdateEvent equivalent TS Client SDK
 * \param connection_id equivalent TS Client SDK
 * \param flag equivalent TS Client SDK
 * \param oldValue equivalent TS Client SDK
 * \param newValue equivalent TS Client SDK
 */
void SnT::onClientSelfVariableUpdateEvent(uint64 connection_id,
                                          int flag,
                                          const char *oldValue,
                                          const char *newValue)
{
    Q_UNUSED(oldValue);

    if ((flag == CLIENT_INPUT_HARDWARE) && (strcmp(newValue, "1") == 0))
    {
        if (m_shallActivatePtt == true)
        {
            if (m_returnCodeScHandler == 0)
            {
                // TSLogging::Log("Activating Ptt from
                // onClientSelfVariableUpdateEvent",connection_id,LogLevel_DEBUG);
                TSPtt::instance()->SetPushToTalk(connection_id, PTT_ACTIVATE);
            }

            m_shallActivatePtt = false;
        }
        // seems to be equal to a ts3plugin_currentServerConnectionChanged WHEN initiated by the user
        // except that this version will fire when returning from a cross server ptt event
        // (ts3plugin_currentServerConnectionChanged is not even fired when ptt-switching for whatever reason)
        // Furtunately, we like that here.
    }
}

auto SnT::getReturnCode() const -> std::string
{
    return m_return_code;
}

//! Note that the primary intend is NOT to handle errors, but the "reply" from set whisper
void SnT::onServerError(uint64 connection_id,
                        std::string_view error_message,
                        ts_errc error,
                        std::string_view return_code,
                        std::string_view extra_message)
{
    Q_UNUSED(error_message);
    Q_UNUSED(extra_message);

    if (m_return_code != return_code)
        return;

    if (ts_errc::ok != error)
        TSLogging::Log("Whoops. That shouldn't happen that this is an error. But do we care? We don't.",
                       connection_id, LogLevel_DEBUG);

    if (m_returnCodeScHandler != connection_id)
        TSLogging::Log("Whoops. That shouldn't happen that those don't match. But do we care? We don't.",
                       connection_id, LogLevel_DEBUG);

    if (m_returnCodeScHandler == 0)
        TSLogging::Log("Whoops. That shouldn't happen that this is 0. But do we care? We don't.",
                       connection_id, LogLevel_DEBUG);

    if (!m_shallActivatePtt)
    {
        // TSLogging::Log("Activating Ptt from onServerReply",connection_id,LogLevel_DEBUG);
        TSPtt::instance()->SetPushToTalk(m_returnCodeScHandler, PTT_ACTIVATE);
    }

    m_returnCodeScHandler = 0;
}

//! Parse Plugin Commands for this module
/*!
 * \brief SnT::ParseCommand Qt Slot
 * \param connection_id the connection handler forwarded from the API
 * \param cmd the command
 * \param arg arguments (if any)
 */

void SnT::ParseCommand(uint64 connection_id, const QString &cmd, const QStringList &args)
{
    if (m_last_cmd == cmd)
        return;

    m_last_cmd = cmd;

    if (m_return_code.empty())
    {
        auto *plugin = qobject_cast<Plugin_Base *>(parent());
        m_return_code = funcs::create_return_code(plugin->id());
    }

    auto *ptt = TSPtt::instance();
    connect(ptt, &TSPtt::PttDelayFinished, this, &SnT::PttDelayFinished,
            Qt::UniqueConnection);  // UniqueConnection saving init

    // Get the active server
    auto scHandlerID = TSHelpers::GetActiveServerConnectionHandlerID();
    if (scHandlerID == NULL)
    {
        TSLogging::Log("Failed to get an active server, falling back to current server", LogLevel_DEBUG);
        scHandlerID = funcs::get_current_server_connection_handler_id();
        if (scHandlerID == NULL)
            return;
    }

    const auto [error_status, connection_status] = funcs::get_connection_status(scHandlerID);
    if (error_status)
        TSLogging::Error("Error retrieving connection status", scHandlerID, error_status);

    if (ConnectStatus::STATUS_DISCONNECTED == connection_status)
        return;

    /***** Communication *****/
    if (cmd == QLatin1String("TS3_PTT_ACTIVATE"))
        ptt->SetPushToTalk(scHandlerID, PTT_ACTIVATE);
    else if (cmd == QLatin1String("TS3_PTT_DEACTIVATE"))
        ptt->SetPushToTalk(scHandlerID, PTT_DEACTIVATE);
    else if (cmd == QLatin1String("TS3_PTT_TOGGLE"))
        ptt->SetPushToTalk(scHandlerID, PTT_TOGGLE);
    else if ((cmd == QLatin1String("TS3_SWITCH_N_TALK_END")) ||
             (cmd == QLatin1String("TS3_NEXT_TAB_AND_TALK_END")) ||
             (cmd == QLatin1String("TS3_NEXT_TAB_AND_WHISPER_END")))  // universal OnKeyUp Handler
    {
        ptt->SetPushToTalk(scHandlerID, false);  // always do immediately regardless of delay settings
        if (m_shallClearWhisper)
        {
            const auto error_set_whisperlist = funcs::request_client_set_whisperlist(scHandlerID, 0, {}, {});
            if (error_set_whisperlist)
                TSLogging::Error("Could not release whisperlist.", scHandlerID, error_set_whisperlist);
            else
                m_shallClearWhisper = false;
        }

        if (m_returnToSCHandler)
        {
            TSHelpers::SetActiveServer(m_returnToSCHandler);
            if ((cmd == QLatin1String("TS3_NEXT_TAB_AND_TALK_END")) ||
                (cmd == QLatin1String("TS3_NEXT_TAB_AND_WHISPER_END")))  // annoy user to upgrade
            {
                TSLogging::Error("Hotkeys Next Tab and Talk Stop & Next Tab and Whisper Stop are being "
                                 "DEPRECATED! Please use SnT Stop instead!",
                                 m_returnToSCHandler);
            }
            m_returnToSCHandler = 0;
        }
    }
    else if (cmd == "TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START")
    {
        auto nextServer = uint64{0};
        if (const auto error = TSHelpers::GetActiveServerRelative(scHandlerID, true, &nextServer); error)
            TSLogging::Error("Could not get next server.", scHandlerID, error);

        if (scHandlerID == nextServer)
            return;

        if (const auto error = TSHelpers::SetWhisperList(nextServer, GROUPWHISPERTYPE_CHANNELCOMMANDER,
                                                         GROUPWHISPERTARGETMODE_ALL);
            error)
        {
            if (ts_errc::ok_no_update != error)
                TSLogging::Error("Could not set whisperlist", scHandlerID, error);

            return;
        }

        ptt->SetPushToTalk(
        scHandlerID,
        false);  // always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt = true;
        m_returnToSCHandler = scHandlerID;
        m_shallClearWhisper = true;
        TSHelpers::SetActiveServer(nextServer);
    }
    else if (cmd == QLatin1String("TS3_NEXT_TAB_AND_TALK_START"))
    {
        auto nextServer = uint64{0};
        if (const auto error = TSHelpers::GetActiveServerRelative(scHandlerID, true, &nextServer); error)
            TSLogging::Error("Could not get next server.", scHandlerID, error);

        if (scHandlerID == nextServer)
            return;

        ptt->SetPushToTalk(
        scHandlerID,
        false);  // always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt = true;
        m_returnToSCHandler = scHandlerID;
        TSHelpers::SetActiveServer(nextServer);
    }
    else if (cmd == QLatin1String("TS3_SWITCH_TAB_AND_TALK_START"))
    {
        if (args.isEmpty())
        {
            TSLogging::Error("No target server specified.", connection_id, ts_errc::parameter_invalid);
            return;
        }
        QString name;
        if (args.count() == 1)
            name = args.at(0);
        else
            name = args.join(" ");

        uint64 targetServer = 0;
        if (const auto error = TSHelpers::GetServerHandler(name, &targetServer); error)
            return;

        if (0 == targetServer)
            return;

        ptt->SetPushToTalk(
        scHandlerID,
        false);  // always do immediately regardless of delay settings; maybe not as necessary as below

        if (scHandlerID != targetServer)
        {
            m_shallActivatePtt = true;
            m_returnToSCHandler = scHandlerID;
            TSHelpers::SetActiveServer(targetServer);
        }
        else
            ptt->SetPushToTalk(scHandlerID, true);
    }
    else if (cmd == QLatin1String("TS3_SWITCH_TAB_AND_WHISPER_START"))
    {
        if (args.count() < 3)
        {
            TSLogging::Error("Too few arguments.", scHandlerID);
            return;
        }

        const auto &name = args.at(0);
        const auto &groupWhisperTypeArg = args.at(1);
        const auto &groupWhisperTargetModeArg = args.at(2);
        auto arg = uint64_t{0};

        uint64 targetServer = 0;
        if (const auto error = TSHelpers::GetServerHandler(name, &targetServer); error)
        {
            TSLogging::Error("Could not get target server.", scHandlerID, error);
            return;
        }
        if (0 == targetServer)
            return;

        GroupWhisperType groupWhisperType = GetGroupWhisperType(groupWhisperTypeArg);
        if (groupWhisperType == GROUPWHISPERTYPE_ENDMARKER)
        {
            TSLogging::Error("Unsupported group whisper type.", scHandlerID, ts_errc::parameter_invalid);
            return;
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELGROUP)
        {
            if (args.count() == 4)
            {
                auto *server_info = m_servers_info.get_server_info(targetServer);
                if (!server_info)
                    return;

                if ((arg = server_info->GetChannelGroupId(args.at(3))) == 0)
                {
                    TSLogging::Error("Could not find channel group.");
                    return;
                }
            }
            else  // Get My Channel Group
            {
                if (const auto error = TSHelpers::GetClientChannelGroup(targetServer, &arg); error)
                    return;

                // Blacklist default channel group only with no arg
                auto *server_info = m_servers_info.get_server_info(targetServer);
                if (!server_info || server_info->getDefaultChannelGroup() == arg)
                    return;
            }
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_SERVERGROUP)
        {
            if (args.count() < 4)
            {
                TSLogging::Error("Not enough arguments.");
                return;
            }

            auto *server_info = m_servers_info.get_server_info(targetServer);
            if (!server_info)
                return;

            if ((arg = server_info->GetServerGroupId(args.at(3))) == (uint64) NULL)
            {
                TSLogging::Error("Could not find server group.");
                return;
            }
        }

        auto groupWhisperTargetMode = GetGroupWhisperTargetMode(groupWhisperTargetModeArg);
        if (groupWhisperTargetMode >= GROUPWHISPERTARGETMODE_ENDMARKER)
        {
            TSLogging::Error("Could not recognize group whisper target mode.", scHandlerID,
                             ts_errc::parameter_invalid);
            return;
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)
        {
            TSLogging::Error("The group whisper target mode ANCESTORCHANNELFAMILY is not supported.",
                             scHandlerID, ts_errc::not_implemented);
            return;
        }

        if (const auto error = TSHelpers::SetWhisperList(targetServer, groupWhisperType,
                                                         groupWhisperTargetMode, m_return_code, arg);
            error)
        {
            if (ts_errc::ok_no_update != error)
                TSLogging::Error("Could not set whisperlist", scHandlerID, error);

            return;
        }
        m_returnCodeScHandler = targetServer;

        if (ConnectStatus::STATUS_DISCONNECTED != connection_status)
            ptt->SetPushToTalk(
            scHandlerID,
            false);  // always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallClearWhisper = true;
        if (scHandlerID != targetServer)
        {
            m_shallActivatePtt = true;
            m_returnToSCHandler = scHandlerID;
            TSHelpers::SetActiveServer(targetServer);
        }
    }
    else if (cmd == QLatin1String("TS3_NEXT_TAB_AND_WHISPER_START"))
    {
        auto nextServer = uint64{0};
        if (const auto error = TSHelpers::GetActiveServerRelative(scHandlerID, true, &nextServer); error)
            TSLogging::Error("Could not get next server.", scHandlerID, error);

        if (scHandlerID == nextServer)
            return;

        if (args.count() < 2)
        {
            TSLogging::Error("Too few arguments.", scHandlerID, ts_errc::parameter_invalid_count);
            return;
        }

        QString arg_qs;
        arg_qs = args.at(0);
        auto groupWhisperType = GetGroupWhisperType(arg_qs);
        if (groupWhisperType == GROUPWHISPERTYPE_ENDMARKER)
        {
            TSLogging::Error("Could not determine group whisper type.", scHandlerID,
                             ts_errc::parameter_invalid);
            return;
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELGROUP)
        {
            // Get My Channel Group
            auto myChannelGroup = uint64{0};
            if (const auto error = TSHelpers::GetClientChannelGroup(nextServer, &myChannelGroup); error)
                return;

            auto *server_info = m_servers_info.get_server_info(nextServer);
            if (!server_info || server_info->getDefaultChannelGroup() == myChannelGroup)
                return;
        }

        arg_qs = args.at(1);
        auto groupWhisperTargetMode = GetGroupWhisperTargetMode(arg_qs);
        if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ENDMARKER)
        {
            TSLogging::Error("Could not recognize group whisper target mode.", scHandlerID);
            return;
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)
        {
            TSLogging::Error("The group whisper target mode ANCESTORCHANNELFAMILY is not supported.",
                             scHandlerID, ts_errc::not_implemented);
            return;
        }

        if (const auto error =
            TSHelpers::SetWhisperList(nextServer, groupWhisperType, groupWhisperTargetMode);
            error)
        {
            if (ts_errc::ok_no_update != error)
                TSLogging::Error("Could not set whisperlist", scHandlerID, error);

            return;
        }

        if (ConnectStatus::STATUS_DISCONNECTED != connection_status)
            ptt->SetPushToTalk(
            scHandlerID,
            false);  // always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt = true;
        m_returnToSCHandler = scHandlerID;
        m_shallClearWhisper = true;
        TSHelpers::SetActiveServer(nextServer);
    }
}

//! Turn Ptt off delayed by the user client setting
/*!
 * \brief SnT::PttDelayFinished Timer slot
 */
void SnT::PttDelayFinished()
{
    // Turn off PTT
    TSPtt::instance()->SetPushToTalk(TSHelpers::GetActiveServerConnectionHandlerID(), false);
}

auto SnT::GetGroupWhisperTargetMode(const QString &val) -> GroupWhisperTargetMode
{
    GroupWhisperTargetMode groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ENDMARKER;

    if (val.contains(QLatin1String("ALLPARENT")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ALLPARENTCHANNELS;
    else if (val.contains(QLatin1String("PARENT")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_PARENTCHANNEL;
    else if (val.contains(QLatin1String("CURRENT")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_CURRENTCHANNEL;
    else if (val.contains(QLatin1String("SUB")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_SUBCHANNELS;
    else if (val.contains(QLatin1String("ANCESTORCHANNELFAMILY")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY;
    else if (val.contains(QLatin1String("FAMILY")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_CHANNELFAMILY;
    else if (val.contains(QLatin1String("ALL")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ALL;

    return groupWhisperTargetMode;
}

auto SnT::GetGroupWhisperType(const QString &val) -> GroupWhisperType
{
    GroupWhisperType groupWhisperType = GROUPWHISPERTYPE_ENDMARKER;
    if (val.contains(QLatin1String("COMMANDER"), Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_CHANNELCOMMANDER;
    else if (val.contains(QLatin1String("CHANNEL_GROUP"), Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_CHANNELGROUP;
    else if (val.contains(QLatin1String("SERVER_GROUP"), Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_SERVERGROUP;
    else if (val.contains(QLatin1String("ALLC"), Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_ALLCLIENTS;

    return groupWhisperType;
}
