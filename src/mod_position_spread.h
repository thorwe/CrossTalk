#pragma once
#include "simplepanner.h"

#include "core/client_storage.h"
#include "core/definitions.h"
#include "core/module_qt.h"
#include "core/plugin_base.h"

#include "teamspeak/public_definitions.h"

#include <QtCore/QObject>

namespace thorwe
{

using namespace com::teamspeak;

enum TALKERS_REGION
{
    TALKERS_REGION_LEFT = 0,
    TALKERS_REGION_CENTER,
    TALKERS_REGION_RIGHT,
    TALKERS_REGION_END
};
Q_DECLARE_METATYPE(thorwe::TALKERS_REGION)

class PositionSpread : public Module_Qt, public TalkInterface
{
    Q_OBJECT
    Q_INTERFACES(TalkInterface)
    Q_PROPERTY(float spreadWidth READ getSpreadWidth WRITE setSpreadWidth NOTIFY spreadWidthSet)
    Q_PROPERTY(
    bool expertModeEnabled READ isExpertModeEnabled WRITE setExpertModeEnabled NOTIFY expertModeEnabledSet)
    Q_PROPERTY(
    TALKERS_REGION regionHomeTab READ getRegionHomeTab WRITE setRegionHomeTab NOTIFY regionHomeTabSet)
    Q_PROPERTY(
    TALKERS_REGION regionWhisper READ getRegionWhisper WRITE setRegionWhisper NOTIFY regionWhisperSet)
    Q_PROPERTY(TALKERS_REGION regionOther READ getRegionOther WRITE setRegionOther NOTIFY regionOtherSet)
  public:
    PositionSpread(Plugin_Base &plugin);

    void on_playback_post_process(connection_id_t connection_id,
                                  anyID client_id,
                                  gsl::span<int16_t> samples,
                                  int channels,
                                  const unsigned int *channelSpeakerArray,
                                  unsigned int *channelFillMask);

    // property getters
    float getSpreadWidth() const;
    bool isExpertModeEnabled() const;
    TALKERS_REGION getRegionHomeTab() const;
    TALKERS_REGION getRegionWhisper() const;
    TALKERS_REGION getRegionOther() const;

    void setHomeId(connection_id_t connection_id);
    uint64 homeId() { return m_homeId; }

    int ParseCommand(connection_id_t connection_id, QString cmd, QStringList args);

    bool onTalkStatusChanged(
    connection_id_t connection_id, int status, bool isReceivedWhisper, anyID clientID, bool isMe);

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
    float m_spreadWidth = 0.0f;

    Talkers &m_talkers;

    QMap<TALKERS_REGION, QList<QPair<uint64, anyID>> *> *TalkerSequences;
    thorwe::Safe_Client_Storage<SimplePanner> m_talkers_panners;

    bool m_ExpertModeEnabled = false;
    uint64 m_homeId = 0;
    TALKERS_REGION m_RegionHomeTab = TALKERS_REGION_CENTER;
    TALKERS_REGION m_RegionWhisper = TALKERS_REGION_CENTER;
    TALKERS_REGION m_RegionOther = TALKERS_REGION_CENTER;

    inline void CheckClearSeq(QList<QPair<uint64, anyID>> *seq);
    void RemoveSeqPair(QPair<uint64, anyID> seqPair, QList<QPair<uint64, anyID>> *seq);

  protected:
    void onRunningStateChanged(bool value);
};

}  // namespace thorwe
