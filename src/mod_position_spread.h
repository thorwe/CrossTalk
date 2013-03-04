#ifndef MOD_POSITION_SPREAD_H
#define MOD_POSITION_SPREAD_H

#include <QObject>
#include "public_definitions.h"
#include "module.h"
#include "talkers.h"
#include "simplepanner.h"

enum TALKERS_REGION {
    TALKERS_REGION_LEFT = 0,
    TALKERS_REGION_CENTER,
    TALKERS_REGION_RIGHT,
    TALKERS_REGION_END
};

class PositionSpread : public Module, public TalkInterface
{
    Q_OBJECT
    Q_INTERFACES(TalkInterface)
    Q_PROPERTY(float spreadWidth
               READ getSpreadWidth
               WRITE setSpreadWidth
               NOTIFY spreadWidthSet)
public:
    explicit PositionSpread(QObject *parent = 0);

    void onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);

    float getSpreadWidth() const;
    void setHomeId(uint64 serverConnectionHandlerID);
    uint64 homeId() {return m_homeId;}

    int ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args);

    bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe);

signals:
    void spreadWidthSet(float);
    void expertModeEnabledSet(bool);
    void regionHomeTabSet(TALKERS_REGION);
    void regionWhisperSet(TALKERS_REGION);
    void regionOtherSet(TALKERS_REGION);

public slots:
    void setSpreadWidth(float value);
    void setExpertModeEnabled(bool value);
    void setRegionHomeTab(int talkersRegion);
    void setRegionWhisper(int talkersRegion);
    void setRegionOther(int talkersRegion);

private:
    float m_spreadWidth;

    Talkers* talkers;

    QMap<TALKERS_REGION,QList< QPair<uint64,anyID> >* >* TalkerSequences;

//    bool isReported;

    QMap<uint64,QMap<anyID,SimplePanner*>* >* TalkersPanners;

    bool m_ExpertModeEnabled;
    uint64 m_homeId;
    TALKERS_REGION m_RegionHomeTab;
    TALKERS_REGION m_RegionWhisper;
    TALKERS_REGION m_RegionOther;

    inline void CheckClearSeq(QList<QPair<uint64, anyID> > *seq);
    void RemoveSeqPair(QPair<uint64, anyID> seqPair, QList<QPair<uint64, anyID> > *seq);
protected:
    void onRunningStateChanged(bool value);
};

#endif // MOD_POSITION_SPREAD_H
