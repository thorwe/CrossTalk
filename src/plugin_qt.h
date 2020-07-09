#pragma once

#include "core/definitions.h"
#include "core/plugin_base.h"

#include "QtWebsocket/ServerThreaded/ServerThreaded.h"

#include <gsl/pointers>

#include <string_view>

class Agmu;
class SnT;
class TSServersInfo;

namespace thorwe
{
class PositionSpread;
class SettingsPositionSpread;
class ChannelMuter;
class PositionalAudio;
class SettingsPositionalAudio;

using namespace com::teamspeak;

class Plugin : public Plugin_Base
{
    Q_OBJECT

  public:
    Plugin(const char *plugin_id, QObject *parent = nullptr);

    static const char *kPluginName;
    static const char *kPluginVersion;
    static const int kPluginApiVersion = 23;
    static const char *kPluginAuthor;
    static const char *kPluginDescription;

    int initialize() override;
    void shutdown() override;

    /* optional */
    void configure(void *handle, void *qParentWidget) override;
    static const int kPluginOffersConfigure = PLUGIN_OFFERS_CONFIGURE_QT_THREAD;

    int process_command(connection_id_t connection_id, const QString &command, QStringList &args) override;
    void on_current_server_connection_changed(connection_id_t connection_id) override;

    /* clientlib */
    void on_connect_status_changed(connection_id_t connection_id,
                                   int new_status,
                                   unsigned int error_number) override;
    void on_client_move(connection_id_t connection_id,
                        anyID client_id,
                        uint64 old_channel_id,
                        uint64 new_channel_id,
                        int visibility,
                        anyID my_id,
                        const char *move_message) override;
    void on_client_move_timeout(connection_id_t connection_id,
                                anyID client_id,
                                uint64 old_channel_id,
                                anyID my_id,
                                const char *timeout_message) override;
    void on_client_move_moved(connection_id_t connection_id,
                              anyID client_id,
                              uint64 old_channel_id,
                              uint64 new_channel_id,
                              int visibility,
                              anyID my_id,
                              anyID mover_id,
                              const char *mover_name,
                              const char *mover_unique_id,
                              const char *move_message) override;

    int on_server_error(com::teamspeak::connection_id_t connection_id,
                        std::string_view error_message,
                        ts_errc error,
                        std::string_view return_code,
                        std::string_view extra_message) final override;

    void on_talk_status_changed(
    connection_id_t connection_id, int status, int is_received_whisper, anyID client_id, bool is_me) override;
    void on_playback_pre_process(
    connection_id_t connection_id, anyID client_id, short *samples, int frame_count, int channels) override;
    void on_playback_post_process(connection_id_t connection_id,
                                  anyID client_id,
                                  gsl::span<int16_t> samples,
                                  int channels,
                                  const unsigned int *channel_speaker_array,
                                  unsigned int *channel_fill_mask) override;
    void on_custom_3d_rolloff_calculation(connection_id_t connection_id,
                                          anyID client_id,
                                          float distance,
                                          float *volume) override;

    /* clientlib rare */
    void on_client_self_variable_update(connection_id_t connection_id,
                                        int flag,
                                        const char *old_value,
                                        const char *new_value) override;

    void on_server_group_list(connection_id_t connection_id,
                              uint64 server_group_id,
                              const char *name,
                              int type,
                              int icon_id,
                              int save_db) override;
    void on_server_group_list_finished(connection_id_t connection_id) override;
    void on_channel_group_list(connection_id_t connection_id,
                               uint64 channel_group_id,
                               const char *name,
                               int type,
                               int icon_id,
                               int save_db) override;
    void on_channel_group_list_finished(connection_id_t connection_id) override;

    void on_plugin_command(connection_id_t connection_id,
                           const char *plugin_name,
                           const char *plugin_command,
                           anyID invoker_client_id,
                           const char *invoker_name,
                           const char *invoker_uid) override;

    TSServersInfo &servers_info() { return *m_servers_info; }
    ServerThreaded &websocket_server() { return *m_websocket_server; };

  private:
    gsl::owner<TSServersInfo *> m_servers_info = nullptr;
    gsl::owner<ServerThreaded *> m_websocket_server = nullptr;
    gsl::owner<ChannelMuter *> m_channel_muter = nullptr;
    gsl::owner<PositionSpread *> m_position_spread = nullptr;
    gsl::owner<SettingsPositionSpread *> m_settings_position_spread = nullptr;
    gsl::owner<Agmu *> m_agmu = nullptr;
    gsl::owner<PositionalAudio *> m_positional_audio = nullptr;
    gsl::owner<SettingsPositionalAudio *> m_settings_positional_audio = nullptr;
    gsl::owner<SnT *> m_snt = nullptr;
};

}  // namespace thorwe
