#include "mod_position_spread.h"

#include "ts3_functions.h"
#include "plugin.h"

const QList<float> SPREAD = QList<float>() << 0 << -0.5f << 0.5f << -1.0f << 1.0f; //<< -0.25f << 0.25f << -0.75f << 0.75f;

const QList<float> SPREAD_CENTER = QList<float>() << 0 << -0.2f << 0.2f << -0.4f << 0.4f << -0.1f << 0.1f << -0.3f << 0.3f;
const QList<float> SPREAD_RIGHT = QList<float>() << 0.75f << 1.0f << 0.5f << 0.875f << 0.625f << 0.9375f << 0.8125f << 0.5625f << 0.6875f;
const QList<float> SPREAD_LEFT = QList<float>() << -0.75f << -1.0f << -0.5f << -0.875f << -0.625f << -0.9375f << -0.8125f << -0.5625f << -0.6875f;

const QPair<uint64,anyID> SEAT_HOLDER = qMakePair((uint64)0,(anyID)0);

PositionSpread::PositionSpread(QObject *parent)
{
    this->setParent(parent);
    this->setObjectName(QStringLiteral("Position Spread"));
    m_isPrintEnabled = false;
    talkers = Talkers::instance();
    TalkersPanners = new QMap<uint64,QMap<anyID,SimplePanner*>* >;
    TalkerSequences = new QMap<TALKERS_REGION,QList< QPair<uint64,anyID> >* >;
    for (int i = 0; i < TALKERS_REGION_END; ++i)
    {
        QList< QPair<uint64,anyID> >* list = new QList< QPair<uint64,anyID> >;
        TalkerSequences->insert((TALKERS_REGION)i,list);
    }
}

//Properties

float PositionSpread::getSpreadWidth() const
{
    return m_spreadWidth;
}

bool PositionSpread::isExpertModeEnabled() const
{
    return m_ExpertModeEnabled;
}

TALKERS_REGION PositionSpread::getRegionHomeTab() const
{
    return m_RegionHomeTab;
}

TALKERS_REGION PositionSpread::getRegionWhisper() const
{
    return m_RegionWhisper;
}

TALKERS_REGION PositionSpread::getRegionOther() const
{
    return m_RegionOther;
}

void PositionSpread::setHomeId(uint64 serverConnectionHandlerID)
{
    if (serverConnectionHandlerID == m_homeId)
        return;
    m_homeId = serverConnectionHandlerID;
    if (m_homeId == 0)
        return;
    if ((isRunning()) && (m_ExpertModeEnabled))
        talkers->DumpTalkStatusChanges(this,true);
}

void PositionSpread::onRunningStateChanged(bool value)
{
//    if (value)
//        connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
//    else
//        disconnect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
//    talkers->RegisterEventTalkStatusChange(this,value);
    talkers->DumpTalkStatusChanges(this,((value)?STATUS_TALKING:STATUS_NOT_TALKING));
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

void PositionSpread::setSpreadWidth(float value)
{
    if (m_spreadWidth == value)
        return;
    m_spreadWidth = value;
    if (isRunning())
        talkers->DumpTalkStatusChanges(this,true);

    Log(QString("setSpreadWidth: %1").arg(m_spreadWidth));
    emit spreadWidthSet(value);
}

void PositionSpread::setExpertModeEnabled(bool value)
{
    if (m_ExpertModeEnabled == value)
        return;
    m_ExpertModeEnabled = value;
    if (isRunning())
        talkers->DumpTalkStatusChanges(this,true);

    Log(QString("setExpertModeEnabled: %1").arg((m_ExpertModeEnabled)?"true":"false"));
    emit expertModeEnabledSet(value);
}

void PositionSpread::setRegionHomeTab(int talkersRegion)
{
    if (m_RegionHomeTab == (TALKERS_REGION)talkersRegion)
        return;
    m_RegionHomeTab = (TALKERS_REGION)talkersRegion;
    if ((isRunning()) && (m_ExpertModeEnabled))
        talkers->DumpTalkStatusChanges(this,true);

    Log(QString("setRegionHomeTab: %1").arg(talkersRegion));
    emit regionHomeTabSet(m_RegionHomeTab);
}

void PositionSpread::setRegionWhisper(int talkersRegion)
{
    if (m_RegionWhisper == (TALKERS_REGION)talkersRegion)
        return;
    m_RegionWhisper = (TALKERS_REGION)talkersRegion;
    if ((isRunning()) && (m_ExpertModeEnabled))
        talkers->DumpTalkStatusChanges(this,true);

    Log(QString("setRegionWhisper: %1").arg(talkersRegion));
    emit regionWhisperSet(m_RegionWhisper);
}

void PositionSpread::setRegionOther(int talkersRegion)
{
    if (m_RegionOther ==  (TALKERS_REGION)talkersRegion)
        return;
    m_RegionOther = (TALKERS_REGION)talkersRegion;
    if ((isRunning()) && (m_ExpertModeEnabled))
        talkers->DumpTalkStatusChanges(this,true);

    Log(QString("setRegionOther: %1").arg(talkersRegion));
    emit regionOtherSet(m_RegionOther);
}

void PositionSpread::onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels, const unsigned int *channelSpeakerArray, unsigned int *channelFillMask)
{
    if (!(isRunning()))
        return;

    // no data in any channel -> nothing to see here move along
    if (*channelFillMask == 0)
        return;

    // channels seems to be basically determined by soundcard setting
    if (channels == 1)
        return;

    if (!(TalkersPanners->contains(serverConnectionHandlerID)))
        return;

    auto sPanners = TalkersPanners->value(serverConnectionHandlerID);
    if (!(sPanners->contains(clientID)))
        return;

    auto panner = sPanners->value(clientID);

    QMap<unsigned int,int> speaker2Channel;
    for(int i=0; i < channels; ++i)
    {
        if (*channelFillMask & (1 << i))
            speaker2Channel.insert(channelSpeakerArray[i],i);
    }

    /*if (speaker2Channel.size() > 2)
    {   // Todo: For now.
        //Log("speaker2Channel.size() > 2: return");
        //return;
    }*/

    if (speaker2Channel.contains(SPEAKER_HEADPHONES_LEFT) && speaker2Channel.contains(SPEAKER_HEADPHONES_RIGHT))
        panner->process(samples,sampleCount,channels,speaker2Channel.value(SPEAKER_HEADPHONES_LEFT),speaker2Channel.value(SPEAKER_HEADPHONES_RIGHT));
    else if (speaker2Channel.contains(SPEAKER_FRONT_LEFT))
        panner->process(samples,sampleCount,channels,speaker2Channel.value(SPEAKER_FRONT_LEFT),speaker2Channel.value(SPEAKER_FRONT_RIGHT));
    else if (speaker2Channel.contains(SPEAKER_FRONT_CENTER))    // TODO: This is effectively never called in practice
    {
        if (!((panner->getPanDesired() == panner->getPanCurrent()) && (panner->getPanDesired() == 0.0f))) // if middle is where it is and is to be, leave it at center, otherwise...
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
                Log("Could not find Front Speakers.",serverConnectionHandlerID, LogLevel_ERROR);
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
}

bool PositionSpread::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe)
{
    if (isMe || !isRunning())
        return false;

//    QList< QPair<uint64,anyID>* >* seq;
    QList< QPair<uint64,anyID> >* seq;
    auto seqPair = qMakePair(serverConnectionHandlerID,clientID);
//    Print(QString("scHandler: %1, seqPair: %2,%3").arg(serverConnectionHandlerID).arg(seqPair.first).arg(seqPair.second));
    if (status == STATUS_TALKING)
    {   // Robust against multiple STATUS_TALKING in a row to be able to use it when the user changes settings
        SimplePanner* panner;
        bool isNewDspObj = true;
        if (!(TalkersPanners->contains(serverConnectionHandlerID)))
        {
            panner = new SimplePanner(this);
            auto ConnectionHandlerPanners = new QMap<anyID,SimplePanner*>;
            ConnectionHandlerPanners->insert(clientID,panner);
            TalkersPanners->insert(serverConnectionHandlerID,ConnectionHandlerPanners);
        }
        else
        {
            auto ConnectionHandlerPanners = TalkersPanners->value(serverConnectionHandlerID);
            if (ConnectionHandlerPanners->contains(clientID))
            {
                panner = ConnectionHandlerPanners->value(clientID);
                isNewDspObj = false;
            }
            else
            {
                panner = new SimplePanner(this);
                ConnectionHandlerPanners->insert(clientID,panner);
            }
        }
        panner->setPanAdjustment(true);

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

        if ((!isNewDspObj) && (!seq->contains(seqPair)))
        {   // some user setting change that demands for clean up
            RemoveSeqPair(seqPair,seq);
            seq = TalkerSequences->value(region);
        }

        int position;
        if (seq->contains(seqPair))
            position = seq->indexOf(seqPair,0);
        else
        {
            position = seq->indexOf(SEAT_HOLDER,0);
            if (position != -1)
                seq->replace(position,seqPair);
            else
            {
//                Print(QString("%1,%2,%3").arg((seq==NULL)?"seq is NULL":"seq is not NULL").arg(seqPair.first).arg(seqPair.second));
//                Log(QString("%1,%2,%3").arg((seq==NULL)?"seq is NULL":"seq is not NULL").arg(seqPair.first).arg(seqPair.second));
                seq->append(seqPair);
                position = seq->size() - 1;
            }
        }

        float val = 0.0f;
        if (spread->size() > position)
            val = spread->at(position) * m_spreadWidth;
        else
            val = spread->at(0) * m_spreadWidth;

        panner->setPanDesiredByPanAdjuster(val);
        if ((panner->getPanDesiredByManual() == 0.0f) && (isNewDspObj))
            panner->setPanCurrent(val); //don't ramp, pre-set pan

//        Print(QString("Added Panner on position %1 with value %2").arg(position).arg(val));
        return true;
    }
    else if (status == STATUS_NOT_TALKING)
    {
        // Removing does not need to be robust against multiple STATUS_NOT_TALKING in a row, since that doesn't happen on user setting change
        if (!TalkersPanners->contains(serverConnectionHandlerID))
        {
            // return silent bec. of ChannelMuter implementation
//            Error("(onTalkStatusChanged) Trying to remove talker from an invalid server connection handler id.",serverConnectionHandlerID,NULL);
            return false;
        }

        auto sPanners = TalkersPanners->value(serverConnectionHandlerID);
        if (!(sPanners->contains(clientID)))
        {
            //Error("(onTalkStatusChanged) Trying to remove talker with an invalid client id.",serverConnectionHandlerID,NULL);
            return false;
        }
        auto panner = sPanners->value(clientID);
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

        RemoveSeqPair(seqPair,seq);
    }
    return false;
//    Print(QString("TalkerSequence region size: %1").arg(seq->size()));
}

void PositionSpread::CheckClearSeq(QList< QPair<uint64,anyID> >* seq)
{
    if (seq->isEmpty())
        return;

    auto shallClear = true;
    for (int i=0;i<seq->size();++i)
    {
        if (seq->value(i) != SEAT_HOLDER)
        {
            shallClear = false;
            break;
        }
    }
    if (shallClear)
    {
        seq->clear();
//        Print("Seq cleared.");
    }
}

void PositionSpread::RemoveSeqPair(QPair<uint64,anyID> seqPair,QList< QPair<uint64,anyID> >* seq)
{
//    Print(QString("RemoveSeqPair: <%1,%2>").arg(seqPair.first).arg(seqPair.second));
//    int position = (seq->contains(seqPair))?seq->indexOf(seqPair):FindSeqPair(seqPair,seq);
    int position;
    if (seq->contains(seqPair))
            position = seq->indexOf(seqPair);
    else
    {
        for (int i = 0; i < TALKERS_REGION_END; ++i)
        {
            seq = TalkerSequences->value((TALKERS_REGION)i);
            if (seq->contains(seqPair))
                break;
        }
        if(seq->contains(seqPair))
            position = seq->indexOf(seqPair);
        else
            position = -1;
    }

    if (position == -1)
    {
        Error(QString("(FindSeqPair) Could not find clientID %1 in sequence.").arg(seqPair.second),seqPair.first,NULL);
        return;
    }
//    Print(QString("Position: %1, Size: %2").arg(position).arg(seq->size()));
    if (seq->size() == 0)
    {
        Error("seq size is 0");
        return;
    }

    seq->replace(position,SEAT_HOLDER);
    CheckClearSeq(seq);
}

/*!
 * \brief PositionSpread::ParseCommand
 * \param serverConnectionHandlerID
 * \param cmd
 * \param args
 * \return Return 0 if plugin handled the command, 1 if not handled.
 */
int PositionSpread::ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args)
{
    Q_UNUSED(serverConnectionHandlerID);

    if ((cmd.compare("SPS",Qt::CaseInsensitive)) != 0)
        return 1;

    cmd = args.at(0);
    if (cmd.compare(QLatin1String("SET_BLOCKED"),Qt::CaseInsensitive) == 0)
    {
        setBlocked((args.at(1).compare(QLatin1String("true"),Qt::CaseInsensitive) == 0));
        return 0;
    }
    return 1;
}
