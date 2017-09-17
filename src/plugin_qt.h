#pragma once

#include "core/plugin_base.h"
#include "QtWebSocket/ServerThreaded/ServerThreaded.h"

class ChannelMuter;
class PositionSpread;
class SettingsPositionSpread;
class Agmu;
class PositionalAudio;
class SettingsPositionalAudio;
class SnT;
class TSServersInfo;

class Plugin : public Plugin_Base
{
    Q_OBJECT

public:
    Plugin(const char* plugin_id, QObject *parent = nullptr);

    static const char* kPluginName;
    static const char* kPluginVersion;
    static const int kPluginApiVersion = 22;
    static const char* kPluginAuthor;
    static const char* kPluginDescription;

    int initialize() override;
    void shutdown() override;

    /* optional */
    void configure(void* handle, void* qParentWidget) override;
    static const int kPluginOffersConfigure = PLUGIN_OFFERS_CONFIGURE_QT_THREAD;

    int process_command(uint64 sch_id, const QString& command, QStringList& args) override;
    void on_current_server_connection_changed(uint64 sch_id) override;
    
    /* clientlib */
    void on_connect_status_changed(uint64 sch_id, int new_status, unsigned int error_number) override;
    void on_client_move(uint64 sch_id, anyID client_id, uint64 old_channel_id, uint64 new_channel_id, int visibility, anyID my_id, const char* move_message) override;
    void on_client_move_timeout(uint64 sch_id, anyID client_id, uint64 old_channel_id, anyID my_id, const char* timeout_message) override;
    void on_client_move_moved(uint64 sch_id, anyID client_id, uint64 old_channel_id, uint64 new_channel_id, int visibility, anyID my_id, anyID mover_id, const char* mover_name, const char* mover_unique_id, const char* move_message) override;

    int on_server_error(uint64 sch_id, const char* error_message, unsigned int error, const char* return_code, const char* extra_message) override;

    void on_talk_status_changed(uint64 sch_id, int status, int is_received_whisper, anyID client_id, bool is_me) override;
    void on_playback_pre_process(uint64 sch_id, anyID client_id, short* samples, int frame_count, int channels) override;
    void on_playback_post_process(uint64 sch_id, anyID client_id, short* samples, int frame_count, int channels, const unsigned int* channel_speaker_array, unsigned int* channel_fill_mask) override;
    void on_custom_3d_rolloff_calculation(uint64 sch_id, anyID client_id, float distance, float* volume) override;

    /* clientlib rare */
    void on_client_self_variable_update(uint64 sch_id, int flag, const char* old_value, const char* new_value) override;

    void on_server_group_list(uint64 sch_id, uint64 server_group_id, const char* name, int type, int icon_id, int save_db) override;
    void on_server_group_list_finished(uint64 sch_id) override;
    void on_channel_group_list(uint64 sch_id, uint64 channel_group_id, const char* name, int type, int icon_id, int save_db) override;
    void on_channel_group_list_finished(uint64 sch_id) override;

    void on_plugin_command(uint64 sch_id, const char* plugin_name, const char* plugin_command) override;

    TSServersInfo& servers_info() { return *m_servers_info; }
    ServerThreaded& websocket_server() { return *m_websocket_server; };

private:
    TSServersInfo* m_servers_info = nullptr;
    ServerThreaded* m_websocket_server = nullptr;
    ChannelMuter* m_channel_muter = nullptr;
    PositionSpread* m_position_spread = nullptr;
    SettingsPositionSpread* m_settings_position_spread = nullptr;
    Agmu* m_agmu = nullptr;
    PositionalAudio* m_positional_audio = nullptr;
    SettingsPositionalAudio* m_settings_positional_audio = nullptr;
    SnT* m_snt = nullptr;
};
