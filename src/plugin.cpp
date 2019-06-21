/*
 * Author: Thorsten Weinz (philosound@gmail.com)
 */

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
#endif

#include <memory>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QtCore/qglobal.h>

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"
#include "plugin.h"

#include "core/ts_logging_qt.h"
#include "core/ts_helpers_qt.h"

#include "core/ts_serversinfo.h"
#include "plugin_qt.h"

#include "ts_ptt_qt.h"

struct TS3Functions ts3Functions;

std::unique_ptr<Plugin> plugin;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); dest[destSize-1] = '\0'; }
#endif

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

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

/* Unique name identifying this plugin */
const char* ts3plugin_name() { return plugin->kPluginName; }

/* Plugin version */
const char* ts3plugin_version() { return plugin->kPluginVersion; }

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() { return plugin->kPluginApiVersion; }

/* Plugin author */
const char* ts3plugin_author() { return plugin->kPluginAuthor; }

/* Plugin description */
const char* ts3plugin_description() { return plugin->kPluginDescription; }

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) { ts3Functions = funcs; }

/*
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on failure.
 * If the function returns 1 on failure, the plugin will be unloaded again.
 */
int ts3plugin_init()
{
    TSPtt::instance()->Init(&command_mutex);
    return plugin->init();  /* 0 = success, 1 = failure */
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown()
{
    plugin->shutdown();
    plugin.release();
}

/****************************** Optional functions ********************************/
/*
 * Following functions are optional, if not needed you don't need to implement them.
 */

/* Tell client if plugin offers a configuration window. If this function is not implemented, it's an assumed "does not offer" (PLUGIN_OFFERS_NO_CONFIGURE). */
int ts3plugin_offersConfigure() { return plugin->kPluginOffersConfigure; }

/* Plugin might offer a configuration window. If ts3plugin_offersConfigure returns 0, this function does not need to be implemented. */
void ts3plugin_configure(void* handle, void* qParentWidget) { plugin->configure(handle, qParentWidget); }

/*
 * If the plugin wants to use error return codes or plugin commands, it needs to register a command ID. This function will be automatically
 * called after the plugin was initialized. This function is optional. If you don't use error return codes or plugin commands, the function
 * can be omitted.
 * Note the passed pluginID parameter is no longer valid after calling this function, so you must copy it and store it in the plugin.
 */
void ts3plugin_registerPluginID(const char* id) { plugin = std::make_unique<Plugin>(id); }

/* Plugin command keyword. Return NULL or "" if not used. */
const char* ts3plugin_commandKeyword() { return "ct"; }

/* Plugin processes console command. Return 0 if plugin handled the command, 1 if not handled. */
int ts3plugin_processCommand(uint64 serverConnectionHandlerID, const char* command)
{
    if (!(command_mutex.tryLock(PLUGIN_THREAD_TIMEOUT)))
    {
        TSLogging::Log("Timeout while waiting for mutex", serverConnectionHandlerID, LogLevel_WARNING);
        return 1;
    }

    int ret = 0;

    QString cmd_qs;
    cmd_qs = command;

    auto inside = (cmd_qs.startsWith(QLatin1String("\""))); //true if the first character is "
    auto tmpList = cmd_qs.split(QRegExp(QStringLiteral("\"")), QString::SkipEmptyParts); // Split by " and make sure you don't have an empty string at the beginning
    QStringList args_qs;
    foreach (auto s, tmpList)
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
    if (cmd_qs == QLatin1String("HIDE_TASKBAR"))
    {
        auto mainWindow = TSHelpers::GetMainWindow();
        auto type = mainWindow->windowType();
        TSLogging::Log(QString("type: %1").arg(type));
        auto flags = mainWindow->windowFlags();
        TSLogging::Log(QString("flags: %1").arg(flags));
        if (mainWindow)
        {
            mainWindow->setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);   //mainWindow->windowFlags() |
            mainWindow->setWindowFlags(flags);
        }

    }
    else if (cmd_qs == QLatin1String("PS_TOGGLE"))
    {
        serverConnectionHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        unsigned int error;
        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            TSLogging::Error("(PS_TOGGLE)",serverConnectionHandlerID,error);
            ret = 1;
        }
        else
        {
            unsigned int permId;
            if ((error = ts3Functions.getPermissionIDByName(serverConnectionHandlerID, "b_client_is_priority_speaker", &permId)) != ERROR_ok)
            {
                TSLogging::Error("(PS_TOGGLE)",serverConnectionHandlerID,error);
                ret = 1;
            }
            else
            {
                uint64 myDbId;
                if ((error = ts3Functions.getClientVariableAsUInt64(serverConnectionHandlerID, myID, CLIENT_DATABASE_ID, &myDbId)) != ERROR_ok)
                {
                    TSLogging::Error("(PS_TOGGLE)",serverConnectionHandlerID,error);
                    ret = 1;
                }
                else
                {
                    int isPrioritySpeaker;
                    if ((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, myID, CLIENT_IS_PRIORITY_SPEAKER, &isPrioritySpeaker)) != ERROR_ok)
                    {
                        TSLogging::Error("(PS_TOGGLE)",serverConnectionHandlerID,error);
                        ret = 1;
                    }
                    else
                    {
                        const unsigned int permissionIDArray [1] = {permId};

                        if (isPrioritySpeaker == 0)
                        {
                            const int permissionValueArray [1] = {1};
                            const int permissionSkipArray [1]  = {0};
                            ts3Functions.requestClientAddPerm(serverConnectionHandlerID, myDbId, permissionIDArray, permissionValueArray, permissionSkipArray, 1, NULL);
                        }
                        else
                            ts3Functions.requestClientDelPerm(serverConnectionHandlerID, myDbId, permissionIDArray, 1, NULL);
                    }
                }
            }
        }
    }
    else if (cmd_qs == "TOGGLE_SELF_SERVER_GROUP")
    {
        if (args_qs.size() < 2)
            ret = 1;
        else
        {
            unsigned int error;
            uint64 targetServer = 0;
            auto serverName = args_qs.takeFirst();

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
                    auto serverGroupName = args_qs.takeFirst();
                    auto serverGroupId = plugin->servers_info().get_server_info(targetServer)->GetServerGroupId(serverGroupName);
                    if (!serverGroupId)
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
            auto serverName = args_qs.takeFirst();

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
                    uint64 myDbId;
                    if ((error = ts3Functions.getClientVariableAsUInt64(targetServer, myID, CLIENT_DATABASE_ID, &myDbId)) != ERROR_ok)
                    {
                        TSLogging::Error("(SET_SELF_CHANNEL_GROUP) Could not get self client db id", targetServer, error, true);
                        ret = 1;
                    }
                    else
                    {
                        uint64 myChannelGroupId;
                        if ((error = ts3Functions.getClientVariableAsUInt64(targetServer, myID, CLIENT_CHANNEL_GROUP_ID, &myChannelGroupId)) != ERROR_ok)
                        {
                            TSLogging::Error("(SET_SELF_CHANNEL_GROUP)",targetServer,error,true);
                            ret = 0;
                        }
                        else
                        {
                            uint64 channelGroupId;
                            auto channelGroupName = args_qs.takeFirst();
                            if (channelGroupName == QLatin1String("DEFAULT_CHANNEL_GROUP"))
                                channelGroupId = plugin->servers_info().get_server_info(targetServer)->getDefaultChannelGroup();
                            else
                                channelGroupId = plugin->servers_info().get_server_info(targetServer)->GetChannelGroupId(channelGroupName);

                            if ((channelGroupId == (uint64)NULL) || (channelGroupId == myChannelGroupId))
                                ret = 1;
                            else
                            {
                                uint64 targetChannelId;
                                if (cmd_qs == QLatin1String("SET_SELF_CHANNEL_GROUP"))
                                {
                                    if ((error = ts3Functions.getChannelOfClient(targetServer,myID,&targetChannelId)) != ERROR_ok)
                                    {
                                        TSLogging::Error("(SET_SELF_CHANNEL_GROUP)",targetServer,error,true);
                                        ret = 0;
                                    }
                                }
                                else if (cmd_qs == QLatin1String("SET_SELF_CHANNEL_GROUP_INHERITED"))
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
                                    uint64 channelGroupIdArray[SIZE] = { channelGroupId };
                                    uint64 channelIdArray[SIZE] = { targetChannelId };
                                    uint64 clientDbIdArray[SIZE] = { myDbId };
//                                    TSLogging::Log(QString("SET_SELF_CHANNEL_GROUP: %1 %2 %3").arg(channelGroupIdArray[0]).arg(channelIdArray[0]).arg(clientDbIdArray[0]));
                                    ts3Functions.requestSetClientChannelGroup(targetServer, &channelGroupIdArray[0], &channelIdArray[0], &clientDbIdArray[0], SIZE, NULL);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
        ret = plugin->process_command(serverConnectionHandlerID, cmd_qs, args_qs); // Dispatch

    command_mutex.unlock();
    return ret;
}

/*
 * Initialize plugin menus.
 * This function is called after ts3plugin_init and ts3plugin_registerPluginID. A pluginID is required for plugin menus to work.
 * Both ts3plugin_registerPluginID and ts3plugin_freeMemory must be implemented to use menus.
 * If plugin menus are not used by a plugin, do not implement this function or return NULL.
 */
void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon)
{
    plugin->context_menu().onInitMenus(menuItems, menuIcon);
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
const char* ts3plugin_infoTitle() { return ts3plugin_name(); }

/*
 * Dynamic content shown in the right column in the info frame. Memory for the data string needs to be allocated in this
 * function. The client will call ts3plugin_freeMemory once done with the string to release the allocated memory again.
 * Check the parameter "type" if you want to implement this feature only for specific item types. Set the parameter
 * "data" to NULL to have the client ignore the info data.
 */
void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data)
{
    plugin->info_data().onInfoData(serverConnectionHandlerID, id, type, data);
    //don't add code here when using infoData. It's all been done.
}

/* Required to release the memory for parameter "data" allocated in ts3plugin_infoData */
void ts3plugin_freeMemory(void* data) { free(data); }

/* Helper function to create a hotkey */
static struct PluginHotkey* createHotkey(const char* keyword, const char* description)
{
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
void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys)
{
	/* Register hotkeys giving a keyword and a description.
	 * The keyword will be later passed to ts3plugin_onHotkeyEvent to identify which hotkey was triggered.
	 * The description is shown in the clients hotkey dialog. */
    BEGIN_CREATE_HOTKEYS(5);  /* Create n hotkeys. Size must be correct for allocating memory. */
	CREATE_HOTKEY("TS3_NEXT_TAB_AND_TALK_START", "Next Tab and Talk Start");
    CREATE_HOTKEY("TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START", "Next Tab and Whisper all Channel Commanders Start");
    CREATE_HOTKEY("TS3_SWITCH_N_TALK_END", "SnT Stop");
    CREATE_HOTKEY("CHANNEL_MUTER", "Toggle Channel Mute");
    CREATE_HOTKEY("PS_TOGGLE", "Toggle Priority Speaker");
	END_CREATE_HOTKEYS;
	/* The client will call ts3plugin_freeMemory to release all allocated memory */
}

/* Show an error message if the plugin failed to load */
void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    plugin->onConnectStatusChangeEvent(serverConnectionHandlerID, newStatus, errorNumber);
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage)
{
    plugin->onClientMoveEvent(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moveMessage);
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage)
{
    plugin->onClientMoveTimeoutEvent(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, timeoutMessage);
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage)
{
    plugin->onClientMoveMovedEvent(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moverID, moverName, moverUniqueIdentifier, moveMessage);
}

int ts3plugin_onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage)
{
    return plugin->on_server_error(serverConnectionHandlerID, errorMessage, error, returnCode, extraMessage);  /* If no plugin return code was used, the return value of this function is ignored */
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
    plugin->onTalkStatusChangeEvent(serverConnectionHandlerID, status, isReceivedWhisper, clientID);
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels)
{
    plugin->onEditPlaybackVoiceDataEvent(serverConnectionHandlerID, clientID, samples, sampleCount, channels);
}

void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
    plugin->onEditPostProcessVoiceDataEvent(serverConnectionHandlerID, clientID, samples, sampleCount, channels, channelSpeakerArray, channelFillMask);
}

void ts3plugin_onCustom3dRolloffCalculationClientEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume)
{
    plugin->on_custom_3d_rolloff_calculation(serverConnectionHandlerID, clientID, distance, volume);
}

/* Clientlib rare */
void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue)
{
    plugin->on_client_self_variable_update(serverConnectionHandlerID, flag, oldValue, newValue);
}

/* Client changed current server connection handler */
void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID)
{
    plugin->currentServerConnectionChanged(serverConnectionHandlerID);
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
void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
    plugin->onMenuItemEvent(serverConnectionHandlerID, type, menuItemID, selectedItemID);
}

void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char* pluginName, const char* pluginCommand)
{
    plugin->on_plugin_command(serverConnectionHandlerID, pluginName, pluginCommand);
}

/* This function is called if a plugin hotkey was pressed. Omit if hotkeys are unused. */
void ts3plugin_onHotkeyEvent(const char* keyword)
{
    /* Identify the hotkey by keyword ("keyword_1", "keyword_2" or "keyword_3" in this example) and handle here... */
    ts3plugin_processCommand((uint64)NULL, keyword);
}

void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB)
{
    plugin->on_server_group_list(serverConnectionHandlerID, serverGroupID, name, type, iconID, saveDB);
}

void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
    plugin->on_server_group_list_finished(serverConnectionHandlerID);
}

void ts3plugin_onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char* name, int type, int iconID, int saveDB)
{
    plugin->on_channel_group_list(serverConnectionHandlerID, channelGroupID, name, type, iconID, saveDB);
}

void ts3plugin_onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
    plugin->on_channel_group_list_finished(serverConnectionHandlerID);
}
