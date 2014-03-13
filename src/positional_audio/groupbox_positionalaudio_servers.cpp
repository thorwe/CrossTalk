#include "groupbox_positionalaudio_servers.h"
#include "ui_groupbox_positionalaudio_servers.h"
#include "ts_helpers_qt.h"
#include "ts_serversinfo.h"
#include "ts_logging_qt.h"
#include <QMessageBox>

GroupBoxPositionalAudioServers::GroupBoxPositionalAudioServers(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::GroupBoxPositionalAudioServers)
{
    ui->setupUi(this);

    QIcon addIcon(":/icons/add.png");    //"gfx/default/16x16_add.png"
    ui->pushButton_add->setIcon(addIcon);
    QIcon removeIcon(":/icons/delete.png");     //"gfx/default/16x16_delete.png"
    ui->pushButton_remove->setIcon(removeIcon);

    connect(ui->listWidget_servers, SIGNAL(currentTextChanged(QString)), this, SLOT(changePage(QString)));
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SIGNAL(addButtonClicked()));
    connect(ui->pushButton_remove,SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));

    connect(ui->groupBoxSettings, SIGNAL(toggled(bool)),this,SLOT(onEnabledCheckToggled(bool)));
    connect(ui->doubleSpinBox_SendInterval,SIGNAL(valueChanged(double)), this, SLOT(onSendIntervalSpinBoxValueChanged(double)));
    connect(ui->doubleSpinBox_SendIntervalSilentInc,SIGNAL(valueChanged(double)), this, SLOT(onSendIntervalSilentIncSpinBoxValueChanged(double)));
    connect(ui->pushButton_unblock, SIGNAL(clicked()), this, SLOT(onUnblockButtonClicked()));
}

GroupBoxPositionalAudioServers::~GroupBoxPositionalAudioServers()
{
    delete ui;
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
    m_ServerSettingsMap.insert(serverUniqueId,setting);

    m_Name2sUId.insert(serverName,serverUniqueId);

    ui->listWidget_servers->addItem(serverName);
}

void GroupBoxPositionalAudioServers::UpdateUIServerRemove(QString serverUniqueId, QString serverName)
{
    m_ServerSettingsMap.remove(serverUniqueId);
    m_Name2sUId.remove(serverName);
    QList<QListWidgetItem *> list = ui->listWidget_servers->findItems(serverName,Qt::MatchExactly);
    if (list.size() == 1)
    {
        int row = ui->listWidget_servers->row(list.at(0));
        delete ui->listWidget_servers->takeItem(row);
    }
    else
    {
        // shouldn't happen, however...
        for (int i = 0; i < list.size(); ++i)
        {
            int row = ui->listWidget_servers->row(list.at(i));
            delete ui->listWidget_servers->takeItem(row);
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

    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == rowName)
    {
        ui->groupBoxSettings->setChecked(val);
    }
}

void GroupBoxPositionalAudioServers::UpdateUIServerBlocked(QString serverUniqueId, bool val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;
    if (m_ServerSettingsMap.value(serverUniqueId).isBlocked == val)
        return;
    m_ServerSettingsMap[serverUniqueId].isBlocked = val;

    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == rowName)
        ui->pushButton_unblock->setVisible(val);
}

void GroupBoxPositionalAudioServers::UpdateUIServerSendInterval(QString serverUniqueId, float val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;
    if (m_ServerSettingsMap.value(serverUniqueId).sendInterval == val)
        return;
    m_ServerSettingsMap[serverUniqueId].sendInterval = val;

    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == rowName)
    {
//        ui->doubleSpinBox_SendInterval->blockSignals(true);
        ui->doubleSpinBox_SendInterval->setValue(val);
//        ui->doubleSpinBox_SendInterval->blockSignals(false);
    }
}

void GroupBoxPositionalAudioServers::UpdateUIServerSendIntervalSilentInc(QString serverUniqueId, float val)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        return;
    if (m_ServerSettingsMap.value(serverUniqueId).sendIntervalSilentInc == val)
        return;
    m_ServerSettingsMap[serverUniqueId].sendIntervalSilentInc = val;

    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    if (m_ServerSettingsMap.value(serverUniqueId).serverName == rowName)
    {
//        ui->doubleSpinBox_SendInterval->blockSignals(true);
        ui->doubleSpinBox_SendIntervalSilentInc->setValue(val);
//        ui->doubleSpinBox_SendInterval->blockSignals(false);
    }
}

void GroupBoxPositionalAudioServers::UpdateUIServerSelect(QString serverUniqueId)
{
    if (!m_ServerSettingsMap.contains(serverUniqueId))
        serverUniqueId = "default";

    QString serverName = m_ServerSettingsMap.value(serverUniqueId).serverName;

    QList<QListWidgetItem *> list = ui->listWidget_servers->findItems(serverName,Qt::MatchExactly);
    if (list.size() == 1)
    {
        int row = ui->listWidget_servers->row(list.at(0));
        ui->listWidget_servers->setCurrentRow(row);
    }
}

//void GroupBoxPositionalAudioServers::UpdateServerSettings(QMap<QString, PositionalAudio_ServerSettings> map)
//{
//    m_ServerSettingsMap = map;
//    ui->listWidget_servers->addItems(QStringList(map.keys()));
//    ui->listWidget_servers->setCurrentRow(0);
//}

void GroupBoxPositionalAudioServers::changePage(QString val)
{
    QString serverUniqueId = m_Name2sUId.value(val);
    ui->groupBoxSettings->blockSignals(true);
    ui->groupBoxSettings->setChecked(m_ServerSettingsMap.value(serverUniqueId).enabled);
    ui->groupBoxSettings->blockSignals(false);
    ui->doubleSpinBox_SendInterval->blockSignals(true);
    ui->doubleSpinBox_SendInterval->setValue(m_ServerSettingsMap.value(serverUniqueId).sendInterval);
    ui->doubleSpinBox_SendInterval->blockSignals(false);
    ui->doubleSpinBox_SendIntervalSilentInc->blockSignals(true);
    ui->doubleSpinBox_SendIntervalSilentInc->setValue(m_ServerSettingsMap.value(serverUniqueId).sendIntervalSilentInc);
    ui->doubleSpinBox_SendIntervalSilentInc->blockSignals(false);
    ui->pushButton_unblock->blockSignals(true);
    ui->pushButton_unblock->setVisible(m_ServerSettingsMap.value(serverUniqueId).isBlocked);
    ui->pushButton_unblock->blockSignals(false);
    ui->pushButton_remove->blockSignals(true);
    ui->pushButton_remove->setEnabled(serverUniqueId != "default");
    ui->pushButton_remove->blockSignals(false);
}

void GroupBoxPositionalAudioServers::onRemoveButtonClicked()
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    QString serverUniqueId = m_Name2sUId.value(rowName);
    emit removeButtonClicked(serverUniqueId);
}

void GroupBoxPositionalAudioServers::onSendIntervalSpinBoxValueChanged(double val)
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    QString serverUniqueId = m_Name2sUId.value(rowName);
    m_ServerSettingsMap[serverUniqueId].sendInterval = (float)val;
    emit sendIntervalChange(serverUniqueId,(float)val);
}

void GroupBoxPositionalAudioServers::onSendIntervalSilentIncSpinBoxValueChanged(double val)
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    QString serverUniqueId = m_Name2sUId.value(rowName);
    m_ServerSettingsMap[serverUniqueId].sendIntervalSilentInc = (float)val;
    emit sendIntervalSilentIncChange(serverUniqueId,(float)val);
}

void GroupBoxPositionalAudioServers::onEnabledCheckToggled(bool val)
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    QString serverUniqueId = m_Name2sUId.value(rowName);
    m_ServerSettingsMap[serverUniqueId].enabled = val;
    emit enabledSet(serverUniqueId,val);
}

void GroupBoxPositionalAudioServers::onUnblockButtonClicked()
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_servers->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QString rowName = selectedItems.at(0)->text();
    QString serverUniqueId = m_Name2sUId.value(rowName);
    m_ServerSettingsMap[serverUniqueId].isBlocked = false;
    ui->pushButton_unblock->setVisible(false);
    emit blockedSet(serverUniqueId,false);
}
