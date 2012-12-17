#include "pantalkers.h"

#include "ts3_functions.h"
#include "plugin.h"

const QList<unsigned int> FlagList = QList<unsigned int>() << 1 << 2 << 4 << 8 << 16 << 32 << 64 << 128;
const QList<float> SPREAD = QList<float>() << 0 << -0.5f << 0.5f << -1.0f << 1.0f; //<< -0.25f << 0.25f << -0.75f << 0.75f;

const QList<float> SPREAD_CENTER = QList<float>() << 0 << -0.2f << 0.2f << -0.4f << 0.4f << -0.1f << 0.1f << -0.3f << 0.3f;
const QList<float> SPREAD_RIGHT = QList<float>() << 0.75f << 1.0f << 0.5f << 0.875f << 0.625f << 0.9375f << 0.8125f << 0.5625f << 0.6875f;
const QList<float> SPREAD_LEFT = QList<float>() << -0.75f << -1.0f << -0.5f << -0.875f << -0.625f << -0.9375f << -0.8125f << -0.5625f << -0.6875f;


PanTalkers::PanTalkers(QObject *parent) :
    QObject(parent),
    m_homeId(0)
{
    ts = TSFunctions::instance();
    talkers = Talkers::instance();
    TalkersPanners = new QMap<QString,SimplePanner*>;
    TalkerSequence = new QList<QString>;
    TalkerSequences = new QMap<TALKERS_REGION,QList<QString>* >;
    for (int i = 0; i<TALKERS_REGION_END;++i)
    {
        QList<QString>* list = new QList<QString>;
        TalkerSequences->insert((TALKERS_REGION)i,list);
    }
}

//Properties
bool PanTalkers::isEnabled() const
{
    return m_enabled;
}

float PanTalkers::getSpreadWidth() const
{
    return m_spreadWidth;
}

void PanTalkers::setHomeId(uint64 serverConnectionHandlerID)
{
    if (serverConnectionHandlerID == m_homeId)
        return;
    m_homeId = serverConnectionHandlerID;
}

void PanTalkers::setEnabled(bool value)
{
    m_enabled = value;
    connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
//    if (m_enabled)
//    {
//        connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
//    }
//    else
//    {
//        disconnect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
//        // TODO: iterate and talkstatus stop
//    }
    ts->Log(QString("Stereo Position Spread: enabled: %1").arg((m_enabled)?"true":"false"));
    emit enabledSet(m_enabled);
}

void PanTalkers::setSpreadWidth(float value)
{
    m_spreadWidth = value;
    ts->Log(QString("Stereo Position Spread: setSpreadWidth: %1").arg(m_spreadWidth));
    emit spreadWidthSet(value);
}

void PanTalkers::setExpertModeEnabled(bool value)
{
    m_ExpertModeEnabled = value;
    ts->Log(QString("Stereo Position Spread: setExpertModeEnabled: %1").arg((m_ExpertModeEnabled)?"true":"false"));
    emit expertModeEnabledSet(value);
}

void PanTalkers::setRegionHomeTab(int talkersRegion)
{
    m_RegionHomeTab = (TALKERS_REGION)talkersRegion;
    ts->Log(QString("Stereo Position Spread: setRegionHomeTab: %1").arg(talkersRegion));
    emit regionHomeTabSet(m_RegionHomeTab);
}

void PanTalkers::setRegionWhisper(int talkersRegion)
{
    m_RegionWhisper = (TALKERS_REGION)talkersRegion;
    ts->Log(QString("Stereo Position Spread: setRegionWhisper: %1").arg(talkersRegion));
    emit regionWhisperSet(m_RegionWhisper);
}

void PanTalkers::setRegionOther(int talkersRegion)
{
    m_RegionOther = (TALKERS_REGION)talkersRegion;
    ts->Log(QString("Stereo Position Spread: setRegionOther: %1").arg(talkersRegion));
    emit regionOtherSet(m_RegionOther);
}

void PanTalkers::onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels, const unsigned int *channelSpeakerArray, unsigned int *channelFillMask)
{
    if (!(m_enabled))
        return;

    // channels seems to be basically determined by soundcard setting
    if (channels == 1)
        return;

    char* res;
    if(ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &res) != ERROR_ok) {
        return;
    }
    QString UID = QString::fromUtf8(res);
    ts3Functions.freeMemory(res);

    if (!(TalkersPanners->contains(UID)))
        return;

    QMap<unsigned int,int> speaker2Channel;
    for(int i=0; i<channels; ++i)
    {
        if (*channelFillMask & FlagList.at(i))
        {
            speaker2Channel.insert(channelSpeakerArray[i],i);
        }
    }

    if (speaker2Channel.size() > 2)    // Todo: For now.
        return;

    // Get SimplePanner
    SimplePanner* panner = TalkersPanners->value(UID);


    // Try to get headphones
    if (speaker2Channel.contains(SPEAKER_HEADPHONES_LEFT))
    {
        panner->process(samples,sampleCount,channels,speaker2Channel.value(SPEAKER_HEADPHONES_LEFT),speaker2Channel.value(SPEAKER_HEADPHONES_RIGHT));
    }
    else if (speaker2Channel.contains(SPEAKER_FRONT_LEFT))
    {
        panner->process(samples,sampleCount,channels,speaker2Channel.value(SPEAKER_FRONT_LEFT),speaker2Channel.value(SPEAKER_FRONT_RIGHT));
    }
    else if (speaker2Channel.contains(SPEAKER_FRONT_CENTER))
    {

        if (!((panner->getPanDesired() == panner->getPanCurrent()) == 0.0f)) // if middle is where it is and is to be, leave it at center, otherwise...
        {
            // Search for the non-used front left & right
            // Populate Front Left and Right & Fill Center with 0s
            int leftChannelNr = -1;
            int rightChannelNr = -1;
            for(int i=0; i<channels; ++i)
            {
                if (channelSpeakerArray[i] == SPEAKER_FRONT_LEFT)
                {
                    leftChannelNr = i;
                    if (rightChannelNr != -1)
                        break;
                }
                else if (channelSpeakerArray[i] == SPEAKER_FRONT_RIGHT)
                {
                    rightChannelNr = i;
                    if (leftChannelNr != -1)
                        break;
                }
            }
            if ((leftChannelNr == -1) || (rightChannelNr == -1))
            {
                ts->Log("Could not find Front Speakers.", LogLevel_ERROR);
                return;
            }

            for(int i=0; i<sampleCount; ++i){
                short monosample = samples[speaker2Channel.value(SPEAKER_FRONT_CENTER) +(i*channels)];
                samples[leftChannelNr + (i*channels)] = monosample; // ToDo: + or - 3dB would be appropriate?
                samples[rightChannelNr + (i*channels)] = monosample;
                samples[speaker2Channel.value(SPEAKER_FRONT_CENTER) +(i*channels)] = 0.0f;
            }
            panner->process(samples,sampleCount,channels,leftChannelNr, rightChannelNr);
        }
    }

//    if (!(isReported))
//    {
//        printf("Channels: %i, channelFillMask: %u \n", channels,*channelFillMask);

//        printf("Active Channels: %i\n",speaker2Channel.size());

//        isReported = true;
//    }
}

void PanTalkers::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID)
{
    char* res;
    if(ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &res) != ERROR_ok) {
        return;
    }
    QString UID = QString::fromUtf8(res);
    ts3Functions.freeMemory(res);

    QList<QString>* seq;
    if (status == STATUS_TALKING)
    {

        SimplePanner* Panner = new SimplePanner;
        Panner->setPanAdjustment(true);
        TalkersPanners->insert(UID,Panner);

//        TALKERS_TYPE talkerType;
        TALKERS_REGION region;
        const QList<float>* spread;
        if (!(m_ExpertModeEnabled))
        {
            region = TALKERS_REGION_CENTER;
            spread = &SPREAD;
        }
        else
        {
            if (isReceivedWhisper)
            {
    //            talkerType = TALKERS_TYPE_WHISPER;
                region = m_RegionWhisper;
            }
            else if (serverConnectionHandlerID == m_homeId)
            {
    //            talkerType = TALKERS_TYPE_HOMETAB;
                region = m_RegionHomeTab;
            }
            else
            {
    //            talkerType = TALKERS_TYPE_OTHER;
                region = m_RegionOther;
            }

            if (region == TALKERS_REGION_CENTER)
                spread = &SPREAD_CENTER;
            else if (region == TALKERS_REGION_LEFT)
                spread = &SPREAD_LEFT;
            else if (region == TALKERS_REGION_RIGHT)
                spread = &SPREAD_RIGHT;
        }
        seq = TalkerSequences->value(region);

        float val = 0.0f;
        int position = seq->indexOf("",0);
        if (position != -1)
        {
            seq->replace(position,UID);
        }
        else
        {
            seq->append(UID);
            position = seq->size() - 1;
        }

        if (spread->size() > position)
            val = spread->at(position) * m_spreadWidth;
        else
            val = spread->at(0) * m_spreadWidth;

        Panner->setPanDesiredByPanAdjuster(val);
        if (Panner->getPanDesiredByManual() == 0.0f)
            Panner->setPanCurrent(val); //don't ramp, pre-set pan

        Print(QString("Added Panner for %1 on position %2 with value %3").arg(UID).arg(position).arg(val));
    }
    else if (status == STATUS_NOT_TALKING)
    {
        SimplePanner* panner = TalkersPanners->value(UID);
        panner->deleteLater();
        TalkersPanners->remove(UID);

        if (isReceivedWhisper)
            seq = TalkerSequences->value(m_RegionWhisper);
        else if (serverConnectionHandlerID == m_homeId)
            seq = TalkerSequences->value(m_RegionHomeTab);
        else
            seq = TalkerSequences->value(m_RegionOther);

        if (!(seq->contains(UID)))
        {
            for (int i = 0; i<TALKERS_REGION_END; ++i)
            {
                seq = TalkerSequences->value((TALKERS_REGION)i);
                if (seq->contains(UID))
                    break;
            }
        }
        if (!(seq->contains(UID)))
        {
            ts->Error(NULL,NULL,"SPS: Could not find talker to remove.");
            return;
        }

        int position = seq->indexOf(UID);
        seq->replace(position,"");
        bool shallClear = true;
        for (int i=0;i<seq->size();++i)
        {
            if (seq->value(i) != "")
            {
                shallClear = false;
                break;
            }
        }
        if (shallClear)
            seq->clear();
    }

    Print(QString("TalkerSequence size: %1").arg(seq->size()));
}

//backup
/*void PanTalkers::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID)
{
    char* res;
    if(ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &res) != ERROR_ok) {
        return;
    }
    QString UID = QString::fromUtf8(res);
    ts3Functions.freeMemory(res);

    if (status == STATUS_TALKING)
    {

        SimplePanner* Panner = new SimplePanner;
        Panner->setPanAdjustment(true);
        TalkersPanners->insert(UID,Panner);

        float val = 0.0f;
        int position = TalkerSequence->indexOf("",0);
        if (position != -1)
        {
            TalkerSequence->replace(position,UID);
        }
        else
        {
            TalkerSequence->append(UID);
            position = TalkerSequence->size() - 1;
        }

        if (SPREAD.size() > position)
            val = SPREAD.at(position) * m_spreadWidth;

        Panner->setPanDesiredByPanAdjuster(val);
        if (Panner->getPanDesiredByManual() == 0.0f)
            Panner->setPanCurrent(val); //don't ramp, pre-set pan

        Print(QString("Added Panner for %1 on position %2 with value %3").arg(UID).arg(position).arg(val));
    }
    else if (status == STATUS_NOT_TALKING)
    {
        SimplePanner* panner = TalkersPanners->value(UID);
        panner->deleteLater();
        TalkersPanners->remove(UID);

        int position = TalkerSequence->indexOf(UID);
        TalkerSequence->replace(position,"");
        bool shallClear = true;
        for (int i=0;i<TalkerSequence->size();++i)
        {
            if (TalkerSequence->value(i) != "")
            {
                shallClear = false;
                break;
            }
        }
        if (shallClear)
            TalkerSequence->clear();
    }

    Print(QString("TalkerSequence size: %1").arg(TalkerSequence->size()));
}*/

void PanTalkers::Print(QString inqstr)
{
    if (!isPrintEnabled) return;
    inqstr.prepend("SPS: ");
    ts->Print(inqstr);
}
