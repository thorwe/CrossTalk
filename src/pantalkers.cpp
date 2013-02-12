#include "pantalkers.h"

#include "ts3_functions.h"
#include "plugin.h"

const QList<unsigned int> FlagList = QList<unsigned int>() << 1 << 2 << 4 << 8 << 16 << 32 << 64 << 128;
const QList<float> SPREAD = QList<float>() << 0 << -0.5f << 0.5f << -1.0f << 1.0f; //<< -0.25f << 0.25f << -0.75f << 0.75f;

const QList<float> SPREAD_CENTER = QList<float>() << 0 << -0.2f << 0.2f << -0.4f << 0.4f << -0.1f << 0.1f << -0.3f << 0.3f;
const QList<float> SPREAD_RIGHT = QList<float>() << 0.75f << 1.0f << 0.5f << 0.875f << 0.625f << 0.9375f << 0.8125f << 0.5625f << 0.6875f;
const QList<float> SPREAD_LEFT = QList<float>() << -0.75f << -1.0f << -0.5f << -0.875f << -0.625f << -0.9375f << -0.8125f << -0.5625f << -0.6875f;

const QPair<uint64,anyID> SEAT_HOLDER = qMakePair((uint64)0,(anyID)0);

PanTalkers::PanTalkers(QObject *parent) :
    //QObject(parent),
    m_homeId(0)
{
    this->setParent(parent);
    this->setObjectName("Position Spread");
    m_isPrintEnabled = true;
    ts = TSFunctions::instance();
    talkers = Talkers::instance();
    TalkersPanners = new QMap<uint64,QMap<anyID,SimplePanner*>* >;
    TalkerSequences = new QMap<TALKERS_REGION,QList< QPair<uint64,anyID> >* >;
    for (int i = 0; i<TALKERS_REGION_END;++i)
    {
        QList< QPair<uint64,anyID> >* list = new QList< QPair<uint64,anyID> >;
        TalkerSequences->insert((TALKERS_REGION)i,list);
    }
}

//Properties

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

void PanTalkers::onRunningStateChanged(bool value)
{
    if (value)
    {
        connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
    }
    else
    {
        disconnect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
        // TODO: iterate and talkstatus stop
    }
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

void PanTalkers::setSpreadWidth(float value)
{
    m_spreadWidth = value;
    Log(QString("setSpreadWidth: %1").arg(m_spreadWidth));
    emit spreadWidthSet(value);
}

void PanTalkers::setExpertModeEnabled(bool value)
{
    m_ExpertModeEnabled = value;
    Log(QString("setExpertModeEnabled: %1").arg((m_ExpertModeEnabled)?"true":"false"));
    emit expertModeEnabledSet(value);
}

void PanTalkers::setRegionHomeTab(int talkersRegion)
{
    m_RegionHomeTab = (TALKERS_REGION)talkersRegion;
    Log(QString("setRegionHomeTab: %1").arg(talkersRegion));
    emit regionHomeTabSet(m_RegionHomeTab);
}

void PanTalkers::setRegionWhisper(int talkersRegion)
{
    m_RegionWhisper = (TALKERS_REGION)talkersRegion;
    Log(QString("setRegionWhisper: %1").arg(talkersRegion));
    emit regionWhisperSet(m_RegionWhisper);
}

void PanTalkers::setRegionOther(int talkersRegion)
{
    m_RegionOther = (TALKERS_REGION)talkersRegion;
    Log(QString("setRegionOther: %1").arg(talkersRegion));
    emit regionOtherSet(m_RegionOther);
}

void PanTalkers::onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels, const unsigned int *channelSpeakerArray, unsigned int *channelFillMask)
{
    if (!(isRunning()))
        return;

    // channels seems to be basically determined by soundcard setting
    if (channels == 1)
        return;

    if (!(TalkersPanners->contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,SimplePanner*>* sPanners = TalkersPanners->value(serverConnectionHandlerID);
    if (!(sPanners->contains(clientID)))
        return;

    SimplePanner* panner = sPanners->value(clientID);

    QMap<unsigned int,int> speaker2Channel;
    for(int i=0; i<channels; ++i)
    {
        if (*channelFillMask & FlagList.at(i))
            speaker2Channel.insert(channelSpeakerArray[i],i);
    }
    if (speaker2Channel.size() > 2)    // Todo: For now.
        return;

    if (speaker2Channel.contains(SPEAKER_HEADPHONES_LEFT))
        panner->process(samples,sampleCount,channels,speaker2Channel.value(SPEAKER_HEADPHONES_LEFT),speaker2Channel.value(SPEAKER_HEADPHONES_RIGHT));
    else if (speaker2Channel.contains(SPEAKER_FRONT_LEFT))
        panner->process(samples,sampleCount,channels,speaker2Channel.value(SPEAKER_FRONT_LEFT),speaker2Channel.value(SPEAKER_FRONT_RIGHT));
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
                Log("Could not find Front Speakers.", LogLevel_ERROR,serverConnectionHandlerID);
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
//    QList< QPair<uint64,anyID>* >* seq;
    QList< QPair<uint64,anyID> >* seq;
    QPair<uint64,anyID> seqPair = qMakePair(serverConnectionHandlerID,clientID);
//    Print(QString("scHandler: %1, seqPair: %2,%3").arg(serverConnectionHandlerID).arg(seqPair.first).arg(seqPair.second));
    if (status == STATUS_TALKING)
    {
        SimplePanner* panner = new SimplePanner(this);
        panner->setPanAdjustment(true);

        if (!(TalkersPanners->contains(serverConnectionHandlerID)))
        {
            QMap<anyID,SimplePanner*>* ConnectionHandlerPanners = new QMap<anyID,SimplePanner*>;
            ConnectionHandlerPanners->insert(clientID,panner);
            TalkersPanners->insert(serverConnectionHandlerID,ConnectionHandlerPanners);
        }
        else
        {
            QMap<anyID,SimplePanner*>* ConnectionHandlerPanners = TalkersPanners->value(serverConnectionHandlerID);
            ConnectionHandlerPanners->insert(clientID,panner);
        }

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
                region = m_RegionWhisper;
            else if (serverConnectionHandlerID == m_homeId)
                region = m_RegionHomeTab;
            else
                region = m_RegionOther;

            if (region == TALKERS_REGION_CENTER)
                spread = &SPREAD_CENTER;
            else if (region == TALKERS_REGION_LEFT)
                spread = &SPREAD_LEFT;
            else if (region == TALKERS_REGION_RIGHT)
                spread = &SPREAD_RIGHT;
        }
        seq = TalkerSequences->value(region);

        float val = 0.0f;
        int position = seq->indexOf(SEAT_HOLDER,0);
        if (position != -1)
        {
            seq->replace(position,seqPair);
        }
        else
        {
            seq->append(seqPair);
            position = seq->size() - 1;
        }

        if (spread->size() > position)
            val = spread->at(position) * m_spreadWidth;
        else
            val = spread->at(0) * m_spreadWidth;

        panner->setPanDesiredByPanAdjuster(val);
        if (panner->getPanDesiredByManual() == 0.0f)
            panner->setPanCurrent(val); //don't ramp, pre-set pan

//        Print(QString("Added Panner on position %1 with value %2").arg(position).arg(val));
    }
    else if (status == STATUS_NOT_TALKING)
    {
        if (!TalkersPanners->contains(serverConnectionHandlerID))
        {
            ts->Error(serverConnectionHandlerID,NULL,"(SPS) Trying to remove talker from an invalid server connection handler id.");
            return;
        }

        QMap<anyID,SimplePanner*>* sPanners = TalkersPanners->value(serverConnectionHandlerID);
        if (!(sPanners->contains(clientID)))
        {
            ts->Error(serverConnectionHandlerID,NULL,"(SPS) Trying to remove talker with an invalid client id.");
            return;
        }
        SimplePanner* panner = sPanners->value(clientID);
        panner->setPanAdjustment(false);
        panner->blockSignals(true);
        panner->deleteLater();
        sPanners->remove(clientID);

        if (isReceivedWhisper)
            seq = TalkerSequences->value(m_RegionWhisper);
        else if (serverConnectionHandlerID == m_homeId)
            seq = TalkerSequences->value(m_RegionHomeTab);
        else
            seq = TalkerSequences->value(m_RegionOther);

        if (!(seq->contains(seqPair)))
        {
            for (int i = 0; i<TALKERS_REGION_END; ++i)
            {
                seq = TalkerSequences->value((TALKERS_REGION)i);
//                Print(QString("TalkerSequence region size: %1").arg(seq->size()));
//                if (seq->size() > 0)
//                    Print(QString("seq: %1, %2").arg(seq->first().first).arg(seq->first().second));

                if (seq->contains(seqPair))
                    break;
            }
            if(!(seq->contains(seqPair)))
            {
                ts->Error(serverConnectionHandlerID,NULL,"SPS: Could not find talker to remove.");
                return;
            }
        }

        int position = seq->indexOf(seqPair);
        seq->replace(position,SEAT_HOLDER);
        bool shallClear = true;
        for (int i=0;i<seq->size();++i)
        {
            if (seq->value(i) != SEAT_HOLDER)
            {
                shallClear = false;
                break;
            }
        }
        if (shallClear)
            seq->clear();
    }

//    Print(QString("TalkerSequence region size: %1").arg(seq->size()));
}

void PanTalkers::ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args)
{
    if ((cmd.compare("SPS",Qt::CaseInsensitive)) != 0)
        return;

    cmd = args.at(0);
    if (cmd.compare("SET_BLOCKED",Qt::CaseInsensitive) == 0)
        setBlocked((args.at(1).compare("true",Qt::CaseInsensitive) == 0));
}
