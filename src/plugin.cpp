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

#include "tsfunctions.h"
#include "talkers.h"
#include "config.h"
#include "ct_volumesuppression.h"
#include "snt.h"
#include "pantalkers.h"

struct TS3Functions ts3Functions;

TSFunctions* ts = TSFunctions::instance();
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

//#define PLUGIN_THREAD_TIMEOUT 1000

#define TIMER_MSEC 10000

/* Array for request client move return codes. See comments within ts3plugin_processCommand for details */
static char requestClientMoveReturnCodes[REQUESTCLIENTMOVERETURNCODES_SLOTS][RETURNCODE_BUFSIZE];

// Plugin values
char* pluginID = NULL;

CT_VolumeSuppression ct_Ducker;
SnT snt;
PanTalkers panTalkers;

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
    return PLUGIN_NAME;
}

/* Plugin version */
const char* ts3plugin_version() {
    return "0.9.1";
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
    return "INTERNAL USE ONLY. This plugin is in internal beta status. It is not ready or meant for public.\n\nFeatures:\nStereo Position  Spread,\nHome Tab Ducking\nSwitch\'n\'Talk Button (Cross Server Tab PTT)\n\nFor information on the modules, use the \'?\' button on the topper right of the plugin settings window.\n";
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
    ts3Functions.logMessage("CrossTalk:init",LogLevel_INFO,PLUGIN_NAME, 0);

    if (CONSOLE_OUTPUT)
        freopen("CONOUT$", "wb", stdout);   //Makes printf work in Release mode (shouldn't been necessary, but is...)

	/* Initialize return codes array for requestClientMove */
	memset(requestClientMoveReturnCodes, 0, REQUESTCLIENTMOVERETURNCODES_SLOTS * RETURNCODE_BUFSIZE);

    ts->Init();
    ts->InitLocalization();
    ts->connect(ts,SIGNAL(Command(char*,char*)),&snt,SLOT(ParseCommand(char*,char*)));
    ts->connect(ts,SIGNAL(PttDelayFinished()),&snt,SLOT(PttDelayFinished()));

    QSettings cfg(ts->GetFullConfigPath(), QSettings::IniFormat);
    ct_Ducker.setEnabled(cfg.value("ducking_enabled",true).toBool());
    ct_Ducker.setValue(cfg.value("ducking_value",-23.0f).toFloat());
    ct_Ducker.setDuckingReverse(cfg.value("ducking_reverse",false).toBool());
    panTalkers.setEnabled(cfg.value("stereo_position_spread_enabled",true).toBool());
    panTalkers.setSpreadWidth(cfg.value("stereo_position_spread_value",0.5f).toFloat());
    panTalkers.setExpertModeEnabled(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
    panTalkers.setRegionHomeTab(cfg.value("stereo_position_spread_region_home",1).toInt());
    panTalkers.setRegionWhisper(cfg.value("stereo_position_spread_region_whisper",2).toInt());
    panTalkers.setRegionOther(cfg.value("stereo_position_spread_region_other",0).toInt());

    // Support enabling the plugin while already connected
    uint64* serverList;
    if(ts3Functions.getServerConnectionHandlerList(&serverList) == ERROR_ok)
    {
        for(int i=0; serverList[i] != NULL; ++i)
        {
            int status;
            if (ts3Functions.getConnectionStatus(serverList[i], &status) != ERROR_ok)
                continue;

            if (status > STATUS_DISCONNECTED)
            {
                uint64 channelID;
                for (int j=STATUS_CONNECTING; j<=status; ++j)
                {
                    ts3plugin_onConnectStatusChangeEvent(serverList[i], j, ERROR_ok);
                    if (j == STATUS_CONNECTED)
                    {
                        // Get our channel and publish it as default channel
                        anyID myID;
                        if(ts->GetClientId(serverList[i],&myID) == ERROR_ok)
                        {
                            if(ts3Functions.getChannelOfClient(serverList[i],myID,&channelID) == ERROR_ok)
                            {
                                uint64 channelParentID;
                                if (ts3Functions.getParentChannelOfChannel(serverList[i],channelID,&channelParentID) == ERROR_ok)
                                    ts3plugin_onNewChannelEvent(serverList[i], channelID, channelParentID);
                            }
                        }
                    }
                    else if (j == STATUS_CONNECTION_ESTABLISHING)
                    {
                        // publish all other channels
                        uint64* channelList;
                        if (ts3Functions.getChannelList(serverList[i],&channelList) == ERROR_ok)
                        {
                            for (int k=0;channelList[k] != NULL;++k)
                            {
                                if (channelList[k] == channelID)
                                    continue;

                                uint64 channelParentID;
                                if (ts3Functions.getParentChannelOfChannel(serverList[i],channelList[k],&channelParentID) == ERROR_ok)
                                    ts3plugin_onNewChannelEvent(serverList[i], channelList[k], channelParentID); // ToDo: Hope it's not necessary to sort those
                            }
                            ts3Functions.freeMemory(channelList);
                        }
                    }
                }
                if (status == STATUS_CONNECTION_ESTABLISHED)
                {
                    ts3plugin_currentServerConnectionChanged(serverList[i]);
                    unsigned int error;
                    if ((error = talkers->RefreshTalkers(serverList[i])) != ERROR_ok)
                        ts->Error(serverList[i],error,"Error refreshing talkers: ");
                }
            }
            ts3Functions.freeMemory(serverList);
        }

        // Get the active server tab
        uint64 scHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        if (scHandlerID != 0)
            ts3plugin_currentServerConnectionChanged(scHandlerID);
    }

    ts3Functions.logMessage("CrossTalk:init done",LogLevel_INFO,PLUGIN_NAME, 0);
    return 0;  /* 0 = success, 1 = failure */
}

/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {

	/*
	 * Note:
	 * If your plugin implements a settings dialog, it must be closed and deleted here, else the
	 * TeamSpeak client will most likely crash (DLL removed but dialog from DLL code still open).
	 */
    ts->ShutdownLocalization();
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
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetDuckingEnabled(bool)),&ct_Ducker,SLOT(setEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetDuckingValue(float)),&ct_Ducker,SLOT(setValue(float)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetDuckingReverse(bool)),&ct_Ducker,SLOT(setDuckingReverse(bool)));

    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadEnabled(bool)), &panTalkers,SLOT(setEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadValue(float)), &panTalkers,SLOT(setSpreadWidth(float)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertEnabled(bool)), &panTalkers,SLOT(setExpertModeEnabled(bool)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertValueHome(int)),&panTalkers,SLOT(setRegionHomeTab(int)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertValueWhisper(int)),&panTalkers, SLOT(setRegionWhisper(int)));
    qParentWidget_p->connect(qParentWidget_p,SIGNAL(SetStereoPositionSpreadExpertValueOther(int)),&panTalkers, SLOT(setRegionOther(int)));
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
	size_t length = strlen(command);
	char* str = (char*)malloc(length+1);
	_strcpy(str, length+1, command);

	// Seperate the argument from the command
	char* arg = strchr(str, ' ');
	if(arg != NULL)
	{
		// Split the string by inserting a NULL-terminator
		*arg = (char)NULL;
		arg++;
	}

    //ParseCommand(str, arg);
    if(!strcmp(str, "TS3_TEST_DUCKER"))
        ct_Ducker.setActive(!ct_Ducker.isActive());
    else
        ts->ParseCommand(str,arg);

	free(str);

	return 0;  /* Plugin did not handle command */
}

/*
* Implement the following three functions when the plugin should display a line in the server,channel,client info.
* If any of ts3plugin_infoTitle, ts3plugin_infoData or ts3plugin_freeMemory is missing, the info text will not be displayed.
*/

/* Static title shown in the left column in the info frame */
const char* ts3plugin_infoTitle() {
	return NULL;
}

/*
 * Dynamic content shown in the right column in the info frame. Memory for the data string needs to be allocated in this
 * function. The client will call ts3plugin_freeMemory once done with the string to release the allocated memory again.
 * Check the parameter "type" if you want to implement this feature only for specific item types. Set the parameter
 * "data" to NULL to have the client ignore the info data.
 */
void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data) {
}

/* Required to release the memory for parameter "data" allocated in ts3plugin_infoData */
void ts3plugin_freeMemory(void* data) {
	free(data);
}

/*
 * Plugin requests to be always automatically loaded by the TeamSpeak 3 client unless
 * the user manually disabled it in the plugin dialog.
 * This function is optional. If missing, no autoload is assumed.
 */
int ts3plugin_requestAutoload() {
	return 1;  /* 1 = request autoloaded, 0 = do not request autoload */
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
    BEGIN_CREATE_HOTKEYS(5);  /* Create 2 hotkeys. Size must be correct for allocating memory. */
    CREATE_HOTKEY("TS3_NEXT_TAB_AND_TALK_END", "Next Tab and Talk Stop");
	CREATE_HOTKEY("TS3_NEXT_TAB_AND_TALK_START", "Next Tab and Talk Start");
    CREATE_HOTKEY("TS3_NEXT_TAB_AND_WHISPER_END", "Next Tab and Whisper Stop");
    CREATE_HOTKEY("TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START", "Next Tab and Whisper all Channel Commanders Start");
    CREATE_HOTKEY("TS3_SWITCH_N_TALK_END", "SnT Stop");
	END_CREATE_HOTKEYS;

	/* The client will call ts3plugin_freeMemory to release all allocated memory */
}

/* This function is called if a plugin hotkey was pressed. Omit if hotkeys are unused. */
void ts3plugin_onHotkeyEvent(const char* keyword) {
	/* Identify the hotkey by keyword ("keyword_1", "keyword_2" or "keyword_3" in this example) and handle here... */
    ts->ParseCommand((char*)keyword,(char)NULL);
}

/* Show an error message if the plugin failed to load */
void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    ts->onConnectStatusChangeEvent(serverConnectionHandlerID, newStatus, errorNumber);
    talkers->onConnectStatusChangeEvent(serverConnectionHandlerID,newStatus,errorNumber);
    if (newStatus==STATUS_CONNECTION_ESTABLISHED)
    {
        ts3plugin_currentServerConnectionChanged(serverConnectionHandlerID);
        // Fake client move of myself

        // Get My Id on this handler
        anyID myID;
        if(ts->GetClientId(serverConnectionHandlerID,&myID) == ERROR_ok)
        {
            // Get My channel on this handler
            uint64 channelID;
            unsigned int error;
            if((error=ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
                ts->Error(serverConnectionHandlerID,error,"ts3plugin_onConnectStatusChangeEvent: Could not get channel of client.");
            else
                ts3plugin_onClientMoveEvent(serverConnectionHandlerID, myID, 0, channelID, RETAIN_VISIBILITY, "");
        }
    }
}

void ts3plugin_onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID)
{
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage)
{
    //talkers->onClientMoveEvent(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moveMessage);
    ct_Ducker.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility);
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage)
{
    //talkers->onClientMoveTimeoutEvent(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility,timeoutMessage);
    ct_Ducker.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility);
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage)
{
    //talkers->onClientMoveMovedEvent(serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility,moverID,moverName,moverUniqueIdentifier,moveMessage);
    ct_Ducker.onClientMoveEvent(serverConnectionHandlerID,clientID,oldChannelID,newChannelID,visibility);
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
    talkers->onTalkStatusChangeEvent(serverConnectionHandlerID,status,isReceivedWhisper,clientID);
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels)
{
    ct_Ducker.onEditPlaybackVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels);
}

void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask)
{
    panTalkers.onEditPostProcessVoiceDataEvent(serverConnectionHandlerID,clientID,samples,sampleCount,channels,channelSpeakerArray,channelFillMask);
}

/* Clientlib rare */
void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue)
{
     snt.onClientSelfVariableUpdateEvent(serverConnectionHandlerID,flag,oldValue,newValue);
//	if ((flag == CLIENT_INPUT_HARDWARE) && (strcmp (newValue,"1") == 0))
//	{
//		if (wo_shallActivatePtt==true)
//		{
//            ts->SetPushToTalk(serverConnectionHandlerID, PTT_ACTIVATE);
//			wo_shallActivatePtt=false;
//		}
//        //seems to be equal to a ts3plugin_currentServerConnectionChanged when initiated by the user
//        //except that this version will fire when returning from a cross server ptt event
//        // (ts3plugin_currentServerConnectionChanged is not even fired when ptt-switching for whatever reason)
//        // Furtunately, we like that here.
//	}
}

/* Client changed current server connection handler */
void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID)
{
    ct_Ducker.setHomeId(serverConnectionHandlerID);
    panTalkers.setHomeId(serverConnectionHandlerID);
}
