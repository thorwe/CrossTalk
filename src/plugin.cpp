/*
 * TeamSpeak 3 Multi Server Tools plugin
 * Author: Thorsten Weinz (philosound@gmail.com)
 */

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
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

#include "ts_context_menu_qt.h"

#include "ts_serversinfo.h"

#include "talkers.h"
#include "config.h"
#include "snt.h"
#include "mod_ducker_channel.h"
#include "mod_ducker_global.h"
#include "mod_muter_channel.h"
#include "mod_position_spread.h"

#include "settings_duck.h"
#include "settings_position_spread.h"

#ifdef USE_POSITIONAL_AUDIO
#include "settings_positionalaudio.h"
#include "mod_positionalaudio.h"
#endif

#include "ts_ptt_qt.h"

#include "updater.h"

#ifdef USE_RADIO
#include "settings_radio.h"
#include "mod_radio.h"
#endif

struct TS3Functions ts3Functions;

Translator* loca = Translator::instance();
Talkers* talkers = Talkers::instance();
TSContextMenu* contextMenu = TSContextMenu::instance();
TSInfoData* infoData = TSInfoData::instance();

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); dest[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 19

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
//#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128
#define REQUESTCLIENTMOVERETURNCODES_SLOTS 5

#define PLUGIN_THREAD_TIMEOUT 1000

#define TIMER_MSEC 10000

QMutex command_mutex;
// Plugin values
char* pluginID = NULL;

Updater updater;
SnT snt;
PositionSpread positionSpread;
Ducker_Global ducker_G;
Ducker_Channel ducker_C;
ChannelMuter channel_Muter;
#ifdef USE_POSITIONAL_AUDIO
SettingsPositionalAudio* settingsPositionalAudio = SettingsPositionalAudio::instance();
PositionalAudio positionalAudio;
#endif
#ifdef USE_RADIO
SettingsRadio* settingsRadio = SettingsRadio::instance();
Radio radio;
#endif

SettingsDuck* settingsDuck = SettingsDuck::instance();
SettingsPositionSpread* settingsPositionSpread = SettingsPositionSpread::instance();

TSServersInfo* centralStation = TSServersInfo::instance();

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
    return "1.4.070101";
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
    return "Features:\n\nPositional Audio\nRadio FX\nStereo Position Spread\nDucking\nSwitch\'n\'Talk (Cross Server Tab PTT)\n\nFor information on the modules, use the \'?\' button on the topper right of the plugin settings window or visit the Github wiki by clicking on the banner.\n";
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

    TSPtt::instance()->Init(&command_mutex);

    loca->InitLocalization();

    contextMenu->setMainIcon("ct_16x16.png");

    settingsDuck->Init(&ducker_G,&ducker_C);
    settingsPositionSpread->Init(&positionSpread);

#ifdef USE_RADIO
    settingsRadio->Init(&radio);
#endif

#ifdef USE_POSITIONAL_AUDIO
    settingsPositionalAudio->Init(&positionalAudio);
#endif

    channel_Muter.setEnabled(true);

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

    updater.CheckUpdate();

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

//    #ifdef USE_POSITIONAL_AUDIO
//    if (positionalAudio.isRunning())
//        positionalAudio.setBlocked(true);
//    #endif

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
    return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;
}

/* Plugin might offer a configuration window. If ts3plugin_offersConfigure returns 0, this function does not need to be implemented. */
void ts3plugin_configure(void* handle, void* qParentWidget) {
    Q_UNUSED(handle);
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

    bool inside = (cmd_qs.startsWith("\"")); //true if the first character is "
    QStringList tmpList = cmd_qs.split(QRegExp("\""), QString::SkipEmptyParts); // Split by " and make sure you don't have an empty string at the beginning
    QStringList args_qs;
    foreach (QString s, tmpList)
    {
        if (inside) // If 's' is inside quotes ...
            args_qs.append(s); // ... get the whole string
        else // If 's' is outside quotes ...
            args_qs.append(s.split(" ", QString::SkipEmptyParts)); // ... get the splitted string

        inside = !inside;
    }

//    QStringList args_qs = cmd_qs.split(" ",QString::SkipEmptyParts);
    if (args_qs.isEmpty())  // this might evolve to some help output
        return 1;

    cmd_qs = args_qs.takeFirst();

    if (cmd_qs == "FLOOD_TEST")
    {
#ifdef USE_POSITIONAL_AUDIO
        ts3plugin_onServerErrorEvent(serverConnectionHandlerID,"CrossTalk Flood Test", ERROR_client_is_flooding,"CrossTalk Flood Test Return Code", "CrossTalk Flood Test Extra Message");
#endif
    }
    else if (cmd_qs == "TOGGLE_SELF_SERVER_GROUP")
    {
        if (args_qs.size() < 2)
            ret = 1;
        else
        {
            unsigned int error;
            uint64 targetServer = 0;
            QString serverName = args_qs.takeFirst();

            if ((error = TSHelpers::GetServerHandler(serverName,&targetServer)) != ERROR_ok)
                ret = 0;
            else
            {
                QSet<uint64> myServerGroups;
                if ((error = TSHelpers::GetClientSelfServerGroups(targetServer, &myServerGroups)) != ERROR_ok)
                {
                    TSLogging::Error("(TOGGLE_SELF_SERVER_GROUP) Could not get self server groups", targetServer, error, true);
                    ret = 1;
                }
                else
                {
                    QString serverGroupName = args_qs.takeFirst();
                    uint64 serverGroupId = centralStation->GetServerInfo(targetServer)->GetServerGroupId(serverGroupName);
                    if (serverGroupId == (uint64)NULL)
                        ret = 1;
                    else
                    {
                        // Get My Id on this handler
                        anyID myID;
                        if((error = ts3Functions.getClientID(targetServer,&myID)) != ERROR_ok)
                        {
                            TSLogging::Error("(TOGGLE_SELF_SERVER_GROUP)",serverConnectionHandlerID,error);
                            ret = 1;
                        }
                        else
                        {
                            // Doesn't work with ClientSelf
                            int myDbId;
                            if ((error = ts3Functions.getClientVariableAsInt(targetServer,myID,CLIENT_DATABASE_ID,&myDbId)) != ERROR_ok)
                            {
                                TSLogging::Error("(TOGGLE_SELF_SERVER_GROUP) Could not get self client db id", targetServer, error, true);
                                ret = 1;
                            }
                            else
                            {
                                if (myServerGroups.contains(serverGroupId))
                                    ts3Functions.requestServerGroupDelClient(targetServer,serverGroupId,myDbId,NULL);
                                else
                                    ts3Functions.requestServerGroupAddClient(targetServer,serverGroupId,myDbId,NULL);

                                ret = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    else if ((cmd_qs == "SET_SELF_CHANNEL_GROUP") || (cmd_qs == "SET_SELF_CHANNEL_GROUP_INHERITED"))
    {
        if (args_qs.size() < 2)
            ret = 1;
        else
        {
            unsigned int error;
            uint64 targetServer = 0;
            QString serverName = args_qs.takeFirst();

            if ((error = TSHelpers::GetServerHandler(serverName,&targetServer)) != ERROR_ok)
                ret = 0;
            else
            {
                anyID myID;
                if((error = ts3Functions.getClientID(targetServer,&myID)) != ERROR_ok)
                {
                    TSLogging::Error("(SET_SELF_CHANNEL_GROUP)",targetServer,error,true);
                    ret = 0;
                }
                else
                {
                    // Doesn't work with ClientSelf
                    int myDbId;
                    if ((error = ts3Functions.getClientVariableAsInt(targetServer,myID,CLIENT_DATABASE_ID,&myDbId)) != ERROR_ok)
                    {
                        TSLogging::Error("(SET_SELF_CHANNEL_GROUP) Could not get self client db id", targetServer, error, true);
                        ret = 1;
                    }
                    else
                    {
                        int myChannelGroupId;
                        if ((error = ts3Functions.getClientVariableAsInt(targetServer, myID, CLIENT_CHANNEL_GROUP_ID, &myChannelGroupId)) != ERROR_ok)
                        {
                            TSLogging::Error("(SET_SELF_CHANNEL_GROUP)",targetServer,error,true);
                            ret = 0;
                        }
                        else
                        {
                            QString channelGroupName = args_qs.takeFirst();
                            uint64 channelGroupId = centralStation->GetServerInfo(targetServer)->GetChannelGroupId(channelGroupName);
                            if ((channelGroupId == (uint64)NULL) || (channelGroupId == myChannelGroupId))
                                ret = 1;
                            else
                            {
                                uint64 targetChannelId;
                                if (cmd_qs == "SET_SELF_CHANNEL_GROUP")
                                {
                                    if ((error = ts3Functions.getChannelOfClient(targetServer,myID,&targetChannelId)) != ERROR_ok)
                                    {
                                        TSLogging::Error("(SET_SELF_CHANNEL_GROUP)",targetServer,error,true);
                                        ret = 0;
                                    }
                                }
                                else if (cmd_qs == "SET_SELF_CHANNEL_GROUP_INHERITED")
                                {
                                    int inheritingChannelId;
                                    if ((error = ts3Functions.getClientVariableAsInt(targetServer, myID, CLIENT_CHANNEL_GROUP_INHERITED_CHANNEL_ID, &inheritingChannelId)) != ERROR_ok)
                                    {
                                        TSLogging::Error("(SET_SELF_CHANNEL_GROUP)",targetServer,error,true);
                                        ret = 0;
                                    }
                                    else
                                        targetChannelId = (uint64)inheritingChannelId;
                                }
                                if (error == ERROR_ok)
                                {
                                    const int SIZE = 1;
                                    uint64 channelGroupIdArray[SIZE] = {channelGroupId};
                                    uint64 channelIdArray[SIZE] = {targetChannelId};
                                    uint64 clientDbIdArray[SIZE] = {myDbId};
//                                    TSLogging::Print(QString("SET_SELF_CHANNEL_GROUP: %1 %2 %3").arg(channelGroupIdArray[0]).arg(channelIdArray[0]).arg(clientDbIdArray[0]));
                                    ts3Functions.requestSetClientChannelGroup(targetServer,&channelGroupIdArray[0],&channelIdArray[0],&clientDbIdArray[0],SIZE,NULL);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Dispatch
        ret = channel_Muter.ParseCommand(serverConnectionHandlerID,cmd_qs,args_qs);
        if (ret != 0)
        {
            ret = positionSpread.ParseCommand(serverConnectionHandlerID,cmd_qs,args_qs);
            if (ret != 0)
            {
                snt.ParseCommand(serverConnectionHandlerID,cmd_qs,args_qs);
                ret = 1;    // ToDo: Suffer through snt.ParseCommand to return an int
            }
        }
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

/*
 * Initialize plugin menus.
 * This function is called after ts3plugin_init and ts3plugin_registerPluginID. A pluginID is required for plugin menus to work.
 * Both ts3plugin_registerPluginID and ts3plugin_freeMemory must be implemented to use menus.
 * If plugin menus are not used by a plugin, do not implement this function or return NULL.
 */
void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
    contextMenu->onInitMenus(menuItems,menuIcon);
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
void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data) {
    infoData->onInfoData(serverConnectionHandlerID,id,type,data);
    //don't add code here when using infoData. It's all been done.
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
    BEGIN_CREATE_HOTKEYS(4);  /* Create n hotkeys. Size must be correct for allocating memory. */
	CREATE_HOTKEY("TS3_NEXT_TAB_AND_TALK_START", "Next Tab and Talk Start");
    CREATE_HOTKEY("TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START", "Next Tab and Whisper all Channel Commanders Start");
    CREATE_HOTKEY("TS3_SWITCH_N_TALK_END", "SnT Stop");
    CREATE_HOTKEY("CHANNEL_MUTER", "Toggle Channel Mute");
	END_CREATE_HOTKEYS;
	/* The client will call ts3plugin_freeMemory to release all allocated memory */
}

/* Show an error message if the plugin failed to load */
void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    centralStation->onConnectStatusChangeEvent(serverConnectionHandlerID,newStatus,errorNumber);
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
    else if ((visibility != LEAVE_VISIBILITY) && (TSHelpers::IsClientQuery(serverConnectionHandlerID,clientID)))
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
    #ifdef USE_POSITIONAL_AUDIO
    positionalAudio.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    #endif
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
    else if ((visibility != LEAVE_VISIBILITY) && (TSHelpers::IsClientQuery(serverConnectionHandlerID,clientID)))
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
    #ifdef USE_POSITIONAL_AUDIO
    positionalAudio.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    #endif
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
    else if ((visibility != LEAVE_VISIBILITY) && (TSHelpers::IsClientQuery(serverConnectionHandlerID,clientID)))
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
    #ifdef USE_POSITIONAL_AUDIO
    positionalAudio.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility,myID);
    #endif
}

int ts3plugin_onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
//    TSLogging::Print(QString("onServerErrorEvent: %1 %2 %3").arg((returnCode ? returnCode : "")).arg(error).arg(errorMessage),serverConnectionHandlerID,LogLevel_DEBUG);
    Q_UNUSED(errorMessage);
    Q_UNUSED(returnCode);
    Q_UNUSED(extraMessage);
//    if  (error== ERROR_client_is_flooding)
//    {
////        TSLogging::Error("Client is flooding. Need throttle.");
//    }
//    if(returnCode) {
//        TSLogging::Print("have returnCode");
//        /* A plugin could now check the returnCode with previously (when calling a function) remembered returnCodes and react accordingly */
//        /* In case of using a a plugin return code, the plugin can return:
//         * 0: Client will continue handling this error (print to chat tab)
//         * 1: Client will ignore this error, the plugin announces it has handled it */

//        if(error != ERROR_ok)
//        {
//            TSLogging::Print("error != ERROR_ok");
//          #ifdef USE_POSITIONAL_AUDIO
//            return positionalAudio.onServerErrorEvent(serverConnectionHandlerID,errorMessage,error,returnCode,extraMessage);
//          #endif
//        }

//        return 1;
//    }
    int isHandledError = 0;
#ifdef USE_POSITIONAL_AUDIO
    if (positionalAudio.isRunning())
    {
        if (error==ERROR_client_is_flooding)
        {
            QString sUId = TSServersInfo::instance()->GetServerInfo(serverConnectionHandlerID)->getUniqueId();
            settingsPositionalAudio->SetServerBlock(sUId,true,serverConnectionHandlerID);
        }
//        else if (error==ERROR_channel_invalid_password)
//        {
//            TSLogging::Print("Invalid Password.");
//        }
    }
#endif

    return isHandledError;  /* If no plugin return code was used, the return value of this function is ignored */
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
    bool isMe = talkers->onTalkStatusChangeEvent(serverConnectionHandlerID,status,isReceivedWhisper,clientID);

    if (channel_Muter.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe))
        return; //Client is muted;

#ifdef USE_RADIO
    radio.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
#endif

    ducker_G.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
    if (!ducker_G.isRunning() || !ducker_G.isClientMusicBotRt(serverConnectionHandlerID,clientID))
    {
        ducker_C.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
        positionSpread.onTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID,isMe);
    }
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels)
{
//    TSLogging::Print(QString("clientID: %1").arg(clientID));
    if (clientID > 32767)
    {
        clientID = 65535 - clientID + 1;
//        TSLogging::Print(QString("corrected clientID: %1").arg(clientID));
    }

    if (channel_Muter.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels))
        return; //Client is muted;

#ifdef USE_RADIO
    radio.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels);
#endif

    if (!ducker_G.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels))
        ducker_C.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels);
}

void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
//    TSLogging::Print(QString("clientID: %1").arg(clientID));
    if (clientID > 32767)
    {
        clientID = 65535 - clientID + 1;
//        TSLogging::Print(QString("corrected clientID: %1").arg(clientID));
    }

    positionSpread.onEditPostProcessVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels,channelSpeakerArray,channelFillMask);
}

void ts3plugin_onEditMixedPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
}

void ts3plugin_onCustom3dRolloffCalculationClientEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume)
{
//    TSLogging::Print(QString("Distance: %1 Volume: %2").arg(distance).arg(*volume));
#ifdef USE_POSITIONAL_AUDIO
    positionalAudio.onCustom3dRolloffCalculationClientEvent(serverConnectionHandlerID,clientID,distance,volume);
#endif
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
#ifdef USE_RADIO
    radio.setHomeId(serverConnectionHandlerID);
#endif
    ducker_C.setHomeId(serverConnectionHandlerID);
    positionSpread.setHomeId(serverConnectionHandlerID);
//    infoData->setHomeId(serverConnectionHandlerID);
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
    contextMenu->onMenuItemEvent(serverConnectionHandlerID,type,menuItemID,selectedItemID);
    PluginItemType itype;
    switch(type) {
        case PLUGIN_MENU_TYPE_GLOBAL:
            /* Global menu item was triggered. selectedItemID is unused and set to zero. */
            itype = PLUGIN_SERVER;   //admittedly not the same, however...
//            switch(menuItemID) {
//                case MENU_ID_GLOBAL_1:
//                    /* Menu global 1 was triggered */
//                    break;
//                case MENU_ID_GLOBAL_2:
//                    /* Menu global 2 was triggered */
//                    break;
//                default:
//                    break;
//            }
            break;
        case PLUGIN_MENU_TYPE_CHANNEL:
            /* Channel contextmenu item was triggered. selectedItemID is the channelID of the selected channel */
            itype = PLUGIN_CHANNEL;
            break;
        case PLUGIN_MENU_TYPE_CLIENT:
            /* Client contextmenu item was triggered. selectedItemID is the clientID of the selected client */
            itype = PLUGIN_CLIENT;
            break;
        default:
            break;
    }
    infoData->RequestUpdate(serverConnectionHandlerID,selectedItemID,itype);
}

void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char* pluginName, const char* pluginCommand)
{
    // pluginName is the dll name minus _someOS, not ts3plugin_name(); thanks for wasting my lifes time.
    // if the user for whatever reason renames the dll, this breaks the code
    if (QString(pluginName) != QString(ts3plugin_name()).toLower())
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
    #ifdef USE_POSITIONAL_AUDIO
    if (!positionalAudio.onPluginCommand(serverConnectionHandlerID, clientID, (clientID == myID) , cmd, args_qs))
    {
        TSLogging::Error("Error on plugin command",serverConnectionHandlerID,NULL,true);
    }
    #endif
}

/* This function is called if a plugin hotkey was pressed. Omit if hotkeys are unused. */
void ts3plugin_onHotkeyEvent(const char* keyword) {
    /* Identify the hotkey by keyword ("keyword_1", "keyword_2" or "keyword_3" in this example) and handle here... */
    ts3plugin_processCommand((uint64)NULL,keyword);
}

void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB)
{
    centralStation->onServerGroupListEvent(serverConnectionHandlerID,serverGroupID,name,type,iconID,saveDB);
}

void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
    centralStation->onServerGroupListFinishedEvent(serverConnectionHandlerID);
}

void ts3plugin_onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char* name, int type, int iconID, int saveDB)
{
    centralStation->onChannelGroupListEvent(serverConnectionHandlerID,channelGroupID,name,type,iconID,saveDB);
}

void ts3plugin_onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
    centralStation->onChannelGroupListFinishedEvent(serverConnectionHandlerID);
}

//void ts3plugin_onServerGroupClientListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char* clientNameIdentifier, const char* clientUniqueID)
//{
//    TSLogging::Print(QString("onServerGroupClientListEvent: %1,%2,%3,%4,%5").arg(serverConnectionHandlerID).arg(serverGroupID).arg(clientDatabaseID).arg(clientNameIdentifier).arg(clientUniqueID));
//}
