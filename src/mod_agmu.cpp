#include "mod_agmu.h"
#include "public_errors.h"
#include "ts_helpers_qt.h"
#include "ts3_functions.h"  //currently only used in a debug output
#include "plugin.h"  //currently only used in a debug output

Agmu::Agmu(QObject *parent) :
    m_isForceProcessing(false)
{
    this->setParent(parent);
    this->setObjectName("Agmu");
    m_isPrintEnabled = true;
    talkers = Talkers::instance();
    m_TalkersDSPs = new QMap<uint64,QMap<anyID,DspVolumeAGMU*>* >;
    m_PeakCache = new QHash<QString,short>;
}

bool Agmu::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
//    if (!(isForceProcessing || isRunning()))
//        return false;

    if (!(m_TalkersDSPs->contains(serverConnectionHandlerID)))
        return false;

    QMap<anyID,DspVolumeAGMU*>* sDspVolumeAGMUs = m_TalkersDSPs->value(serverConnectionHandlerID);
    if (!(sDspVolumeAGMUs->contains(clientID)))
        return false;

    DspVolumeAGMU* dspObj = sDspVolumeAGMUs->value(clientID);
    dspObj->process(samples,sampleCount,channels);
    return true;
}

bool Agmu::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe)
{
    Q_UNUSED(isReceivedWhisper);

    if ( isMe || (!(isRunning() || m_isForceProcessing)) )
        return false;

    m_isForceProcessing = false;

    if (status == STATUS_TALKING)
    {   // Robust against multiple STATUS_TALKING in a row to be able to use it when the user changes settings
        DspVolumeAGMU* dspObj;
        bool isNewDspObj = true;
        if (!(m_TalkersDSPs->contains(serverConnectionHandlerID)))
        {
            dspObj = new DspVolumeAGMU(this);
            QMap<anyID,DspVolumeAGMU*>* sDspVolumeAGMUs = new QMap<anyID,DspVolumeAGMU*>;
            sDspVolumeAGMUs->insert(clientID,dspObj);
            m_TalkersDSPs->insert(serverConnectionHandlerID,sDspVolumeAGMUs);
        }
        else
        {
            QMap<anyID,DspVolumeAGMU*>* sDspVolumeAGMUs = m_TalkersDSPs->value(serverConnectionHandlerID);
            if (sDspVolumeAGMUs->contains(clientID))
            {
                dspObj = sDspVolumeAGMUs->value(clientID);
                isNewDspObj = false;
            }
            else
            {
                dspObj = new DspVolumeAGMU(this);
                sDspVolumeAGMUs->insert(clientID,dspObj);
            }
        }

        if (!isNewDspObj)
            this->disconnect(dspObj);
        else
        {
            //todo: move this out at a later point when cache is saved (outside of settings, with date and cleanup)
            QString clientUID;
            unsigned int error;
            if ((error = TSHelpers::GetClientUID(serverConnectionHandlerID, clientID, clientUID)) != ERROR_ok)
                Error("(onTalkStatusChanged)",serverConnectionHandlerID,error);
            else
            {
                dspObj->setPeak(m_PeakCache->value(clientUID));
                dspObj->setGainCurrent(dspObj->computeGainDesired());
                char name[512];
                if((error = ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512)) != ERROR_ok)
                {
                    Error("(onTalkStatusChanged) Error getting client display name",serverConnectionHandlerID,error);
                    return true;
                }
                Print(QString("Setting dspObj from cache: %1 %2 %3").arg(name).arg(dspObj->GetPeak()).arg(dspObj->getGainCurrent()));
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
        if (!m_TalkersDSPs->contains(serverConnectionHandlerID))
            return false;   // return silent bec. of ChannelMuter implementation

        QMap<anyID,DspVolumeAGMU*>* sDspVolumeAGMUs = m_TalkersDSPs->value(serverConnectionHandlerID);
        if (!(sDspVolumeAGMUs->contains(clientID)))
            return false;

        DspVolumeAGMU* dspObj = sDspVolumeAGMUs->value(clientID);
        QString clientUID;
        unsigned int error;
        if ((error = TSHelpers::GetClientUID(serverConnectionHandlerID,clientID, clientUID)) != ERROR_ok)
        {
            Error("(onTalkStatusChanged)",serverConnectionHandlerID,error);
            return false;
        }
        m_PeakCache->insert(clientUID,dspObj->GetPeak());
        dspObj->blockSignals(true);
        dspObj->deleteLater();
        sDspVolumeAGMUs->remove(clientID);
    }
    return false;
}

void Agmu::setNextTalkStatusChangeForceProcessing(bool val)
{
    m_isForceProcessing = val;
}

// This module is currently always on, the setting switches bypassing (since it's also used for post-RadioFX makeup gain)
void Agmu::onRunningStateChanged(bool value)
{
    talkers->DumpTalkStatusChanges(this,((value)?STATUS_TALKING:STATUS_NOT_TALKING));
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}
