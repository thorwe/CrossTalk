#include "mod_agmu.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"

Agmu::Agmu(Plugin_Base& plugin)
	: m_talkers(plugin.talkers())
{
    setParent(&plugin);
    setObjectName(QStringLiteral("Agmu"));
    m_isPrintEnabled = false;
}

auto Agmu::onEditPlaybackVoiceDataEvent(
uint64 connection_id, anyID client_id, short *samples, int sampleCount, int channels) -> bool
{
    //    if (!(isForceProcessing || isRunning()))
    //        return false;

    return m_volumes.do_for(
    [samples, sampleCount, channels](DspVolumeAGMU *volume) -> bool {
        if (!volume)
            return false;

        volume->process(gsl::span<int16_t>{samples, static_cast<size_t>(sampleCount * channels)}, channels);
        return true;
    },
    connection_id, client_id);
}

auto Agmu::onTalkStatusChanged(
uint64 connection_id, int status, bool is_received_whisper, anyID client_id, bool is_me) -> bool
{
    Q_UNUSED(is_received_whisper);

    if (is_me || (!(running() || m_is_force_processing)))
        return false;

    m_is_force_processing = false;

    if (status == STATUS_TALKING)
    {   // Robust against multiple STATUS_TALKING in a row to be able to use it when the user changes settings
        auto [dsp_obj, added] = m_volumes.add_volume(connection_id, client_id);

        if (added)
        {
            //todo: move this out at a later point when cache is saved (outside of settings, with date and cleanup)
            const auto [error_client_uid, client_uid] =
            com::teamspeak::pluginsdk::funcs::get_client_property_as_string(connection_id, client_id,
                                                                            CLIENT_UNIQUE_IDENTIFIER);
            if (error_client_uid)
                Error("(onTalkStatusChanged)", connection_id, error_client_uid);
            else
            {
                if (auto it = m_peak_cache.find(client_uid); it != m_peak_cache.end())
                {
                    dsp_obj->set_peak(it->second);
                }
                dsp_obj->set_gain_current(dsp_obj->gain_desired());
            }
        }

//        dspObj->setChannelType(settings.name);
//        dspObj->setEnabled(settings.name, settings.enabled);
//        connect(this,SIGNAL(ChannelStripEnabledSet(QString,bool)), dspObj, SLOT(setEnabled(QString,bool)),Qt::UniqueConnection);

        return true;
    }
    else if (status == STATUS_NOT_TALKING)
    {
        // Removing does not need to be robust against multiple STATUS_NOT_TALKING in a row, since that doesn't happen on user setting change
        const auto [error_client_uid, client_uid] =
        com::teamspeak::pluginsdk::funcs::get_client_property_as_string(connection_id, client_id,
                                                                        CLIENT_UNIQUE_IDENTIFIER);
        if (error_client_uid)
        {
            Error("(onTalkStatusChanged)", connection_id, error_client_uid);
            m_volumes.delete_item(connection_id, client_id);
            return false;
        }

        m_volumes.do_for(
        [this, client_uid = client_uid](DspVolumeAGMU *volume) {
            if (volume)
                m_peak_cache.insert_or_assign(client_uid, volume->peak());
        },
        connection_id, client_id);
        m_volumes.delete_item(connection_id, client_id);
    }
    return false;
}

void Agmu::setNextTalkStatusChangeForceProcessing(bool val)
{
    m_is_force_processing = val;
}

// This module is currently always on, the setting switches bypassing (since it's also used for post-RadioFX makeup gain)
void Agmu::onRunningStateChanged(bool value)
{
    m_talkers.DumpTalkStatusChanges(this, ((value) ? STATUS_TALKING:STATUS_NOT_TALKING));
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}
