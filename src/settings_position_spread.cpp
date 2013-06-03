#include "settings_position_spread.h"

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

SettingsPositionSpread* SettingsPositionSpread::m_Instance = 0;

SettingsPositionSpread::SettingsPositionSpread():
    m_ContextMenuUi(-1)
{
    this->setObjectName("SettingsPositionSpread");
}
SettingsPositionSpread::~SettingsPositionSpread(){}

void SettingsPositionSpread::Init(PositionSpread *positionSpread)
{
    if(m_ContextMenuUi == -1)
    {
        m_ContextMenuUi = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_GLOBAL,"Position Spread","radar_16.png");
        connect(TSContextMenu::instance(),SIGNAL(MenusInitialized()),SLOT(onMenusInitialized()),Qt::AutoConnection);
        connect(TSContextMenu::instance(),SIGNAL(FireContextMenuEvent(uint64,PluginMenuType,int,uint64)),SLOT(onContextMenuEvent(uint64,PluginMenuType,int,uint64)),Qt::AutoConnection);
    }

    this->connect(this,SIGNAL(EnabledSet(bool)),positionSpread, SLOT(setEnabled(bool)));
    this->connect(this,SIGNAL(ValueChanged(float)),positionSpread, SLOT(setSpreadWidth(float)));
    this->connect(this,SIGNAL(ExpertModeEnabledSet(bool)),positionSpread, SLOT(setExpertModeEnabled(bool)));
    this->connect(this,SIGNAL(RegionHomeTabSet(int)),positionSpread,SLOT(setRegionHomeTab(int)));
    this->connect(this,SIGNAL(RegionWhisperSet(int)),positionSpread,SLOT(setRegionWhisper(int)));
    this->connect(this,SIGNAL(RegionOtherSet(int)),positionSpread,SLOT(setRegionOther(int)));

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);

    positionSpread->setEnabled(cfg.value("stereo_position_spread_enabled",true).toBool());
    positionSpread->setSpreadWidth(cfg.value("stereo_position_spread_value",0.5f).toFloat());
    positionSpread->setExpertModeEnabled(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
    positionSpread->setRegionHomeTab(cfg.value("stereo_position_spread_region_home",1).toInt());
    positionSpread->setRegionWhisper(cfg.value("stereo_position_spread_region_whisper",2).toInt());
    positionSpread->setRegionOther(cfg.value("stereo_position_spread_region_other",0).toInt());

    mP_positionSpread = positionSpread;
}

void SettingsPositionSpread::onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID)
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
                ConfigPositionSpread* p_config = new ConfigPositionSpread(TSHelpers::GetMainWindow());  //has delete on close attribute

                QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
                p_config->UpdateEnabledSet(cfg.value("stereo_position_spread_enabled",true).toBool());
                p_config->UpdateSpreadWidth(cfg.value("stereo_position_spread_value",0.5f).toFloat());
                p_config->UpdateExpertEnabledSet(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
                p_config->UpdateExpertHomeValueChanged(cfg.value("stereo_position_spread_region_home",1).toInt());
                p_config->UpdateExpertWhisperValueChanged(cfg.value("stereo_position_spread_region_whisper",2).toInt());
                p_config->UpdateExpertOtherValueChanged(cfg.value("stereo_position_spread_region_other",0).toInt());

                this->connect(p_config,SIGNAL(EnabledSet(bool)),SIGNAL(EnabledSet(bool)));
                this->connect(p_config,SIGNAL(ValueChanged(float)),SIGNAL(ValueChanged(float)));

                this->connect(p_config,SIGNAL(ExpertEnabledSet(bool)),SIGNAL(ExpertModeEnabledSet(bool)));
                this->connect(p_config,SIGNAL(ExpertHomeValueChanged(int)),SIGNAL(RegionHomeTabSet(int)));
                this->connect(p_config,SIGNAL(ExpertWhisperValueChanged(int)),SIGNAL(RegionWhisperSet(int)));
                this->connect(p_config,SIGNAL(ExpertOtherValueChanged(int)),SIGNAL(RegionOtherSet(int)));

                connect(p_config,SIGNAL(finished(int)),this,SLOT(saveSettings(int)));
                p_config->show();
                config = p_config;
            }
        }
    }
}

void SettingsPositionSpread::onMenusInitialized()
{
    if(m_ContextMenuUi == -1)
        TSLogging::Error(QString("%1: Menu wasn't registered.").arg(this->objectName()));
}

void SettingsPositionSpread::saveSettings(int r)
{
    Q_UNUSED(r);
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("stereo_position_spread_enabled",mP_positionSpread.data()->isEnabled());
    cfg.setValue("stereo_position_spread_value",mP_positionSpread.data()->getSpreadWidth());

    cfg.setValue("stereo_position_spread_expert_enabled",mP_positionSpread.data()->isExpertModeEnabled());
    cfg.setValue("stereo_position_spread_region_home",(int)(mP_positionSpread.data()->getRegionHomeTab()));
    cfg.setValue("stereo_position_spread_region_whisper",(int)(mP_positionSpread.data()->getRegionWhisper()));
    cfg.setValue("stereo_position_spread_region_other",(int)(mP_positionSpread.data()->getRegionOther()));
}


