#include "mod_radio.h"

#include "ts_helpers_qt.h"

Radio::Radio(QObject *parent)
{
    this->setParent(parent);
    this->setObjectName("Radio");
    m_isPrintEnabled = false;
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

void Radio::setEnabledHomeTab(bool val)
{
    if (m_enabled_HomeTab == val)
        return;
    m_enabled_HomeTab = val;
    emit EnabledHomeTabSet(m_enabled_HomeTab);
}

void Radio::setEnabledWhisper(bool val)
{
    if (m_enabled_Whisper == val)
        return;
    m_enabled_Whisper = val;
    emit EnabledWhisperSet(m_enabled_Whisper);
}

void Radio::setEnabledOther(bool val)
{
    if (m_enabled_Other == val)
        return;
    m_enabled_Other = val;
    emit EnabledOtherSet(m_enabled_Other);
}

void Radio::setFudgeHomeTab(double val)
{
    if (m_fudge_HomeTab == val)
        return;
    m_fudge_HomeTab = val;
    Print(QString("Fudge Home: %1").arg(m_fudge_HomeTab));
    emit FudgeHomeTabSet(m_fudge_HomeTab);
}

void Radio::setFudgeWhisper(double val)
{
    if (m_fudge_Whisper == val)
        return;
    m_fudge_Whisper = val;
    emit FudgeWhisperSet(m_fudge_Whisper);
}

void Radio::setFudgeOther(double val)
{
    if (m_fudge_Other == val)
        return;
    m_fudge_Other = val;
    emit FudgeOtherSet(m_fudge_Other);
}

/*void Radio::setCenterFrequencyHomeTab(double val)
{
    if (m_centerFrequency_HomeTab == val)
        return;
    m_centerFrequency_HomeTab = val;
    Print(QString("Center Frequency Home: %1").arg(m_centerFrequency_HomeTab));
    emit CenterFrequencyHomeTabSet(m_centerFrequency_HomeTab);
}

void Radio::setCenterFrequencyWhisper(double val)
{
    if (m_centerFrequency_Whisper == val)
        return;
    m_centerFrequency_Whisper = val;
    emit CenterFrequencyWhisperSet(m_centerFrequency_Whisper);
}

void Radio::setCenterFrequencyOther(double val)
{
    if (m_centerFrequency_Other == val)
        return;
    m_centerFrequency_Other = val;
    emit CenterFrequencyOtherSet(m_centerFrequency_Other);
}

void Radio::setBandWidthHomeTab(double val)
{
    if (m_bandWidth_HomeTab == val)
        return;
    m_bandWidth_HomeTab = val;
    Print(QString("BandWidth Home: %1").arg(m_bandWidth_HomeTab));
    emit BandWidthHomeTabSet(m_bandWidth_HomeTab);
}

void Radio::setBandWidthWhisper(double val)
{
    if (m_bandWidth_Whisper == val)
        return;
    m_bandWidth_Whisper = val;
    emit BandWidthWhisperSet(m_bandWidth_Whisper);
}

void Radio::setBandWidthOther(double val)
{
    if (m_bandWidth_Other == val)
        return;
    m_bandWidth_Other = val;
    emit BandWidthOtherSet(m_bandWidth_Other);
}*/

void Radio::setLowFrequencyHomeTab(double val)
{
    if (m_lowFrequency_HomeTab == val)
        return;
    m_lowFrequency_HomeTab = val;

    emit CenterFrequencyHomeTabSet(getCenterFrequencyHomeTab());
    emit BandWidthHomeTabSet(getBandWidthHomeTab());

    Print(QString("Low Frequency Home: %1").arg(m_lowFrequency_HomeTab));
    emit LowFrequencyHomeTabSet(m_lowFrequency_HomeTab);
}

void Radio::setLowFrequencyWhisper(double val)
{
    if (m_lowFrequency_Whisper == val)
        return;
    m_lowFrequency_Whisper = val;

    emit CenterFrequencyWhisperSet(getCenterFrequencyWhisper());
    emit BandWidthWhisperSet(getBandWidthWhisper());
    emit LowFrequencyWhisperSet(m_lowFrequency_Whisper);
}

void Radio::setLowFrequencyOther(double val)
{
    if (m_lowFrequency_Other == val)
        return;
    m_lowFrequency_Other = val;

    emit CenterFrequencyOtherSet(getCenterFrequencyOther());
    emit BandWidthOtherSet(getBandWidthOther());
    emit LowFrequencyOtherSet(m_lowFrequency_Other);
}

void Radio::setHighFrequencyHomeTab(double val)
{
    if (m_highFrequency_HomeTab == val)
        return;
    m_highFrequency_HomeTab = val;

    emit CenterFrequencyHomeTabSet(getCenterFrequencyHomeTab());
    emit BandWidthHomeTabSet(getBandWidthHomeTab());

    Print(QString("High Frequency Home: %1").arg(m_highFrequency_HomeTab));
    emit HighFrequencyHomeTabSet(m_highFrequency_HomeTab);
}

void Radio::setHighFrequencyWhisper(double val)
{
    if (m_highFrequency_Whisper == val)
        return;
    m_highFrequency_Whisper = val;

    emit CenterFrequencyWhisperSet(getCenterFrequencyWhisper());
    emit BandWidthWhisperSet(getBandWidthWhisper());
    emit HighFrequencyWhisperSet(m_highFrequency_Whisper);
}

void Radio::setHighFrequencyOther(double val)
{
    if (m_highFrequency_Other == val)
        return;
    m_highFrequency_Other = val;

    emit CenterFrequencyOtherSet(getCenterFrequencyOther());
    emit BandWidthOtherSet(getBandWidthOther());
    emit HighFrequencyOtherSet(m_highFrequency_Other);
}

void Radio::saveSettings(int r)
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
}

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

        if (isReceivedWhisper)
        {
            dspObj->setEnabled(m_enabled_Whisper);
            dspObj->setFudge(m_fudge_Whisper);
            dspObj->setBandpassEqCenterFrequency(getCenterFrequencyWhisper());
            dspObj->setBandpassEqBandWidth(getBandWidthWhisper());
            connect(this,SIGNAL(EnabledWhisperSet(bool)),dspObj,SLOT(setEnabled(bool)),Qt::UniqueConnection);
            connect(this,SIGNAL(FudgeWhisperSet(double)),dspObj,SLOT(setFudge(double)),Qt::UniqueConnection);
            connect(this,SIGNAL(CenterFrequencyWhisperSet(double)),dspObj,SLOT(setBandpassEqCenterFrequency(double)),Qt::UniqueConnection);
            connect(this,SIGNAL(BandWidthWhisperSet(double)),dspObj,SLOT(setBandpassEqBandWidth(double)),Qt::UniqueConnection);
//            Print(QString("Received Whisper: Enabled: %1 Fudge: %2 CenterFrequency: %3 Bandwidth: %4").arg(m_enabled_Whisper).arg(m_fudge_Whisper).arg(getCenterFrequencyWhisper()).arg(getBandWidthWhisper()));
//            test = true;
        }
        else if (serverConnectionHandlerID == m_homeId)
        {
            dspObj->setEnabled(m_enabled_HomeTab);
            dspObj->setFudge(m_fudge_HomeTab);
            dspObj->setBandpassEqCenterFrequency(getCenterFrequencyHomeTab());
            dspObj->setBandpassEqBandWidth(getBandWidthHomeTab());
            connect(this,SIGNAL(EnabledHomeTabSet(bool)),dspObj,SLOT(setEnabled(bool)),Qt::UniqueConnection);
            connect(this,SIGNAL(FudgeHomeTabSet(double)),dspObj,SLOT(setFudge(double)),Qt::UniqueConnection);
            connect(this,SIGNAL(CenterFrequencyHomeTabSet(double)),dspObj,SLOT(setBandpassEqCenterFrequency(double)),Qt::UniqueConnection);
            connect(this,SIGNAL(BandWidthHomeTabSet(double)),dspObj,SLOT(setBandpassEqBandWidth(double)),Qt::UniqueConnection);

//            Print(QString("Received Talk: Enabled: %1 Fudge: %2 CenterFrequency: %3 Bandwidth: %4").arg(m_enabled_HomeTab).arg(m_fudge_HomeTab).arg(getCenterFrequencyHomeTab()).arg(getBandWidthHomeTab()));
//            test = true;
        }
        else
        {
            dspObj->setEnabled(m_enabled_Other);
            dspObj->setFudge(m_fudge_Other);
            dspObj->setBandpassEqCenterFrequency(getCenterFrequencyOther());
            dspObj->setBandpassEqBandWidth(getBandWidthOther());
            connect(this,SIGNAL(EnabledOtherSet(bool)),dspObj,SLOT(setEnabled(bool)),Qt::UniqueConnection);
            connect(this,SIGNAL(FudgeOtherSet(double)),dspObj,SLOT(setFudge(double)),Qt::UniqueConnection);
            connect(this,SIGNAL(CenterFrequencyOtherSet(double)),dspObj,SLOT(setBandpassEqCenterFrequency(double)),Qt::UniqueConnection);
            connect(this,SIGNAL(BandWidthOtherSet(double)),dspObj,SLOT(setBandpassEqBandWidth(double)),Qt::UniqueConnection);
        }

//        if (TalkersDspRadios->contains(serverConnectionHandlerID))
//        {
//            Print("Have scHandlerId");
//            QMap<anyID,DspRadio*>* sDspRadios = TalkersDspRadios->value(serverConnectionHandlerID);
//            if (sDspRadios->contains(clientID))
//                Print("Have clientID");
//        }

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
        {
            //Error("(onTalkStatusChanged) Trying to remove talker with an invalid client id.",serverConnectionHandlerID,NULL);
            return false;
        }
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

//    if (test)
//        Print(QString("Processing %1 %2").arg(serverConnectionHandlerID).arg(clientID));

    QMap<anyID,DspRadio*>* sDspRadios = TalkersDspRadios->value(serverConnectionHandlerID);
    if (!(sDspRadios->contains(clientID)))
    {
//        if (test)
//        {
//            Print("Don't Have clientID");
//            test = false;
//        }
        return;
    }

    DspRadio* dspObj = sDspRadios->value(clientID);
    dspObj->Process(samples,sampleCount,channels);
//    if (test)
//    {
//        Print("Processed");
//        test = false;
//    }
}
