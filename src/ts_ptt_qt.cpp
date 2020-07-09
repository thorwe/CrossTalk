#include "ts_ptt_qt.h"

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"

#include "plugin.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"
#include "core/ts_logging_qt.h"
#include "core/ts_settings_qt.h"

constexpr const int32_t kPluginThreadTimeout = 1000;

using namespace com::teamspeak::pluginsdk;

TSPtt *TSPtt::m_Instance = nullptr;

TSPtt::TSPtt() = default;

TSPtt::~TSPtt()
{
    if (timer)
        timer->stop();

    delete timer;
}

void TSPtt::Init(QMutex *mutex_cmd)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TSPtt::onPttDelayFinished);
    timer->setSingleShot(true);
    command_mutex = mutex_cmd;
    this->setObjectName("TSPtt");
}

int TSPtt::SetPushToTalk(uint64 connection_id, PTT_CHANGE_STATUS action)
{
    if (action == PTT_ACTIVATE)
    {
        if (timer->isActive())
            timer->stop();

        return SetPushToTalk(connection_id, true);
    }
    else if (action == PTT_DEACTIVATE)
    {
        UpdatePttDelayInfo();
        if (pttDelayEnabled)
            timer->start(pttDelayMsec);
        else
            return SetPushToTalk(connection_id, false);
    }
    else if (action == PTT_TOGGLE)  // Toggling is always instant
    {
        if (pttActive && timer->isActive())
            timer->stop();

        return SetPushToTalk(connection_id, !pttActive);
    }
    else  // use NULL for Toggle?
    {
        TSLogging::Error("Ptt Status Error", connection_id);
    }

    return 1;
}

int TSPtt::SetPushToTalk(uint64 connection_id, bool shouldTalk)
{
    // If PTT is inactive, store the current settings
    if (!pttActive)
    {
        // Get the current VAD setting
        const auto [error_vad, vad] = funcs::sound::get_preprocessor_config_value(connection_id, "vad");
        if (error_vad)
        {
            TSLogging::Error("Error retrieving vad setting", connection_id, error_vad);
            return 1;
        }
        vadActive = vad == "true";

        // Get the current input setting, this will indicate whether VAD is being used in combination with PTT

        const auto [error_input_deactivated, input_deactivated] =
        funcs::get_client_self_property_as_int(connection_id, CLIENT_INPUT_DEACTIVATED);
        if (error_input_deactivated)
        {
            TSLogging::Error("Error retrieving input setting", connection_id, error_input_deactivated);
            return 1;
        }
        inputActive = !input_deactivated;  // We want to know when it is active, not when it is inactive
    }

    // If VAD is active and the input is active, disable VAD, restore VAD setting afterwards
    const auto error_set_vad = funcs::sound::set_preprocessor_config_value(
    connection_id, "vad",
    (shouldTalk && (vadActive && inputActive)) ? "false" : (vadActive) ? "true" : "false");
    if (error_set_vad)
    {
        TSLogging::Error("Error toggling vad", connection_id, error_set_vad);
        return 1;
    }

    // Activate the input, restore the input setting afterwards
    const auto error_activate_input = funcs::set_client_self_property_as_int(
    connection_id, CLIENT_INPUT_DEACTIVATED, (shouldTalk || inputActive) ? INPUT_ACTIVE : INPUT_DEACTIVATED);
    if (error_activate_input)
    {
        TSLogging::Error("Error toggling input", connection_id, error_activate_input);
        return 1;
    }

    // Update the client
    funcs::flush_client_self_updates(connection_id);

    // Commit the change
    pttActive = shouldTalk;

    return 0;
}

void TSPtt::UpdatePttDelayInfo()
{
    // Get default capture profile and preprocessor data
    const auto [error_profiles, default_profile, profiles] =
    funcs::get_profile_list(PLUGIN_GUI_SOUND_CAPTURE);
    if (error_profiles)
    {
        TSLogging::Error("Error retrieving capture profiles", error_profiles);
        return;
    }
    const auto &profile = profiles[default_profile];

    QString preProcessorData;
    QString profile_q = QString::fromUtf8(profile.data());
    if (!(TSSettings::instance()->GetPreProcessorData(profile_q, preProcessorData)))
    {
        TSLogging::Error("(TSPtt::UpdatePttDelayInfo()) (GetPreProcessorData) (" + profile_q + ")" +
                         TSSettings::instance()->GetLastError().text());
        return;
    }

    auto lines = preProcessorData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    QStringListIterator iterator(lines);
    while (iterator.hasNext())
    {
        QString str_qstr = iterator.next();
        if (str_qstr.startsWith("delay_ptt_msecs"))
        {
            str_qstr.remove("delay_ptt_msecs=");
            bool ok;
            int msec = str_qstr.toInt(&ok);
            if (!ok)
                TSLogging::Error("Error retrieving delay_ptt_msecs");
            else
                pttDelayMsec = msec;
        }
        else if (str_qstr.startsWith("delay_ptt"))
        {
            pttDelayEnabled = (str_qstr == "delay_ptt=true");
        }
    }
}

//! Emits PttDelayFinished when the timer runs out
/*!
 * \brief TSPtt::onPttDelayFinished Qt Slot
 */
void TSPtt::onPttDelayFinished()
{
    command_mutex->tryLock(kPluginThreadTimeout);
    emit PttDelayFinished();
    command_mutex->unlock();
}
