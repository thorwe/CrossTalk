#include "settings_duck.h"

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

SettingsDuck* SettingsDuck::m_Instance = 0;

SettingsDuck::SettingsDuck():
    m_ContextMenuUi(-1)
{
    this->setObjectName("SettingsDuck");
}
SettingsDuck::~SettingsDuck(){}


void SettingsDuck::Init(Ducker_Global* ducker_G, Ducker_Channel* ducker_C)
{
    if(m_ContextMenuUi == -1)
    {
        m_ContextMenuUi = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_GLOBAL,"Ducking","duck_16.png");
        connect(TSContextMenu::instance(),SIGNAL(MenusInitialized()),SLOT(onMenusInitialized()),Qt::AutoConnection);
        connect(TSContextMenu::instance(),SIGNAL(FireContextMenuEvent(uint64,PluginMenuType,int,uint64)),SLOT(onContextMenuEvent(uint64,PluginMenuType,int,uint64)),Qt::AutoConnection);
    }

    this->connect(this,SIGNAL(globalDuckerEnabledSet(bool)),ducker_G, SLOT(setEnabled(bool)));
    this->connect(this,SIGNAL(globalDuckerValueChanged(float)),ducker_G, SLOT(setValue(float)));

    this->connect(this,SIGNAL(channelDuckerEnabledSet(bool)),ducker_C, SLOT(setEnabled(bool)));
    this->connect(this,SIGNAL(channelDuckerValueChanged(float)),ducker_C,SLOT(setValue(float)));
    this->connect(this,SIGNAL(channelDuckerReverseSet(bool)), ducker_C, SLOT(setDuckingReverse(bool)));
    this->connect(this,SIGNAL(channelDuckerDuckPSEnabledSet(bool)), ducker_C, SLOT(setDuckPrioritySpeakers(bool)));

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);

    cfg.beginGroup("ducker_global");
    int size = cfg.beginReadArray("targets");
    for (int i = 0; i < size; ++i)
    {
        cfg.setArrayIndex(i);
        ducker_G->DuckTargets->insert(cfg.value("uid").toString(),cfg.value("name").toString());
    }
    cfg.endArray();
    ducker_G->setValue(cfg.value("value",-23.0f).toFloat());
    ducker_G->setEnabled(cfg.value("enabled",true).toBool());
    cfg.endGroup();

    ducker_C->setEnabled(cfg.value("ducking_enabled",true).toBool());
    ducker_C->setValue(cfg.value("ducking_value",-23.0f).toFloat());
    ducker_C->setDuckingReverse(cfg.value("ducking_reverse",false).toBool());
    ducker_C->setDuckPrioritySpeakers(cfg.value("ducking_PS",false).toBool());

    mP_ducker_G = ducker_G;
    mP_ducker_C = ducker_C;
}

void SettingsDuck::onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
    Q_UNUSED(serverConnectionHandlerID);
    Q_UNUSED(selectedItemID);

    if (type == PLUGIN_MENU_TYPE_GLOBAL)
    {
        if (menuItemID == m_ContextMenuUi)
        {
            if (config)
                config.data()->activateWindow();
            else
            {
                ConfigDucking* p_config = new ConfigDucking(TSHelpers::GetMainWindow());  //has delete on close attribute

                QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
                cfg.beginGroup("ducker_global");
                p_config->UpdateGlobalDuckerEnabled(cfg.value("enabled",true).toBool());
                p_config->UpdateGlobalDuckerValue(cfg.value("value",-23.0f).toFloat());
                cfg.endGroup();

                p_config->UpdateChannelDuckerEnabled(cfg.value("ducking_enabled",true).toBool());
                p_config->UpdateChannelDuckerValue(cfg.value("ducking_value",-23.0).toFloat());
                p_config->UpdateChannelDuckerReverse((cfg.value("ducking_reverse",false).toBool())?1:0);
                p_config->UpdateChannelDuckerDuckPSEnabled(cfg.value("ducking_PS",false).toBool());

                this->connect(p_config,SIGNAL(globalDuckerEnabledSet(bool)),SIGNAL(globalDuckerEnabledSet(bool)));
                this->connect(p_config,SIGNAL(globalDuckerValueChanged(float)),SIGNAL(globalDuckerValueChanged(float)));

                this->connect(p_config,SIGNAL(channelDuckerEnabledSet(bool)),SIGNAL(channelDuckerEnabledSet(bool)));
                this->connect(p_config,SIGNAL(channelDuckerValueChanged(float)),SIGNAL(channelDuckerValueChanged(float)));
                this->connect(p_config,SIGNAL(channelDuckerReverseSet(bool)),SIGNAL(channelDuckerReverseSet(bool)));
                this->connect(p_config,SIGNAL(channelDuckerDuckPSEnabledSet(bool)),SIGNAL(channelDuckerDuckPSEnabledSet(bool)));

                connect(p_config,SIGNAL(finished(int)),this,SLOT(saveSettings(int)));
                p_config->show();
                config = p_config;
            }
        }
    }
}

void SettingsDuck::onMenusInitialized()
{
    if(m_ContextMenuUi == -1)
        TSLogging::Error(QString("%1: Menu wasn't registered.").arg(this->objectName()));
}

void SettingsDuck::saveSettings(int r)
{
    Q_UNUSED(r);
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    if(mP_ducker_G)
    {
        cfg.beginGroup("ducker_global");
        cfg.setValue("enabled",mP_ducker_G.data()->isEnabled());
        cfg.setValue("value",mP_ducker_G.data()->getValue());
        cfg.endGroup();
    }
    if(mP_ducker_C)
    {
        cfg.setValue("ducking_enabled", mP_ducker_C.data()->isEnabled());
        cfg.setValue("ducking_value", mP_ducker_C.data()->getValue());
        cfg.setValue("ducking_reverse", mP_ducker_C.data()->isTargetOtherTabs());
        cfg.setValue("ducking_PS", mP_ducker_C.data()->isDuckPrioritySpeakers());
    }

    emit settingsSave();
}
