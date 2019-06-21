#include "groupbox_positionalaudio_servers.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "core/ts_helpers_qt.h"
#include "core/ts_serversinfo.h"
#include "core/ts_logging_qt.h"

GroupBoxPositionalAudioServers::GroupBoxPositionalAudioServers(QWidget *parent) :
    QGroupBox(parent),
    m_servers(new QListWidget),
    m_settings(new QGroupBox(tr("Enabled"))),
    m_send_interval(new QDoubleSpinBox),
    m_send_interval_silent_inc(new QDoubleSpinBox),
    m_unblock(new QPushButton(tr("Unblock"))),
    m_remove(new QPushButton(QIcon(":/icons/delete.png"), QString::null))       //"gfx/default/16x16_delete.png"
{
    setTitle(tr("Server Settings"));
    
    auto layout = new QGridLayout;

    m_servers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_servers->setResizeMode(QListView::Adjust);
    connect(m_servers, &QListWidget::currentTextChanged, this, &GroupBoxPositionalAudioServers::changePage);
    layout->addWidget(m_servers, 0, 0, 1, 2);

    auto add_button = new QPushButton(QIcon(":/icons/add.png"), QString::null); //"gfx/default/16x16_add.png"
    add_button->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    add_button->setToolTip(tr("Add current server tab as custom server setting"));
    connect(add_button, &QPushButton::clicked, this, &GroupBoxPositionalAudioServers::addButtonClicked);
    layout->addWidget(add_button, 1, 0);

    m_remove->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_remove->setToolTip(tr("Remove the selected rows custom server setting"));
    connect(m_remove, &QPushButton::clicked, this, &GroupBoxPositionalAudioServers::onRemoveButtonClicked);
    layout->addWidget(m_remove, 1, 1);

    // settings
    {
        m_settings->setCheckable(true);
        m_settings->setChecked(false);

        auto settings_layout = new QVBoxLayout;
        connect(m_settings, &QGroupBox::toggled, this, &GroupBoxPositionalAudioServers::onEnabledCheckToggled);

        settings_layout->addWidget(new QLabel(tr("Send Interval")));
        connect(m_send_interval, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &GroupBoxPositionalAudioServers::onSendIntervalSpinBoxValueChanged);
        settings_layout->addWidget(m_send_interval);

        settings_layout->addWidget(new QLabel(tr("Silent +")));
        connect(m_send_interval_silent_inc, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &GroupBoxPositionalAudioServers::onSendIntervalSilentIncSpinBoxValueChanged);
        settings_layout->addWidget(m_send_interval_silent_inc);

        auto sp_retain = m_unblock->sizePolicy();
        sp_retain.setRetainSizeWhenHidden(true);
        m_unblock->setSizePolicy(sp_retain);

        connect(m_unblock, &QPushButton::clicked, this, &GroupBoxPositionalAudioServers::onUnblockButtonClicked);
        settings_layout->addWidget(m_unblock);

        m_settings->setLayout(settings_layout);
        layout->addWidget(m_settings, 0, 2, 2, 1);
    }

    setLayout(layout);
}

void GroupBoxPositionalAudioServers::UpdateUIServerAdd(QString serverUniqueId, QString serverName)
{
    if (m_ServerSettingsMap.contains(serverUniqueId))
    {
        if (m_ServerSettingsMap.value(serverUniqueId).serverName != serverName)
            m_ServerSettingsMap[serverUniqueId].serverName = serverName;

        return;
    }
    PositionalAudio_ServerSettings setting;
    setting.serverName = serverName;
    setting.serverUniqueId = serverUniqueId;
    setting.enabled = false;
    setting.isBlocked = true;
    setting.sendInterval = 1.0f;
    setting.sendIntervalSilentInc = 1.0f;
    m_ServerSettingsMap.insert(serverUniqueId, setting);

    m_Name2sUId.insert(serverName, serverUniqueId);

    m_servers->addItem(serverName);
}

void GroupBoxPositionalAudioServers::UpdateUIServerRemove(QString serverUniqueId, QString serverName)
{
    m_ServerSettingsMap.remove(serverUniqueId);
    m_Name2sUId.remove(serverName);
    auto list = m_servers->findItems(serverName, Qt::MatchExactly);
    if (list.size() == 1)
    {
        const auto kRow = m_servers->row(list.at(0));
        delete m_servers->takeItem(kRow);
    }
    else
    {
        // shouldn't happen, however...
        for (int i = 0; i < list.size(); ++i)
        {
            const auto kRow = m_servers->row(list.at(i));
            delete m_servers->takeItem(kRow);
        }
    }
}

void GroupBoxPositionalAudioServers::UpdateUIServerEnabled(QString serverUniqueId, bool val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;

    if (m_ServerSettingsMap.value(serverUniqueId).enabled == val)
        return;

    m_ServerSettingsMap[serverUniqueId].enabled = val;

    auto selected_items = m_servers->selectedItems();
    if (selected_items.isEmpty())
        return;

    const auto kRowName = selected_items.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == kRowName)
        m_settings->setChecked(val);
}

void GroupBoxPositionalAudioServers::UpdateUIServerBlocked(QString serverUniqueId, bool val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;

    if (m_ServerSettingsMap.value(serverUniqueId).isBlocked == val)
        return;

    m_ServerSettingsMap[serverUniqueId].isBlocked = val;

    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == kRowName)
        m_unblock->setVisible(val);
}

void GroupBoxPositionalAudioServers::UpdateUIServerSendInterval(QString serverUniqueId, float val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;

    if (m_ServerSettingsMap.value(serverUniqueId).sendInterval == val)
        return;

    m_ServerSettingsMap[serverUniqueId].sendInterval = val;

    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == kRowName)
    {
//        m_send_interval->blockSignals(true);
        m_send_interval->setValue(val);
//        m_send_interval->blockSignals(false);
    }
}

void GroupBoxPositionalAudioServers::UpdateUIServerSendIntervalSilentInc(QString serverUniqueId, float val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;

    if (m_ServerSettingsMap.value(serverUniqueId).sendIntervalSilentInc == val)
        return;

    m_ServerSettingsMap[serverUniqueId].sendIntervalSilentInc = val;

    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == kRowName)
    {
//        m_send_interval_silent_inc->blockSignals(true);
        m_send_interval_silent_inc->setValue(val);
//        m_send_interval_silent_inc->blockSignals(false);
    }
}

void GroupBoxPositionalAudioServers::UpdateUIServerSelect(QString serverUniqueId)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        serverUniqueId = "default";

    const auto kServerName = m_ServerSettingsMap.value(serverUniqueId).serverName;

    auto list = m_servers->findItems(kServerName, Qt::MatchExactly);
    if (list.size() == 1)
    {
        const auto kRow = m_servers->row(list.at(0));
        m_servers->setCurrentRow(kRow);
    }
}

//void GroupBoxPositionalAudioServers::UpdateServerSettings(QMap<QString, PositionalAudio_ServerSettings> map)
//{
//    m_ServerSettingsMap = map;
//    m_servers->addItems(QStringList(map.keys()));
//    m_servers->setCurrentRow(0);
//}

void GroupBoxPositionalAudioServers::changePage(QString val)
{
    const auto kServerUid = m_Name2sUId.value(val);
    m_settings->blockSignals(true);
    m_settings->setChecked(m_ServerSettingsMap.value(kServerUid).enabled);
    m_settings->blockSignals(false);
    m_send_interval->blockSignals(true);
    m_send_interval->setValue(m_ServerSettingsMap.value(kServerUid).sendInterval);
    m_send_interval->blockSignals(false);
    m_send_interval_silent_inc->blockSignals(true);
    m_send_interval_silent_inc->setValue(m_ServerSettingsMap.value(kServerUid).sendIntervalSilentInc);
    m_send_interval_silent_inc->blockSignals(false);
    m_unblock->blockSignals(true);
    m_unblock->setVisible(m_ServerSettingsMap.value(kServerUid).isBlocked);
    m_unblock->blockSignals(false);
    m_remove->blockSignals(true);
    m_remove->setEnabled(kServerUid != "default");
    m_remove->blockSignals(false);
}

void GroupBoxPositionalAudioServers::onRemoveButtonClicked()
{
    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    const auto kServerUid = m_Name2sUId.value(kRowName);
    emit removeButtonClicked(kServerUid);
}

void GroupBoxPositionalAudioServers::onSendIntervalSpinBoxValueChanged(double val)
{
    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    const auto kServerUid = m_Name2sUId.value(kRowName);
    m_ServerSettingsMap[kServerUid].sendInterval = static_cast<float>(val);
    emit sendIntervalChange(kServerUid, static_cast<float>(val));
}

void GroupBoxPositionalAudioServers::onSendIntervalSilentIncSpinBoxValueChanged(double val)
{
    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    const auto kServerUid = m_Name2sUId.value(kRowName);
    m_ServerSettingsMap[kServerUid].sendIntervalSilentInc = static_cast<float>(val);
    emit sendIntervalSilentIncChange(kServerUid, static_cast<float>(val));
}

void GroupBoxPositionalAudioServers::onEnabledCheckToggled(bool val)
{
    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    const auto kServerUid = m_Name2sUId.value(kRowName);
    m_ServerSettingsMap[kServerUid].enabled = val;
    emit enabledSet(kServerUid, val);
}

void GroupBoxPositionalAudioServers::onUnblockButtonClicked()
{
    auto selectedItems = m_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    const auto kRowName = selectedItems.at(0)->text();
    const auto kServerUid = m_Name2sUId.value(kRowName);
    m_ServerSettingsMap[kServerUid].isBlocked = false;
    m_unblock->setVisible(false);
    emit blockedSet(kServerUid, false);
}
