/*
 * TeamSpeak 3 Multi Server Tools plugin
 * Author: Thorsten Weinz (philosound@gmail.com)
 */

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
//#include <Windows.h>
//#include <TlHelp32.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"
#include "plugin.h"

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"
#include "translator.h"
#include "talkers.h"
#include "config.h"
#include "snt.h"
#include "mod_ducker_channel.h"
#include "mod_ducker_global.h"
#include "mod_muter_channel.h"
#include "mod_position_spread.h"
//#include "mod_positionalaudio.h"
#include "ts_ptt_qt.h"

struct TS3Functions ts3Functions;

Translator* loca = Translator::instance();
Talkers* talkers = Talkers::instance();

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); dest[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 19

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128
#define REQUESTCLIENTMOVERETURNCODES_SLOTS 5

#define PLUGIN_THREAD_TIMEOUT 1000

#define TIMER_MSEC 10000

/* Array for request client move return codes. See comments within ts3plugin_processCommand for details */
static char requestClientMoveReturnCodes[REQUESTCLIENTMOVERETURNCODES_SLOTS][RETURNCODE_BUFSIZE];

QMutex command_mutex;
// Plugin values
char* pluginID = NULL;

SnT snt;
PositionSpread positionSpread;
Ducker_Global ducker_G;
Ducker_Channel ducker_C;
ChannelMuter channel_Muter;
//PositionalAudio positionalAudio;

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
    return "CrossTalk";
}

/* Plugin version */
const char* ts3plugin_version() {
    return "1.2.1";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() {
	return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
    return "Thorsten Weinz";
}

/* Plugin description */
const char* ts3plugin_description() {
    return "Features:\nStereo Position Spread,\nDucking\nSwitch\'n\'Talk (Cross Server Tab PTT)\n\nFor information on the modules, use the \'?\' button on the topper right of the plugin settings window or visit the Github wiki by clicking on the banner.\n";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
    ts3Functions = funcs;
}

/*
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on failure.
 * If the function returns 1 on failure, the plugin will be unloaded again.
 */
int ts3plugin_init() {
    TSLogging::Log("init");

    if (CONSOLE_OUTPUT)
        freopen("CONOUT$", "wb", stdout);   //Makes printf work in Release mode (shouldn't been necessary, but is...)

	/* Initialize return codes array for requestClientMove */
	memset(requestClientMoveReturnCodes, 0, REQUESTCLIENTMOVERETURNCODES_SLOTS * RETURNCODE_BUFSIZE);

    TSPtt::instance()->Init(&command_mutex);

    loca->InitLocalization();

//    positionalAudio.setEnabled(true);

    channel_Muter.setEnabled(true);

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    ducker_C.setEnabled(cfg.value("ducking_enabled",true).toBool());
    ducker_C.setValue(cfg.value("ducking_value",-23.0f).toFloat());
    ducker_C.setDuckingReverse(cfg.value("ducking_reverse",false).toBool());
    positionSpread.setEnabled(cfg.value("stereo_position_spread_enabled",true).toBool());
    positionSpread.setSpreadWidth(cfg.value("stereo_position_spread_value",0.5f).toFloat());
    positionSpread.setExpertModeEnabled(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
    positionSpread.setRegionHomeTab(cfg.value("stereo_position_spread_region_home",1).toInt());
    positionSpread.setRegionWhisper(cfg.value("stereo_position_spread_region_whisper",2).toInt());
    positionSpread.setRegionOther(cfg.value("stereo_position_spread_region_other",0).toInt());

    cfg.beginGroup("ducker_global");
    int size = cfg.beginReadArray("targets");
    for (int i = 0; i < size; ++i)
    {
        cfg.setArrayIndex(i);
        ducker_G.DuckTargets->insert(cfg.value("uid").toString(),cfg.value("name").toString());
    }
    cfg.endArray();
    ducker_G.setValue(cfg.value("value",-23.0f).toFloat());
    ducker_G.setEnabled(cfg.value("enabled",true).toBool());
    cfg.endGroup();


    // Support enabling the plugin while already connected
    uint64* servers;
    if(ts3Functions.getServerConnectionHandlerList(&servers) == ERROR_ok)
    {
        uint64* server;
        for(server = servers; *server != (uint64)NULL; ++server)
        {
            int status;
            if (ts3Functions.getConnectionStatus(*server, &status) != ERROR_ok)
                continue;

            if (status > STATUS_DISCONNECTED)
            {
                // Not used by now
                //uint64 channelID;
                for (int j=STATUS_CONNECTING; j<=status; ++j)
                {
                    ts3plugin_onConnectStatusChangeEvent(*server, j, ERROR_ok);
                    /*if (j == STATUS_CONNECTED)
                    {
                        // Get our channel and publish it as default channel
                        anyID myID;
                        if(ts3Functions.getClientID(*server,&myID) == ERROR_ok)
                        {
                            if(ts3Functions.getChannelOfClient(*server,myID,&channelID) == ERROR_ok)
                            {
                                uint64 channelParentID;
                                if (ts3Functions.getParentChannelOfChannel(*server,channelID,&channelParentID) == ERROR_ok)
                                    ts3plugin_onNewChannelEvent(*server, channelID, channelParentID);
                            }
                        }
                    }
                    else if (j == STATUS_CONNECTION_ESTABLISHING)
                    {
                        // publish all other channels
                        uint64* channelList;
                        if (ts3Functions.getChannelList(*server,&channelList) == ERROR_ok)
                        {
                            for (int k=0;channelList[k] != NULL;++k)
                            {
                                if (channelList[k] == channelID)
                                    continue;

                                uint64 channelParentID;
                                if (ts3Functions.getParentChannelOfChannel(*server,channelList[k],&channelParentID) == ERROR_ok)
                                    ts3plugin_onNewChannelEvent(*server, channelList[k], channelParentID); // ToDo: Hope it's not necessary to sort those
                            }
                            ts3Functions.freeMemory(channelList);
                        }
                    }*/
                }
                if (status == STATUS_CONNECTION_ESTABLISHED)
                {
                    ts3plugin_currentServerConnectionChanged(*server);
                    unsigned int error;
                    if ((error = talkers->RefreshTalkers(*server)) != ERROR_ok)
                        TSLogging::Error("Error refreshing talkers: ",*server,error);
                }
            }
        }
        ts3Functions.freeMemory(servers);

        // Get the active server tab
        uint64 scHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        if (scHandlerID != 0)
            ts3plugin_currentServerConnectionChanged(scHandlerID);
    }

    TSLogging::Log("init done");
    return 0;  /* 0 = success, 1 = failure */
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {

	/*
	 * Note:
	 * If your plugin implements a settings dialog, it must be closed and deleted here, else the
	 * TeamSpeak client will most likely crash (DLL removed but dialog from DLL code still open).
	 */

//    if (positionalAudio.isRunning())
//        positionalAudio.setBlocked(true);

	/* Free pluginID if we registered it */
	if(pluginID) {
		free(pluginID);
		pluginID = NULL;
	}
}

/****************************** Optional functions ********************************/
/*
 * Following functions are optional, if not needed you don't need to implement them.
 */

/* Tell client if plugin offers a configuration window. If this function is not implemented, it's an assumed "does not offer" (PLUGIN_OFFERS_NO_CONFIGURE). */
int ts3plugin_offersConfigure() {
	/*
	 * Return values:
	 * PLUGIN_OFFERS_NO_CONFIGURE         - Plugin does not implement ts3plugin_configure
	 * PLUGIN_OFFERS_CONFIGURE_NEW_THREAD - Plugin does implement ts3plugin_configure and requests to run this function in an own thread
	 * PLUGIN_OFFERS_CONFIGURE_QT_THREAD  - Plugin does implement ts3plugin_configure and requests to run this function in the Qt GUI thread
	 */
	return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;  /* In this case ts3plugin_configure does not need to be implemented */
}

/* Plugin might offer a configuration window. If ts3plugin_offersConfigure returns 0, this function does not need to be implemented. */
void ts3plugin_configure(void* handle, void* qParentWidget) {
    Config* qParentWidget_p = (Config*)qParentWidget;

    qParentWidget_p = new Config();
    qParentWidget_p->SetupUi();
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetGlobalDuckerEnabled(bool)),&ducker_G,SLOT(setEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetGlobalDuckerValue(float)),&ducker_G,SLOT(setValue(float)));

    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetDuckingEnabled(bool)),&ducker_C,SLOT(setEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetDuckingValue(float)),&ducker_C,SLOT(setValue(float)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetDuckingReverse(bool)),&ducker_C,SLOT(setDuckingReverse(bool)));

    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadEnabled(bool)), &positionSpread,SLOT(setEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadValue(float)), &positionSpread,SLOT(setSpreadWidth(float)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertEnabled(bool)), &positionSpread,SLOT(setExpertModeEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertValueHome(int)),&positionSpread,SLOT(setRegionHomeTab(int)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertValueWhisper(int)),&positionSpread, SLOT(setRegionWhisper(int)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertValueOther(int)),&positionSpread, SLOT(setRegionOther(int)));
    qParentWidget_p->exec();
}

/*
 * If the plugin wants to use error return codes or plugin commands, it needs to register a command ID. This function will be automatically
 * called after the plugin was initialized. This function is optional. If you don't use error return codes or plugin commands, the function
 * can be omitted.
 * Note the passed pluginID parameter is no longer valid after calling this function, so you must copy it and store it in the plugin.
 */
void ts3plugin_registerPluginID(const char* id) {
	const size_t sz = strlen(id) + 1;
	pluginID = (char*)malloc(sz * sizeof(char));
	_strcpy(pluginID, sz, id);  /* The id buffer will invalidate after exiting this function */
}

/* Plugin command keyword. Return NULL or "" if not used. */
const char* ts3plugin_commandKeyword() {
    return "ct";
}

/* Plugin processes console command. Return 0 if plugin handled the command, 1 if not handled. */
int ts3plugin_processCommand(uint64 serverConnectionHandlerID, const char* command) {
    if (!(command_mutex.tryLock(PLUGIN_THREAD_TIMEOUT)))
    {
        TSLogging::Log("Timeout while waiting for mutex",serverConnectionHandlerID,LogLevel_WARNING);
        return 1;
    }

    int ret = 0;

    QString cmd_qs;
    cmd_qs = command;
    QStringList args_qs = cmd_qs.split(" ",QString::SkipEmptyParts);
    if (args_qs.isEmpty())  // this might evolve to some help output
        return 1;

    cmd_qs = args_qs.takeFirst();

    // Dispatch
    ret = positionSpread.ParseCommand(serverConnectionHandlerID,cmd_qs,args_qs);
    if (ret != 0)
    {
        snt.ParseCommand(serverConnectionHandlerID,cmd_qs,args_qs);
        ret = 1;    // ToDo: Suffer through snt.ParseCommand to return an int
    }

    command_mutex.unlock();
    return ret;
}

/*
 * Plugin requests to be always automatically loaded by the TeamSpeak 3 client unless
 * the user manually disabled it in the plugin dialog.
 * This function is optional. If missing, no autoload is assumed.
 */
int ts3plugin_requestAutoload() {
	return 1;  /* 1 = request autoloaded, 0 = do not request autoload */
}

/* Helper function to create a menu item */
static struct PluginMenuItem* createMenuItem(enum PluginMenuType type, int id, const char* text, const char* icon) {
    struct PluginMenuItem* menuItem = (struct PluginMenuItem*)malloc(sizeof(struct PluginMenuItem));
    menuItem->type = type;
    menuItem->id = id;
    _strcpy(menuItem->text, PLUGIN_MENU_BUFSZ, text);
    _strcpy(menuItem->icon, PLUGIN_MENU_BUFSZ, icon);
    return menuItem;
}

/* Some makros to make the code to create menu items a bit more readable */
#define BEGIN_CREATE_MENUS(x) const size_t sz = x + 1; size_t n = 0; *menuItems = (struct PluginMenuItem**)malloc(sizeof(struct PluginMenuItem*) * sz);
#define CREATE_MENU_ITEM(a, b, c, d) (*menuItems)[n++] = createMenuItem(a, b, c, d);
#define END_CREATE_MENUS (*menuItems)[n++] = NULL; assert(n == sz);

/*
 * Menu IDs for this plugin. Pass these IDs when creating a menuitem to the TS3 client. When the menu item is triggered,
 * ts3plugin_onMenuItemEvent will be called passing the menu ID of the triggered menu item.
 * These IDs are freely choosable by the plugin author. It's not really needed to use an enum, it just looks prettier.
 */
enum {
    MENU_ID_CLIENT_1 = 1,
    MENU_ID_CLIENT_2,
    MENU_ID_CHANNEL_1,
//	MENU_ID_CHANNEL_2,
//	MENU_ID_CHANNEL_3,
    MENU_ID_GLOBAL_1,
    MENU_ID_GLOBAL_2
};

/*
 * Initialize plugin menus.
 * This function is called after ts3plugin_init and ts3plugin_registerPluginID. A pluginID is required for plugin menus to work.
 * Both ts3plugin_registerPluginID and ts3plugin_freeMemory must be implemented to use menus.
 * If plugin menus are not used by a plugin, do not implement this function or return NULL.
 */
void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
    /*
     * Create the menus
     * There are three types of menu items:
     * - PLUGIN_MENU_TYPE_CLIENT:  Client context menu
     * - PLUGIN_MENU_TYPE_CHANNEL: Channel context menu
     * - PLUGIN_MENU_TYPE_GLOBAL:  "Plugins" menu in menu bar of main window
     *
     * Menu IDs are used to identify the menu item when ts3plugin_onMenuItemEvent is called
     *
     * The menu text is required, max length is 128 characters
     *
     * The icon is optional, max length is 128 characters. When not using icons, just pass an empty string.
     * Icons are loaded from a subdirectory in the TeamSpeak client plugins folder. The subdirectory must be named like the
     * plugin filename, without dll/so/dylib suffix
     * e.g. for "test_plugin.dll", icon "1.png" is loaded from <TeamSpeak 3 Client install dir>\plugins\test_plugin\1.png
     */
    BEGIN_CREATE_MENUS(3);  /* IMPORTANT: Number of menu items must be correct! */
    QString src;
    QString trans;
    src = "Toggle Global Ducking Target";
    trans = loca->translator->translate("context_menu", src.toLocal8Bit().constData());
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CLIENT,  MENU_ID_CLIENT_1,  (trans.isEmpty()?src:trans).toLocal8Bit().constData(),  "");
    src = "Toggle ChannelMuter Whitelisting [temp]";
    trans = loca->translator->translate("context_menu", src.toLocal8Bit().constData());
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CLIENT,  MENU_ID_CLIENT_2,  (trans.isEmpty()?src:trans).toLocal8Bit().constData(),  "");
    src = "Toggle Channel Mute [temp]";
    trans = loca->translator->translate("context_menu", src.toLocal8Bit().constData());
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CHANNEL, MENU_ID_CHANNEL_1, (trans.isEmpty()?src:trans).toLocal8Bit().constData(),  "");
//	CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CHANNEL, MENU_ID_CHANNEL_2, "Channel item 2", "2.png");
//	CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CHANNEL, MENU_ID_CHANNEL_3, "Channel item 3", "3.png");
//    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL,  MENU_ID_GLOBAL_1,  "Global item 1",  "");
//    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL,  MENU_ID_GLOBAL_2,  "Global item 2",  "");
    END_CREATE_MENUS;  /* Includes an assert checking if the number of menu items matched */

    /*
     * Specify an optional icon for the plugin. This icon is used for the plugins submenu within context and main menus
     * If unused, set menuIcon to NULL
     */
//    *menuIcon = (char*)malloc(PLUGIN_MENU_BUFSZ * sizeof(char));
//    _strcpy(*menuIcon, PLUGIN_MENU_BUFSZ, "t.png");

    /*
     * Menus can be enabled or disabled with: ts3Functions.setPluginMenuEnabled(pluginID, menuID, 0|1);
     * Test it with plugin command: /test enablemenu <menuID> <0|1>
     * Menus are enabled by default. Please note that shown menus will not automatically enable or disable when calling this function to
     * ensure Qt menus are not modified by any thread other the UI thread. The enabled or disable state will change the next time a
     * menu is displayed.
     */
    /* For example, this would disable MENU_ID_GLOBAL_2: */
    /* ts3Functions.setPluginMenuEnabled(pluginID, MENU_ID_GLOBAL_2, 0); */

    /* All memory allocated in this function will be automatically released by the TeamSpeak client later by calling ts3plugin_freeMemory */
}

/*
* Implement the following three functions when the plugin should display a line in the server,channel,client info.
* If any of ts3plugin_infoTitle, ts3plugin_infoData or ts3plugin_freeMemory is missing, the info text will not be displayed.
*/

/* Static title shown in the left column in the info frame */
const char* ts3plugin_infoTitle() {
    return ts3plugin_name();
}

/*
 * Dynamic content shown in the right column in the info frame. Memory for the data string needs to be allocated in this
 * function. The client will call ts3plugin_freeMemory once done with the string to release the allocated memory again.
 * Check the parameter "type" if you want to implement this feature only for specific item types. Set the parameter
 * "data" to NULL to have the client ignore the info data.
 */
static PluginItemType m_InfoType = PLUGIN_SERVER;
static uint64 m_InfoId = 0;
void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data) {
    m_InfoType = type; // Save type when user manually changes the selected Item
    m_InfoId = id; // Save currently selected Item
    if (type == PLUGIN_CLIENT)
    {
        unsigned int error;
        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            TSLogging::Error("(ts3plugin_infoData) Error getting my client id",serverConnectionHandlerID,error);
            return;
        }

        ts3Functions.setPluginMenuEnabled(pluginID,MENU_ID_CLIENT_1,((anyID)id != myID)?1:0);
        ts3Functions.setPluginMenuEnabled(pluginID,MENU_ID_CLIENT_2,((anyID)id != myID)?1:0);

        QString outstr = "";
        if ((anyID)id == myID)
        {
//            if (positionalAudio.isRunning())
//            {
//                QString game = positionalAudio.getMyGame();
//                if (!game.isEmpty())
//                {
//                    outstr.append(QString("is playing %1").arg(game));
//                    QString id = positionalAudio.getMyIdentity();
//                    if (!id.isEmpty())
//                        outstr.append(QString(" as %1").arg(id));

//                    outstr.append(".\n");
//                    TSLogging::Print(outstr);
//                }
//            }
        }
        else
        {
            if (ducker_G.isClientMusicBot(serverConnectionHandlerID,(anyID)id))
                outstr.append("Global Ducking Target (MusicBot)\n");

            if (channel_Muter.isClientWhitelisted(serverConnectionHandlerID,(anyID)id))
                outstr.append("ChannelMuter whitelisted [temp]\n");
        }

        if (outstr != "")
        {
            *data = (char*)malloc(INFODATA_BUFSIZE * sizeof(char));
            _strcpy(*data, INFODATA_BUFSIZE, outstr.toLocal8Bit().data());
        }
        else
            data = NULL;
    }
    else if ((type == PLUGIN_CHANNEL) && channel_Muter.isChannelMuted(serverConnectionHandlerID,id))
    {
        *data = (char*)malloc(INFODATA_BUFSIZE * sizeof(char));
        _strcpy(*data, INFODATA_BUFSIZE, QString("Channel Muted [temp]\n").toLocal8Bit().data());
    }
    else
        data = NULL;
}

/* Required to release the memory for parameter "data" allocated in ts3plugin_infoData */
void ts3plugin_freeMemory(void* data) {
    free(data);
}

/* Helper function to create a hotkey */
static struct PluginHotkey* createHotkey(const char* keyword, const char* description) {
	struct PluginHotkey* hotkey = (struct PluginHotkey*)malloc(sizeof(struct PluginHotkey));
	_strcpy(hotkey->keyword, PLUGIN_HOTKEY_BUFSZ, keyword);
	_strcpy(hotkey->description, PLUGIN_HOTKEY_BUFSZ, description);
	return hotkey;
}

/* Some makros to make the code to create hotkeys a bit more readable */
#define BEGIN_CREATE_HOTKEYS(x) const size_t sz = x + 1; size_t n = 0; *hotkeys = (struct PluginHotkey**)malloc(sizeof(struct PluginHotkey*) * sz);
#define CREATE_HOTKEY(a, b) (*hotkeys)[n++] = createHotkey(a, b);
#define END_CREATE_HOTKEYS (*hotkeys)[n++] = NULL; assert(n == sz);

/*
 * Initialize plugin hotkeys. If your plugin does not use this feature, this function can be omitted.
 * Hotkeys require ts3plugin_registerPluginID and ts3plugin_freeMemory to be implemented.
 * This function is automatically called by the client after ts3plugin_init.
 */
void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys) {
	/* Register hotkeys giving a keyword and a description.
	 * The keyword will be later passed to ts3plugin_onHotkeyEvent to identify which hotkey was triggered.
	 * The description is shown in the clients hotkey dialog. */
    BEGIN_CREATE_HOTKEYS(3);  /* Create 2 hotkeys. Size must be correct for allocating memory. */
	CREATE_HOTKEY("TS3_NEXT_TAB_AND_TALK_START", "Next Tab and Talk Start");
    CREATE_HOTKEY("TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START", "Next Tab and Whisper all Channel Commanders Start");
    CREATE_HOTKEY("TS3_SWITCH_N_TALK_END", "SnT Stop");
	END_CREATE_HOTKEYS;
	/* The client will call ts3plugin_freeMemory to release all allocated memory */
}

/* Show an error message if the plugin failed to load */
void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
//    TSLogging::Print(QString("ts3plugin_onConnectStatusChangeEvent: %1").arg(newStatus),serverConnectionHandlerID,LogLevel_DEBUG);
    talkers->onConnectStatusChangeEvent(serverConnectionHandlerID,newStatus,errorNumber);
    if (newStatus==STATUS_CONNECTION_ESTABLISHED)
    {
        ts3plugin_currentServerConnectionChanged(serverConnectionHandlerID);
        // Fake client move of myself

        unsigned int error;
        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
            TSLogging::Error("(ts3plugin_onConnectStatusChangeEvent) Error getting my clientID");
        {
            // Get My channel on this handler
            uint64 channelID;
            if((error=ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
                TSLogging::Error("(ts3plugin_onConnectStatusChangeEvent) Error getting my clients channel id",serverConnectionHandlerID,error);
            else
                ts3plugin_onClientMoveEvent(serverConnectionHandlerID, myID, 0, channelID, ENTER_VISIBILITY, "");
        }
    }
}

void ts3plugin_onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID)
{
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage)
{
    Q_UNUSED(moveMessage);

    unsigned int error;
    if (newChannelID == 0)  // When we disconnect, we get moved to chan 0 before the connection event
    {                       // However, we aren't able to get our own id etc. anymore via the API for comparison
        int con_status;     // Therefor, unless we cache our ids for no other benefit, this is a workaround by filtering those out
        if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID,&con_status)) != ERROR_ok)
        {
            TSLogging::Error("(ts3plugin_onClientMoveEvent)",serverConnectionHandlerID,error);
            return;
        }
        if (con_status == STATUS_DISCONNECTED)
            return;
    }
    else if (TSHelpers::IsClientQuery(serverConnectionHandlerID,clientID))
        return;

    // Get My Id on this handler
    anyID myID;
    if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
    {
        TSLogging::Error("(ts3plugin_onClientMoveEvent)",serverConnectionHandlerID,error);
        return;
    }

    channel_Muter.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    ducker_G.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    ducker_C.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
//    positionalAudio.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage)
{
    Q_UNUSED(timeoutMessage);

    unsigned int error;
    if (newChannelID == 0)  // When we disconnect, we get moved to chan 0 before the connection event
    {                       // However, we aren't able to get our own id etc. anymore via the API for comparison
        int con_status;     // Therefor, unless we cache our ids for no other benefit, this is a workaround by filtering those out
        if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID,&con_status)) != ERROR_ok)
        {
            TSLogging::Error("(ts3plugin_onClientMoveTimeoutEvent)",serverConnectionHandlerID,error);
            return;
        }
        if (con_status == STATUS_DISCONNECTED)
            return;
    }
    else if (TSHelpers::IsClientQuery(serverConnectionHandlerID,clientID))
        return;

    // Get My Id on this handler
    anyID myID;
    if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
    {
        TSLogging::Error("(ts3plugin_onClientMoveTimeoutEvent)",serverConnectionHandlerID,error);
        return;
    }

    channel_Muter.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    ducker_G.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    ducker_C.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
//    positionalAudio.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage)
{
    Q_UNUSED(moverID);
    Q_UNUSED(moverName);
    Q_UNUSED(moverUniqueIdentifier);
    Q_UNUSED(moveMessage);

    unsigned int error;
    if (newChannelID == 0)  // When we disconnect, we get moved to chan 0 before the connection event
    {                       // However, we aren't able to get our own id etc. anymore via the API for comparison
        int con_status;     // Therefor, unless we cache our ids for no other benefit, this is a workaround by filtering those out
        if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID,&con_status)) != ERROR_ok)
        {
            TSLogging::Error("(ts3plugin_onClientMoveMovedEvent)",serverConnectionHandlerID,error);
            return;
        }
        if (con_status == STATUS_DISCONNECTED)
            return;
    }
    else if (TSHelpers::IsClientQuery(serverConnectionHandlerID,clientID))
        return;

    // Get My Id on this handler; it's used anywhere anyways so get it now
    anyID myID;
    if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
    {
        TSLogging::Error("(ts3plugin_onClientMoveMovedEvent)",serverConnectionHandlerID,error);
        return;
    }

    channel_Muter.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    ducker_G.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    ducker_C.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
//    positionalAudio.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
    bool isMe = talkers->onTalkStatusChangeEvent(serverConnectionHandlerID,status,isReceivedWhisper,clientID);

    if (channel_Muter.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe))
        return; //Client is muted;

    ducker_G.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
    if (!ducker_G.isRunning() || !ducker_G.isClientMusicBotRt(serverConnectionHandlerID,clientID))
    {
        ducker_C.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
        positionSpread.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
    }
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels)
{
    if (channel_Muter.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels))
        return; //Client is muted;

    if (!ducker_G.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels))
        ducker_C.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels);
}

void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
    positionSpread.onEditPostProcessVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels,channelSpeakerArray,channelFillMask);
}

void ts3plugin_onEditMixedPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
}

/* Clientlib rare */
void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue)
{
     snt.onClientSelfVariableUpdateEvent(serverConnectionHandlerID,flag,oldValue,newValue);

    // seems to be equal to a ts3plugin_currentServerConnectionChanged when initiated by the user
    // except that this version will fire when returning from a cross server ptt event
    // (ts3plugin_currentServerConnectionChanged is not even fired when ptt-switching for whatever reason)
    // Furtunately, we can make use of that difference.
}

/* Client changed current server connection handler */
void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID)
{
//    TSLogging::Print(QString("ts3plugin_currentServerConnectionChanged"),serverConnectionHandlerID,LogLevel_DEBUG);
    // event will fire twice on connecting to a new tab; first before connecting, second after established by our manual trigger
    unsigned int error;
    int status;
    if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID,&status)) != ERROR_ok)
    {
        TSLogging::Error("ts3plugin_currentServerConnectionChanged",serverConnectionHandlerID,error);
        return;
    }
    if (status != STATUS_CONNECTION_ESTABLISHED)
    {
//        TSLogging::Print("ts3plugin_currentServerConnectionChanged: filtering early tab change event",serverConnectionHandlerID,LogLevel_DEBUG);
        return;
    }
    ducker_C.setHomeId(serverConnectionHandlerID);
    positionSpread.setHomeId(serverConnectionHandlerID);
}

/* Client UI callbacks */

/*
 * Called when a plugin menu item (see ts3plugin_initMenus) is triggered. Optional function, when not using plugin menus, do not implement this.
 *
 * Parameters:
 * - serverConnectionHandlerID: ID of the current server tab
 * - type: Type of the menu (PLUGIN_MENU_TYPE_CHANNEL, PLUGIN_MENU_TYPE_CLIENT or PLUGIN_MENU_TYPE_GLOBAL)
 * - menuItemID: Id used when creating the menu item
 * - selectedItemID: Channel or Client ID in the case of PLUGIN_MENU_TYPE_CHANNEL and PLUGIN_MENU_TYPE_CLIENT. 0 for PLUGIN_MENU_TYPE_GLOBAL.
 */
void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID) {
//    printf("PLUGIN: onMenuItemEvent: serverConnectionHandlerID=%llu, type=%d, menuItemID=%d, selectedItemID=%llu\n", (long long unsigned int)serverConnectionHandlerID, type, menuItemID, (long long unsigned int)selectedItemID);
    switch(type) {
        case PLUGIN_MENU_TYPE_GLOBAL:
            /* Global menu item was triggered. selectedItemID is unused and set to zero. */
            switch(menuItemID) {
                case MENU_ID_GLOBAL_1:
                    /* Menu global 1 was triggered */
                    break;
                case MENU_ID_GLOBAL_2:
                    /* Menu global 2 was triggered */
                    break;
                default:
                    break;
            }
            break;
        case PLUGIN_MENU_TYPE_CHANNEL:
            /* Channel contextmenu item was triggered. selectedItemID is the channelID of the selected channel */
            switch(menuItemID) {
                case MENU_ID_CHANNEL_1:
                    /* Menu channel 1 was triggered */
                    channel_Muter.toggleChannelMute(serverConnectionHandlerID,selectedItemID);
                    break;
//				case MENU_ID_CHANNEL_2:
//					/* Menu channel 2 was triggered */
//					break;
//				case MENU_ID_CHANNEL_3:
//					/* Menu channel 3 was triggered */
//					break;
                default:
                    break;
            }
            break;
        case PLUGIN_MENU_TYPE_CLIENT:
            /* Client contextmenu item was triggered. selectedItemID is the clientID of the selected client */
            switch(menuItemID) {
                case MENU_ID_CLIENT_1:
                    ducker_G.ToggleMusicBot(serverConnectionHandlerID,(anyID)selectedItemID);
                    break;
                case MENU_ID_CLIENT_2:
                    /* Menu client 2 was triggered */
                    channel_Muter.toggleClientWhitelisted(serverConnectionHandlerID,(anyID)selectedItemID);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    if (m_InfoType > 0 && m_InfoId > 0) ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
}

void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char* pluginName, const char* pluginCommand)
{
    if (QString(pluginName) != ts3plugin_name())
        return;

    anyID myID;
    unsigned int error;
    if ((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
    {
        TSLogging::Error("(ts3plugin_onPluginCommandEvent)",serverConnectionHandlerID,error);
        return;
    }

    QString cmd_qs(pluginCommand);
    QTextStream args_qs(&cmd_qs);

    anyID clientID;
    args_qs >> clientID;

    QString cmd;
    args_qs >> cmd;

    // note: args_qs leading white space if streaming to QByteArray or using readAll()
//    if (!positionalAudio.onPluginCommand(serverConnectionHandlerID, clientID, (clientID == myID) , cmd, args_qs))
//    {
//        TSLogging::Error("Error on plugin command",serverConnectionHandlerID,NULL,true);
//    }
}

/* This function is called if a plugin hotkey was pressed. Omit if hotkeys are unused. */
void ts3plugin_onHotkeyEvent(const char* keyword) {
    /* Identify the hotkey by keyword ("keyword_1", "keyword_2" or "keyword_3" in this example) and handle here... */
    ts3plugin_processCommand((uint64)NULL,keyword);
}
