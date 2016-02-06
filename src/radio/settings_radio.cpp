#include "settings_radio.h"

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

#include "plugin.h" //pluginID
#include "ts_serversinfo.h"
#include <teamspeak/public_errors.h>

SettingsRadio* SettingsRadio::m_Instance = 0;
SettingsRadio::SettingsRadio() :
    m_ContextMenuUi(-1)
{
    this->setObjectName("SettingsRadio");
}
SettingsRadio::~SettingsRadio(){}

void SettingsRadio::Init(Radio *radio)
{
    if(m_ContextMenuUi == -1)
    {
        m_ContextMenuUi = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_GLOBAL,"Radio FX","walkie_talkie_16.png");
        m_ContextMenuChannelUi = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_CHANNEL,"Radio FX (Channel)","walkie_talkie_16.png");
        m_ContextMenuToggleClientBlacklisted = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_CLIENT, "Radio FX: Toggle Client Blacklisted [temp]", "walkie_talkie_16.png");
        connect(TSContextMenu::instance(), &TSContextMenu::MenusInitialized, this, &SettingsRadio::onMenusInitialized, Qt::AutoConnection);
        connect(TSContextMenu::instance(), &TSContextMenu::FireContextMenuEvent, this, &SettingsRadio::onContextMenuEvent, Qt::AutoConnection);

        TSInfoData::instance()->Register(this,true,1);
    }

    connect(this, &SettingsRadio::EnabledSet, radio, &Radio::setChannelStripEnabled);
    connect(this, &SettingsRadio::InLoFreqSet, radio, &Radio::setInLoFreq);
    connect(this, &SettingsRadio::InHiFreqSet, radio, &Radio::setInHiFreq);
    connect(this, &SettingsRadio::DestructionSet, radio, &Radio::setFudge);
    connect(this, &SettingsRadio::RingModFrequencySet, radio, &Radio::setRingModFrequency);
    connect(this, &SettingsRadio::RingModMixSet, radio, &Radio::setRingModMix);
    connect(this, &SettingsRadio::OutLoFreqSet, radio, &Radio::setOutLoFreq);
    connect(this, &SettingsRadio::OutHiFreqSet, radio, &Radio::setOutHiFreq);

    connect(this, &SettingsRadio::ToggleClientBlacklisted, radio, &Radio::ToggleClientBlacklisted);

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup(radio->objectName());

    QStringList stringList = cfg.childGroups();
    //stringList << "HomeTab" << "Whisper" << "Other";
    for (int i = 0; i<stringList.size(); ++i)
    {
        QString name = stringList.at(i);
        cfg.beginGroup(name);
        if (name == "HomeTab")
            name = "Home";

        radio->setChannelStripEnabled(name,cfg.value("enabled",false).toBool());
        radio->setInLoFreq(name,cfg.value("low_freq",300.0).toDouble());
        radio->setInHiFreq(name,cfg.value("high_freq",3000.0).toDouble());
        radio->setFudge(name,cfg.value("fudge",2.0).toDouble());
        cfg.beginGroup("RingMod");
        radio->setRingModFrequency(name,cfg.value("rm_mod_freq",0.0f).toDouble());
        radio->setRingModMix(name,cfg.value("rm_mix",0.0f).toDouble());
        cfg.endGroup();
        radio->setOutLoFreq(name,cfg.value("o_freq_lo",300.0).toDouble());
        radio->setOutHiFreq(name,cfg.value("o_freq_hi",3000.0).toDouble());
        cfg.endGroup();
    }

    radio->setEnabled(true);
    mP_radio = radio;
}

bool SettingsRadio::onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, uint64 mine, QTextStream &data)
{
    bool isDirty = false;
    if (type == PLUGIN_CLIENT)
    {
        if (m_ContextMenuToggleClientBlacklisted != -1)
            ts3Functions.setPluginMenuEnabled(pluginID,m_ContextMenuToggleClientBlacklisted,(id != mine)?1:0);

        if ((id != mine) && (mP_radio != 0) && (mP_radio.data()->isClientBlacklisted(serverConnectionHandlerID,(anyID)id)))
        {
            data << mP_radio.data()->objectName() << ":";
            isDirty = true;
            data << "blacklisted [temp]";
        }
    }
    return isDirty;
}

void SettingsRadio::onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
    if (type == PLUGIN_MENU_TYPE_GLOBAL)
    {
        if (menuItemID == m_ContextMenuUi)
        {
            if (config)
                config.data()->activateWindow();
            else if(mP_radio)
            {
                ConfigRadio* p_config = new ConfigRadio(TSHelpers::GetMainWindow());  //has delete on close attribute

                QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
                Radio* radio = mP_radio.data();
                cfg.beginGroup(radio->objectName());

                QStringList stringList;
                stringList << "HomeTab" << "Whisper" << "Other";
                for (int i = 0; i<stringList.size(); ++i)
                {
                    QString name = stringList.at(i);
                    cfg.beginGroup(name);
                    if (name == "HomeTab")
                        name = "Home";

                    p_config->UpdateEnabled(name,cfg.value("enabled",false).toBool());
                    p_config->UpdateBandpassInLowFrequency(name,cfg.value("low_freq",300.0).toDouble());
                    p_config->UpdateBandpassInHighFrequency(name,cfg.value("high_freq",3000.0).toDouble());
                    p_config->UpdateDestruction(name,cfg.value("fudge",2.0).toDouble());
                    cfg.beginGroup("RingMod");
                    p_config->UpdateRingModFrequency(name,cfg.value("rm_mod_freq",0.0f).toDouble());
                    p_config->UpdateRingModMix(name,cfg.value("rm_mix",0.0f).toDouble());
                    cfg.endGroup();
                    p_config->UpdateBandpassOutLowFrequency(name,cfg.value("o_freq_lo",300.0).toDouble());
                    p_config->UpdateBandpassOutHighFrequency(name,cfg.value("o_freq_hi",3000.0).toDouble());
                    cfg.endGroup();
                }

                connect(p_config, &ConfigRadio::EnabledSet, this, &SettingsRadio::EnabledSet);
                connect(p_config, &ConfigRadio::InLoFreqSet, this, &SettingsRadio::InLoFreqSet);
                connect(p_config, &ConfigRadio::InHiFreqSet, this,  &SettingsRadio::InHiFreqSet);
                connect(p_config, &ConfigRadio::DestructionSet, this, &SettingsRadio::DestructionSet);
                connect(p_config, &ConfigRadio::RingModFrequencySet, this, &SettingsRadio::RingModFrequencySet);
                connect(p_config, &ConfigRadio::RingModMixSet, this, &SettingsRadio::RingModMixSet);
                connect(p_config, &ConfigRadio::OutLoFreqSet, this, &SettingsRadio::OutLoFreqSet);
                connect(p_config, &ConfigRadio::OutHiFreqSet, this, &SettingsRadio::OutHiFreqSet);

                connect(p_config, &ConfigRadio::finished, this, &SettingsRadio::saveSettings);
                p_config->show();
                config = p_config;
            }
        }
    }
    else if (type == PLUGIN_MENU_TYPE_CHANNEL)
    {
        if (menuItemID == m_ContextMenuChannelUi)
        {
            QPair<uint64, uint64> server_channel = qMakePair<uint64, uint64> (serverConnectionHandlerID, selectedItemID);
            if (m_channel_configs.contains(server_channel) && m_channel_configs.value(server_channel))
            {
                m_channel_configs.value(server_channel).data()->activateWindow();
                return;
            }

            if (!mP_radio)
                return;

            unsigned int error;

            char* channel_name_c;
            if ((error = ts3Functions.getChannelVariableAsString(serverConnectionHandlerID, selectedItemID, CHANNEL_NAME, &channel_name_c)) != ERROR_ok)
            {
                TSLogging::Error(QString("%1: Could not get channel name").arg(this->objectName()));
                return;
            }
            QString channel_name = QString::fromUtf8(channel_name_c);
            ts3Functions.freeMemory(channel_name_c);

            ConfigRadio* p_config = new ConfigRadio(TSHelpers::GetMainWindow(), TSServersInfo::instance()->GetServerInfo(serverConnectionHandlerID)->getName() + ":" + channel_name);  //has delete on close attribute
            QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
            cfg.beginGroup(mP_radio.data()->objectName());

            QString custom_channel_id = TSServersInfo::instance()->GetServerInfo(serverConnectionHandlerID)->getUniqueId() + TSHelpers::GetChannelPath(serverConnectionHandlerID, selectedItemID);
            cfg.beginGroup(custom_channel_id);

            // also push temp default setting to radio
            mP_radio->setChannelStripEnabled(custom_channel_id,cfg.value("enabled",false).toBool());
            mP_radio->setInLoFreq(custom_channel_id,cfg.value("low_freq",300.0).toDouble());
            mP_radio->setInHiFreq(custom_channel_id,cfg.value("high_freq",3000.0).toDouble());
            mP_radio->setFudge(custom_channel_id,cfg.value("fudge",2.0).toDouble());
            p_config->UpdateEnabled(custom_channel_id,cfg.value("enabled",false).toBool());
            p_config->UpdateBandpassInLowFrequency(custom_channel_id,cfg.value("low_freq",300.0).toDouble());
            p_config->UpdateBandpassInHighFrequency(custom_channel_id,cfg.value("high_freq",3000.0).toDouble());
            p_config->UpdateDestruction(custom_channel_id,cfg.value("fudge",2.0).toDouble());
            cfg.beginGroup("RingMod");
            mP_radio->setRingModFrequency(custom_channel_id,cfg.value("rm_mod_freq",0.0f).toDouble());
            mP_radio->setRingModMix(custom_channel_id,cfg.value("rm_mix",0.0f).toDouble());
            p_config->UpdateRingModFrequency(custom_channel_id,cfg.value("rm_mod_freq",0.0f).toDouble());
            p_config->UpdateRingModMix(custom_channel_id,cfg.value("rm_mix",0.0f).toDouble());
            cfg.endGroup();
            mP_radio->setOutLoFreq(custom_channel_id,cfg.value("o_freq_lo",300.0).toDouble());
            mP_radio->setOutHiFreq(custom_channel_id,cfg.value("o_freq_hi",3000.0).toDouble());
            p_config->UpdateBandpassOutLowFrequency(custom_channel_id,cfg.value("o_freq_lo",300.0).toDouble());
            p_config->UpdateBandpassOutHighFrequency(custom_channel_id,cfg.value("o_freq_hi",3000.0).toDouble());

            cfg.endGroup(); // Channel
            cfg.endGroup(); // radio module

            connect(p_config, &ConfigRadio::EnabledSet, this, &SettingsRadio::EnabledSet);
            connect(p_config, &ConfigRadio::InLoFreqSet, this, &SettingsRadio::InLoFreqSet);
            connect(p_config, &ConfigRadio::InHiFreqSet, this, &SettingsRadio::InHiFreqSet);
            connect(p_config, &ConfigRadio::DestructionSet, this, &SettingsRadio::DestructionSet);
            connect(p_config, &ConfigRadio::RingModFrequencySet, this, &SettingsRadio::RingModFrequencySet);
            connect(p_config, &ConfigRadio::RingModMixSet, this, &SettingsRadio::RingModMixSet);
            connect(p_config, &ConfigRadio::OutLoFreqSet, this, &SettingsRadio::OutLoFreqSet);
            connect(p_config, &ConfigRadio::OutHiFreqSet, this, &SettingsRadio::OutHiFreqSet);

            connect(p_config, &ConfigRadio::channel_closed, this, &SettingsRadio::on_channel_settings_finished);
            p_config->show();
            m_channel_configs.insert(server_channel,p_config);
        }
    }
    else if (type == PLUGIN_MENU_TYPE_CLIENT)
    {
        if (menuItemID == m_ContextMenuToggleClientBlacklisted)
            emit ToggleClientBlacklisted(serverConnectionHandlerID, (anyID)selectedItemID);
    }
}

void SettingsRadio::onMenusInitialized()
{
    if(m_ContextMenuUi == -1)
        TSLogging::Error(QString("%1: Menu wasn't registered.").arg(this->objectName()));

    if (m_ContextMenuChannelUi == -1)
        TSLogging::Error(QString("%1: Channel Menu wasn't registered.").arg(this->objectName()));

    if(m_ContextMenuToggleClientBlacklisted == -1)
        TSLogging::Error(QString("%1: Toggle Client Blacklisted menu item wasn't registered.").arg(this->objectName()));
}

void SettingsRadio::saveSettings(int r)
{
    Q_UNUSED(r);
    if(mP_radio)
    {
        QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
        Radio* radio = mP_radio.data();
        cfg.beginGroup(radio->objectName());

        QMapIterator<QString,RadioFX_Settings> i(radio->GetSettingsMap());
        {
            while (i.hasNext())
            {
                i.next();

                QString name = i.key();
                if (name == "Home")
                    name = "HomeTab";

                cfg.beginGroup(name);
                RadioFX_Settings settings = i.value();
                cfg.setValue("enabled",settings.enabled);
                cfg.setValue("low_freq",settings.freq_low);
                cfg.setValue("high_freq",settings.freq_hi);
                cfg.setValue("o_freq_lo",settings.o_freq_lo);
                cfg.setValue("o_freq_hi",settings.o_freq_hi);
                cfg.setValue("fudge",settings.fudge);
                cfg.beginGroup("RingMod");
                cfg.setValue("rm_mod_freq",settings.rm_mod_freq);
                cfg.setValue("rm_mix",settings.rm_mix);
                cfg.endGroup();
                cfg.endGroup();
            }
        }
        cfg.endGroup();

        emit settingsSave();
    }
}

void SettingsRadio::on_channel_settings_finished(int r, QString setting_id)
{
    // clean up and close widget
    uint64 serverConnectionHandlerID = TSServersInfo::instance()->FindServerByUniqueId(setting_id.left(28));    // 28 length always? or always ends with = ?
    if (serverConnectionHandlerID != NULL)
    {
        uint64 channel_id = TSHelpers::GetChannelIDFromPath(serverConnectionHandlerID, setting_id.right(setting_id.length() - 28));
        if (channel_id != NULL)
        {
            //TSLogging::Log(QString("serverid: %1 channelid: %2").arg(serverConnectionHandlerID).arg(channel_id));

            QPair<uint64, uint64> server_channel = qMakePair<uint64, uint64>(serverConnectionHandlerID,channel_id);
            if (!m_channel_configs.contains(server_channel))
            {
                TSLogging::Error("Could not remove setting dialog from map");
            }
            else
                m_channel_configs.remove(server_channel);
        }
    }

    // Settings
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup(mP_radio.data()->objectName());

    if (r == QDialog::DialogCode::Accepted) // delete button
    {
        TSLogging::Log(QString("Removing setting %1").arg(setting_id));

        // remove from setting
        cfg.remove(setting_id);
        cfg.endGroup();

        // This makes me wish for a redesign
        QMap<QString, RadioFX_Settings> &settings_map = mP_radio->GetSettingsMapRef();
        if (settings_map.contains(setting_id))
            settings_map.remove(setting_id);

        // TODO Update all current talkers
    }
    else    // save / create
    {
        TSLogging::Log(QString("Save channel settings: %1").arg(setting_id));

        // First: Check if setting exists
        bool already_exists = cfg.childGroups().contains(setting_id);
        if (already_exists)
        {
            this->saveSettings(NULL);
            return;
        }

        // else if not enabled remove setting (aka don't create setting) to not pollute for every dialog open
        // This makes me wish for a redesign
        QMap<QString, RadioFX_Settings> &settings_map = mP_radio->GetSettingsMapRef();

        if ((settings_map.contains(setting_id)) && (!settings_map.value(setting_id).enabled))
            settings_map.remove(setting_id);
        else
        {
            this->saveSettings(NULL);
            return;
        }
    }
    cfg.endGroup();
}
