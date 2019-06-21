#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include "core/ts_context_menu_qt.h"
#include "mod_position_spread.h"

class SettingsPositionSpread : public QObject, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(ContextMenuInterface)

public:
    explicit SettingsPositionSpread(QObject* parent = nullptr);

    void Init(PositionSpread *positionSpread);
    void shutdown();

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
    int m_ContextMenuUi = -1;
    QPointer<QDialog> config;

    QPointer<PositionSpread> mP_positionSpread;
};
