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

    this->connect(this, SIGNAL(EnabledSet(QString,bool)), radio, SLOT(setChannelStripEnabled(QString,bool)));
    this->connect(this, SIGNAL(InLoFreqSet(QString,double)), radio, SLOT(setInLoFreq(QString,double)));
    this->connect(this, SIGNAL(InHiFreqSet(QString,double)), radio, SLOT(setInHiFreq(QString,double)));
    this->connect(this, SIGNAL(DestructionSet(QString,double)), radio, SLOT(setFudge(QString,double)));
    this->connect(this, SIGNAL(RingModFrequencySet(QString,double)), radio, SLOT(setRingModFrequency(QString,double)));
    this->connect(this, SIGNAL(RingModMixSet(QString,double)), radio, SLOT(setRingModMix(QString,double)));
    this->connect(this, SIGNAL(OutLoFreqSet(QString,double)), radio, SLOT(setOutLoFreq(QString,double)));
    this->connect(this, SIGNAL(OutHiFreqSet(QString,double)), radio, SLOT(setOutHiFreq(QString,double)));

    this->connect(this, SIGNAL(ToggleClientBlacklisted(uint64,anyID)), radio, SLOT(ToggleClientBlacklisted(uint64,anyID)));

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup(radio->objectName());

    QStringList stringList;
    stringList << "HomeTab" << "Whisper" << "Other";
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

                this->connect(p_config,SIGNAL(EnabledSet(QString,bool)),SIGNAL(EnabledSet(QString,bool)));
                this->connect(p_config,SIGNAL(InLoFreqSet(QString,double)),SIGNAL(InLoFreqSet(QString,double)));
                this->connect(p_config,SIGNAL(InHiFreqSet(QString,double)),SIGNAL(InHiFreqSet(QString,double)));
                this->connect(p_config,SIGNAL(DestructionSet(QString,double)),SIGNAL(DestructionSet(QString,double)));
                this->connect(p_config,SIGNAL(RingModFrequencySet(QString,double)),SIGNAL(RingModFrequencySet(QString,double)));
                this->connect(p_config,SIGNAL(RingModMixSet(QString,double)),SIGNAL(RingModMixSet(QString,double)));
                this->connect(p_config,SIGNAL(OutLoFreqSet(QString,double)),SIGNAL(OutLoFreqSet(QString,double)));
                this->connect(p_config,SIGNAL(OutHiFreqSet(QString,double)),SIGNAL(OutHiFreqSet(QString,double)));

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
