#pragma once

#include <QObject>
#include <QtCore>
#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"

class ContextMenuInterface
{
public:
    virtual void onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID) = 0;
};
Q_DECLARE_INTERFACE(ContextMenuInterface,"net.thorwe.CrossTalk.ContextMenuInterface/1.0")

class TSContextMenu : public QObject
{
    Q_OBJECT
public:
    static TSContextMenu* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSContextMenu;

            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop() {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

    bool setMainIcon(QString icon);
    int Register(QObject *p, PluginMenuType type, QString text, QString icon);

    void onInitMenus(struct PluginMenuItem ***menuItems, char **menuIcon);
    void onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID);

signals:
    void MenusInitialized();
    void FireContextMenuEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID);

public slots:
    
private:
    //singleton
    explicit TSContextMenu();
    ~TSContextMenu() = default;
    static TSContextMenu* m_Instance;
    TSContextMenu(const TSContextMenu &);
    TSContextMenu& operator=(const TSContextMenu &);

    QList<PluginMenuItem*> m_Items;
    QVector<QPointer<QObject> > m_Callbacks;
    QString m_MainIcon;
    bool m_isInit = false;
};
