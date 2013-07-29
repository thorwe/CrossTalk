#include "settings_radio.h"

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

#include "plugin.h" //pluginID

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
        m_ContextMenuToggleClientBlacklisted = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_CLIENT, "Radio FX: Toggle Client Blacklisted [temp]", "walkie_talkie_16.png");
        connect(TSContextMenu::instance(),SIGNAL(MenusInitialized()),SLOT(onMenusInitialized()),Qt::AutoConnection);
        connect(TSContextMenu::instance(),SIGNAL(FireContextMenuEvent(uint64,PluginMenuType,int,uint64)),SLOT(onContextMenuEvent(uint64,PluginMenuType,int,uint64)),Qt::AutoConnection);

        TSInfoData::instance()->Register(this,true,1);
    }

    this->connect(this, SIGNAL(HomeEnabledSet(bool)), radio, SLOT(setEnabledHomeTab(bool)));
    this->connect(this, SIGNAL(HomeLowFrequencySet(double)), radio, SLOT(setLowFrequencyHomeTab(double)));
    this->connect(this, SIGNAL(HomeHighFrequencySet(double)), radio, SLOT(setHighFrequencyHomeTab(double)));
    this->connect(this, SIGNAL(HomeDestructionSet(double)), radio, SLOT(setFudgeHomeTab(double)));

    this->connect(this, SIGNAL(WhisperEnabledSet(bool)), radio, SLOT(setEnabledWhisper(bool)));
    this->connect(this, SIGNAL(WhisperLowFrequencySet(double)), radio, SLOT(setLowFrequencyWhisper(double)));
    this->connect(this, SIGNAL(WhisperHighFrequencySet(double)), radio, SLOT(setHighFrequencyWhisper(double)));
    this->connect(this, SIGNAL(WhisperDestructionSet(double)), radio, SLOT(setFudgeWhisper(double)));

    this->connect(this, SIGNAL(OtherEnabledSet(bool)), radio, SLOT(setEnabledOther(bool)));
    this->connect(this, SIGNAL(OtherLowFrequencySet(double)), radio, SLOT(setLowFrequencyOther(double)));
    this->connect(this, SIGNAL(OtherHighFrequencySet(double)), radio, SLOT(setHighFrequencyOther(double)));
    this->connect(this, SIGNAL(OtherDestructionSet(double)), radio, SLOT(setFudgeOther(double)));

    this->connect(this, SIGNAL(ToggleClientBlacklisted(uint64,anyID)), radio, SLOT(ToggleClientBlacklisted(uint64,anyID)));

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup(radio->objectName());

    cfg.beginGroup("HomeTab");
    radio->setEnabledHomeTab(cfg.value("enabled",false).toBool());
    radio->setLowFrequencyHomeTab(cfg.value("low_freq",300.0).toDouble());
    radio->setHighFrequencyHomeTab(cfg.value("high_freq",3000.0).toDouble());
    radio->setFudgeHomeTab(cfg.value("fudge",2.0).toDouble());
    cfg.endGroup();

    cfg.beginGroup("Whisper");
    radio->setEnabledWhisper(cfg.value("enabled",false).toBool());
    radio->setLowFrequencyWhisper(cfg.value("low_freq",300.0).toDouble());
    radio->setHighFrequencyWhisper(cfg.value("high_freq",3000.0).toDouble());
    radio->setFudgeWhisper(cfg.value("fudge",2.0).toDouble());
    cfg.endGroup();

    cfg.beginGroup("Other");
    radio->setEnabledOther(cfg.value("enabled",false).toBool());
    radio->setLowFrequencyOther(cfg.value("low_freq",300.0).toDouble());
    radio->setHighFrequencyOther(cfg.value("high_freq",3000.0).toDouble());
    radio->setFudgeOther(cfg.value("fudge",2.0).toDouble());
    cfg.endGroup();

    cfg.endGroup();

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
//    Q_UNUSED(serverConnectionHandlerID);
//    Q_UNUSED(selectedItemID);

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

                cfg.beginGroup("HomeTab");
                p_config->UpdateHomeEnabled(cfg.value("enabled",false).toBool());
                p_config->UpdateHomeLowFrequency(cfg.value("low_freq",300.0).toDouble());
                p_config->UpdateHomeHighFrequency(cfg.value("high_freq",3000.0).toDouble());
                p_config->UpdateHomeDestruction(cfg.value("fudge",2.0).toDouble());
                cfg.endGroup();

                cfg.beginGroup("Whisper");
                p_config->UpdateWhisperEnabled(cfg.value("enabled",false).toBool());
                p_config->UpdateWhisperLowFrequency(cfg.value("low_freq",300.0).toDouble());
                p_config->UpdateWhisperHighFrequency(cfg.value("high_freq",3000.0).toDouble());
                p_config->UpdateWhisperDestruction(cfg.value("fudge",2.0).toDouble());
                cfg.endGroup();

                cfg.beginGroup("Other");
                p_config->UpdateOtherEnabled(cfg.value("enabled",false).toBool());
                p_config->UpdateOtherLowFrequency(cfg.value("low_freq",300.0).toDouble());
                p_config->UpdateOtherHighFrequency(cfg.value("high_freq",3000.0).toDouble());
                p_config->UpdateOtherDestruction(cfg.value("fudge",2.0).toDouble());
                cfg.endGroup();

                cfg.endGroup();

                this->connect(p_config,SIGNAL(HomeEnabledSet(bool)),SIGNAL(HomeEnabledSet(bool)));
                this->connect(p_config,SIGNAL(HomeLowFrequencySet(double)),SIGNAL(HomeLowFrequencySet(double)));
                this->connect(p_config,SIGNAL(HomeHighFrequencySet(double)),SIGNAL(HomeHighFrequencySet(double)));
                this->connect(p_config,SIGNAL(HomeDestructionSet(double)),SIGNAL(HomeDestructionSet(double)));

                this->connect(p_config,SIGNAL(WhisperEnabledSet(bool)),SIGNAL(WhisperEnabledSet(bool)));
                this->connect(p_config,SIGNAL(WhisperLowFrequencySet(double)),SIGNAL(WhisperLowFrequencySet(double)));
                this->connect(p_config,SIGNAL(WhisperHighFrequencySet(double)),SIGNAL(WhisperHighFrequencySet(double)));
                this->connect(p_config,SIGNAL(WhisperDestructionSet(double)),SIGNAL(WhisperDestructionSet(double)));

                this->connect(p_config,SIGNAL(OtherEnabledSet(bool)),SIGNAL(OtherEnabledSet(bool)));
                this->connect(p_config,SIGNAL(OtherLowFrequencySet(double)),SIGNAL(OtherLowFrequencySet(double)));
                this->connect(p_config,SIGNAL(OtherHighFrequencySet(double)),SIGNAL(OtherHighFrequencySet(double)));
                this->connect(p_config,SIGNAL(OtherDestructionSet(double)),SIGNAL(OtherDestructionSet(double)));

                connect(p_config,SIGNAL(finished(int)),this,SLOT(saveSettings(int)));
                p_config->show();
                config = p_config;
            }
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

        cfg.beginGroup("HomeTab");
        cfg.setValue("enabled",radio->getEnabledHomeTab());
        cfg.setValue("low_freq",radio->getLowFrequencyHomeTab());
        cfg.setValue("high_freq",radio->getHighFrequencyHomeTab());
        cfg.setValue("fudge",radio->getFudgeHomeTab());
        cfg.endGroup();

        cfg.beginGroup("Whisper");
        cfg.setValue("enabled",radio->getEnabledWhisper());
        cfg.setValue("low_freq",radio->getLowFrequencyWhisper());
        cfg.setValue("high_freq",radio->getHighFrequencyWhisper());
        cfg.setValue("fudge",radio->getFudgeWhisper());
        cfg.endGroup();

        cfg.beginGroup("Other");
        cfg.setValue("enabled",radio->getEnabledOther());
        cfg.setValue("low_freq",radio->getLowFrequencyOther());
        cfg.setValue("high_freq",radio->getHighFrequencyOther());
        cfg.setValue("fudge",radio->getFudgeOther());
        cfg.endGroup();

        cfg.endGroup();

        emit settingsSave();
    }
}
