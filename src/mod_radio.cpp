#include "mod_radio.h"

#include "ts_helpers_qt.h"

Radio::Radio(QObject *parent)
{
    this->setParent(parent);
    this->setObjectName("Radio");
    m_isPrintEnabled = true;
    talkers = Talkers::instance();
    TalkersDspRadios = new QMap<uint64,QMap<anyID,DspRadio*>* >;
}

void Radio::setHomeId(uint64 serverConnectionHandlerID)
{
    if (m_homeId == serverConnectionHandlerID)
        return;
    m_homeId = serverConnectionHandlerID;
    if (m_homeId == 0)
        return;
    if (isRunning())
        talkers->DumpTalkStatusChanges(this,true);
}

void Radio::setChannelStripEnabled(QString name, bool val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).enabled != val)
            m_SettingsMap[name].enabled = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.enabled = val;
//        setting.freq_low = 0.0f;
//        setting.freq_hi = 0.0f;
//        setting.fudge = 0.0f;
//        setting.rm_mod_freq = 0.0f;
        m_SettingsMap.insert(name,setting);
    }
    //Print(QString("%1 enabled %2").arg(name).arg(val));
    emit ChannelStripEnabledSet(name,val);
}

void Radio::setFudge(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).fudge != val)
            m_SettingsMap[name].fudge = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
//        setting.enabled = false;
//        setting.freq_low = 0.0f;
//        setting.freq_hi = 0.0f;
        setting.fudge = val;
//        setting.rm_mod_freq = 0.0f;
        m_SettingsMap.insert(name,setting);
    }
    //Print(QString("%1 fudge %2").arg(name).arg(val));
    emit FudgeChanged(name,val);
}

void Radio::setInLoFreq(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).freq_low != val)
            m_SettingsMap[name].freq_low = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.freq_low = val;
        m_SettingsMap.insert(name,setting);
    }
    emit InBpCenterFreqSet(name,getCenterFrequencyIn(m_SettingsMap.value(name)));
    emit InBpBandwidthSet(name,getBandWidthIn(m_SettingsMap.value(name)));

    //Print(QString("%1 low_freq %2").arg(name).arg(val));
    emit InLoFreqSet(name,val);
}

void Radio::setInHiFreq(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).freq_hi != val)
            m_SettingsMap[name].freq_hi = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.freq_hi = val;
        m_SettingsMap.insert(name,setting);
    }
    emit InBpCenterFreqSet(name,getCenterFrequencyIn(m_SettingsMap.value(name)));
    emit InBpBandwidthSet(name,getBandWidthIn(m_SettingsMap.value(name)));

    //Print(QString("%1 hi_freq %2").arg(name).arg(val));
    emit InHiFreqSet(name,val);
}

void Radio::setRingModFrequency(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).rm_mod_freq != val)
            m_SettingsMap[name].rm_mod_freq = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.rm_mod_freq = val;
        m_SettingsMap.insert(name,setting);
    }
    //Print(QString("%1 rm_mod_freq %2").arg(name).arg(val));
    emit RingModFrequencyChanged(name,val);
}

void Radio::setRingModMix(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).rm_mix != val)
            m_SettingsMap[name].rm_mix = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.rm_mix = val;
        m_SettingsMap.insert(name,setting);
    }
    //Print(QString("%1 rm_mix %2").arg(name).arg(val));
    emit RingModMixChanged(name,val);
}

void Radio::setOutLoFreq(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).o_freq_lo != val)
            m_SettingsMap[name].o_freq_lo = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.o_freq_lo = val;
        m_SettingsMap.insert(name,setting);
    }
    emit OutBpCenterFreqSet(name,getCenterFrequencyOut(m_SettingsMap.value(name)));
    emit OutBpBandwidthSet(name,getBandWidthOut(m_SettingsMap.value(name)));

    emit OutLoFreqSet(name,val);
}

void Radio::setOutHiFreq(QString name, double val)
{
    if (m_SettingsMap.contains(name))
    {
        if (m_SettingsMap.value(name).o_freq_hi != val)
            m_SettingsMap[name].o_freq_hi = val;
    }
    else
    {
        RadioFX_Settings setting = RadioFX_Settings();
        setting.name = name;
        setting.o_freq_hi = val;
        m_SettingsMap.insert(name,setting);
    }
    emit OutBpCenterFreqSet(name,getCenterFrequencyOut(m_SettingsMap.value(name)));
    emit OutBpBandwidthSet(name,getBandWidthOut(m_SettingsMap.value(name)));

    //Print(QString("%1 rm_mix %2").arg(name).arg(val));
    emit OutHiFreqSet(name,val);
}

void Radio::ToggleClientBlacklisted(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (m_ClientBlacklist.contains(serverConnectionHandlerID,clientID))
        m_ClientBlacklist.remove(serverConnectionHandlerID,clientID);
    else
        m_ClientBlacklist.insert(serverConnectionHandlerID,clientID);

    if (!(isRunning()))
        return;

    if (!(TalkersDspRadios->contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,DspRadio*>* sDspRadios = TalkersDspRadios->value(serverConnectionHandlerID);
    if (sDspRadios->contains(clientID))
    {
        DspRadio* dspObj = sDspRadios->value(clientID);
        dspObj->setEnabled(QString::null,!isClientBlacklisted(serverConnectionHandlerID,clientID));
    }
}

bool Radio::isClientBlacklisted(uint64 serverConnectionHandlerID, anyID clientID)
{
    return m_ClientBlacklist.contains(serverConnectionHandlerID,clientID);
}

/*void Radio::saveSettings(int r)
{
    Q_UNUSED(r);
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup(this->objectName());

    cfg.beginGroup("HomeTab");
    cfg.setValue("enabled",m_enabled_HomeTab);
    cfg.setValue("low_freq",m_lowFrequency_HomeTab);
    cfg.setValue("high_freq",m_highFrequency_HomeTab);
    cfg.setValue("fudge",m_fudge_HomeTab);
    cfg.endGroup();

    cfg.beginGroup("Whisper");
    cfg.setValue("enabled",m_enabled_Whisper);
    cfg.setValue("low_freq",m_lowFrequency_Whisper);
    cfg.setValue("high_freq",m_highFrequency_Whisper);
    cfg.setValue("fudge",m_fudge_Whisper);
    cfg.endGroup();

    cfg.beginGroup("Other");
    cfg.setValue("enabled",m_enabled_Other);
    cfg.setValue("low_freq",m_lowFrequency_Other);
    cfg.setValue("high_freq",m_highFrequency_Other);
    cfg.setValue("fudge",m_fudge_Other);
    cfg.endGroup();

    cfg.endGroup();
}*/

void Radio::onRunningStateChanged(bool value)
{
    talkers->DumpTalkStatusChanges(this,((value)?STATUS_TALKING:STATUS_NOT_TALKING));//FlushTalkStatusChanges((value)?STATUS_TALKING:STATUS_NOT_TALKING);
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

//bool test = false;
bool Radio::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe)
{
    if (isMe || !isRunning())
        return false;

    if (status == STATUS_TALKING)
    {   // Robust against multiple STATUS_TALKING in a row to be able to use it when the user changes settings
        DspRadio* dspObj;
        bool isNewDspObj = true;
        if (!(TalkersDspRadios->contains(serverConnectionHandlerID)))
        {
            dspObj = new DspRadio(this);
            QMap<anyID,DspRadio*>* sDspRadios = new QMap<anyID,DspRadio*>;
            sDspRadios->insert(clientID,dspObj);
            TalkersDspRadios->insert(serverConnectionHandlerID,sDspRadios);
        }
        else
        {
            QMap<anyID,DspRadio*>* sDspRadios = TalkersDspRadios->value(serverConnectionHandlerID);
            if (sDspRadios->contains(clientID))
            {
                dspObj = sDspRadios->value(clientID);
                isNewDspObj = false;
            }
            else
            {
                dspObj = new DspRadio(this);
                sDspRadios->insert(clientID,dspObj);
            }
        }

        if (!isNewDspObj)
            this->disconnect(dspObj);

        RadioFX_Settings settings;
        if (isReceivedWhisper)
            settings = m_SettingsMap.value("Whisper");
        else if (serverConnectionHandlerID == m_homeId)
            settings = m_SettingsMap.value("Home");
        else
            settings = m_SettingsMap.value("Other");

        dspObj->setChannelType(settings.name);
        dspObj->setEnabled(settings.name, settings.enabled);
        dspObj->setBandpassEqInCenterFrequency(settings.name, getCenterFrequencyIn(settings));
        dspObj->setBandpassEqInBandWidth(settings.name, getBandWidthIn(settings));
        dspObj->setFudge(settings.name, settings.fudge);
        dspObj->setRmModFreq(settings.name, settings.rm_mod_freq);
        dspObj->setRmMix(settings.name, settings.rm_mix);
        dspObj->setBandpassEqOutCenterFrequency(settings.name, getCenterFrequencyOut(settings));
        dspObj->setBandpassEqOutBandWidth(settings.name, getBandWidthOut(settings));
        connect(this,SIGNAL(ChannelStripEnabledSet(QString,bool)), dspObj, SLOT(setEnabled(QString,bool)),Qt::UniqueConnection);
        connect(this,SIGNAL(FudgeChanged(QString,double)), dspObj, SLOT(setFudge(QString,double)),Qt::UniqueConnection);
        connect(this,SIGNAL(InBpCenterFreqSet(QString,double)), dspObj, SLOT(setBandpassEqInCenterFrequency(QString,double)),Qt::UniqueConnection);
        connect(this,SIGNAL(InBpBandwidthSet(QString,double)), dspObj, SLOT(setBandpassEqInBandWidth(QString,double)),Qt::UniqueConnection);
        connect(this,SIGNAL(RingModFrequencyChanged(QString,double)), dspObj, SLOT(setRmModFreq(QString,double)),Qt::UniqueConnection);
        connect(this,SIGNAL(RingModMixChanged(QString,double)), dspObj, SLOT(setRmMix(QString,double)));
        connect(this,SIGNAL(OutBpCenterFreqSet(QString,double)), dspObj, SLOT(setBandpassEqOutCenterFrequency(QString,double)),Qt::UniqueConnection);
        connect(this,SIGNAL(OutBpBandwidthSet(QString,double)), dspObj, SLOT(setBandpassEqOutBandWidth(QString,double)),Qt::UniqueConnection);

        return true;
    }
    else if (status == STATUS_NOT_TALKING)
    {
        // Removing does not need to be robust against multiple STATUS_NOT_TALKING in a row, since that doesn't happen on user setting change
        if (!TalkersDspRadios->contains(serverConnectionHandlerID))
        {
            // return silent bec. of ChannelMuter implementation
//            Error("(onTalkStatusChanged) Trying to remove talker from an invalid server connection handler id.",serverConnectionHandlerID,NULL);
            return false;
        }

        QMap<anyID,DspRadio*>* sDspRadios = TalkersDspRadios->value(serverConnectionHandlerID);
        if (!(sDspRadios->contains(clientID)))
            return false;

        DspRadio* dspObj = sDspRadios->value(clientID);
//        dspObj->setPanAdjustment(false);
        dspObj->blockSignals(true);
        dspObj->deleteLater();
        sDspRadios->remove(clientID);
//        Print(QString("Removed %1 %2").arg(serverConnectionHandlerID).arg(clientID));
    }
    return false;
}

//! Routes the arguments of the event to the corresponding volume object
/*!
 * \brief Radio::onEditPlaybackVoiceDataEvent pre-processing voice event
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client-side runtime-id of the sender
 * \param samples the sample array to manipulate
 * \param sampleCount amount of samples in the array
 * \param channels amount of channels
 */
void Radio::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
    if (!(isRunning()))
        return;

    if (!(TalkersDspRadios->contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,DspRadio*>* sDspRadios = TalkersDspRadios->value(serverConnectionHandlerID);
    if (!(sDspRadios->contains(clientID)))
        return;

    DspRadio* dspObj = sDspRadios->value(clientID);
    dspObj->Process(samples,sampleCount,channels);
}

QMap<QString, RadioFX_Settings> Radio::GetSettingsMap() const
{
    return m_SettingsMap;
}
