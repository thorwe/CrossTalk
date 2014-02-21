#ifndef SETTINGS_POSITION_SPREAD_H
#define SETTINGS_POSITION_SPREAD_H

#include <QObject>
#include <QDialog>
#include "ts_context_menu_qt.h"
#include "mod_position_spread.h"

class SettingsPositionSpread : public QObject, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(ContextMenuInterface)

public:
    static SettingsPositionSpread* instance()
    {
        static QMutex mutex;
        if(!m_Instance)
        {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new SettingsPositionSpread;

            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

    void Init(PositionSpread *positionSpread);

signals:
    void EnabledSet(bool);
    void ValueChanged(float);

    void ExpertModeEnabledSet(bool);
    void RegionHomeTabSet(int);
    void RegionWhisperSet(int);
    void RegionOtherSet(int);

    void settingsSave();
    
public slots:
    void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID);
    void onMenusInitialized();

private slots:
    void saveSettings(int r);

private:
    explicit SettingsPositionSpread();
    ~SettingsPositionSpread();
    static SettingsPositionSpread* m_Instance;
    SettingsPositionSpread(const SettingsPositionSpread &);
    SettingsPositionSpread& operator=(const SettingsPositionSpread &);

    int m_ContextMenuUi;
    QPointer<QDialog> config;

    QPointer<PositionSpread> mP_positionSpread;

};

#endif // SETTINGS_POSITION_SPREAD_H
