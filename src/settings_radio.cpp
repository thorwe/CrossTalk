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
    this->connect(this, SIGNAL(LowFrequencySet(QString,double)), radio, SLOT(setLowFrequency(QString,double)));
    this->connect(this, SIGNAL(HighFrequencySet(QString,double)), radio, SLOT(setHighFrequency(QString,double)));
    this->connect(this, SIGNAL(DestructionSet(QString,double)), radio, SLOT(setFudge(QString,double)));
    this->connect(this, SIGNAL(RingModFrequencySet(QString,double)), radio, SLOT(setRingModFrequency(QString,double)));

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
        radio->setLowFrequency(name,cfg.value("low_freq",300.0).toDouble());
        radio->setHighFrequency(name,cfg.value("high_freq",3000.0).toDouble());
        radio->setFudge(name,cfg.value("fudge",2.0).toDouble());
        cfg.beginGroup("RingMod");
        radio->setRingModFrequency(name,cfg.value("mod_freq",0.0f).toDouble());
        cfg.endGroup();
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
                    p_config->UpdateBandpassLowFrequency(name,cfg.value("low_freq",300.0).toDouble());
                    p_config->UpdateBandpassHighFrequency(name,cfg.value("high_freq",3000.0).toDouble());
                    p_config->UpdateDestruction(name,cfg.value("fudge",2.0).toDouble());
                    cfg.beginGroup("RingMod");
                    p_config->UpdateRingModFrequency(name,cfg.value("mod_freq",0.0f).toDouble());
                    cfg.endGroup();
                    cfg.endGroup();
                }

                this->connect(p_config,SIGNAL(EnabledSet(QString,bool)),SIGNAL(EnabledSet(QString,bool)));
                this->connect(p_config,SIGNAL(LowFrequencySet(QString,double)),SIGNAL(LowFrequencySet(QString,double)));
                this->connect(p_config,SIGNAL(HighFrequencySet(QString,double)),SIGNAL(HighFrequencySet(QString,double)));
                this->connect(p_config,SIGNAL(DestructionSet(QString,double)),SIGNAL(DestructionSet(QString,double)));
                this->connect(p_config,SIGNAL(RingModFrequencySet(QString,double)),SIGNAL(RingModFrequencySet(QString,double)));

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

                //TSLogging::Print(QString("Saving %1").arg(name));
                cfg.beginGroup(name);
                RadioFX_Settings settings = i.value();
                cfg.setValue("enabled",settings.enabled);
                cfg.setValue("low_freq",settings.freq_low);
                cfg.setValue("high_freq",settings.freq_hi);
                cfg.setValue("fudge",settings.fudge);
                cfg.beginGroup("RingMod");
                cfg.setValue("mod_freq",settings.rm_freq);
                cfg.endGroup();
                cfg.endGroup();
                //TSLogging::Print(QString("enabled %1 low_freq %2 hi_freq %3 fudge %4 mod_freq %5").arg(settings.enabled).arg(settings.freq_low).arg(settings.freq_hi).arg(settings.fudge).arg(settings.rm_freq));
            }
        }
        cfg.endGroup();

        emit settingsSave();
    }
}
