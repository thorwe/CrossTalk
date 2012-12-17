#include "snt.h"

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"

//! Constructor
/*!
 * \brief SnT::SnT Creates an instance of this class
 * \param parent optional Qt Object
 */
SnT::SnT(QObject *parent) :
    QObject(parent)
{
    ts = TSFunctions::instance();
}

//! When the input hardware gets activated, check if there's a Ptt activation scheduled and if so, turn Ptt on
/*!
 * \brief SnT::onClientSelfVariableUpdateEvent equivalent TS Client SDK
 * \param serverConnectionHandlerID equivalent TS Client SDK
 * \param flag equivalent TS Client SDK
 * \param oldValue equivalent TS Client SDK
 * \param newValue equivalent TS Client SDK
 */
void SnT::onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char *oldValue, const char *newValue)
{
    if ((flag == CLIENT_INPUT_HARDWARE) && (strcmp (newValue,"1") == 0))
    {
        if (m_shallActivatePtt==true)
        {
            ts->SetPushToTalk(serverConnectionHandlerID, PTT_ACTIVATE);
            m_shallActivatePtt=false;
        }
        // seems to be equal to a ts3plugin_currentServerConnectionChanged WHEN initiated by the user
        // except that this version will fire when returning from a cross server ptt event
        // (ts3plugin_currentServerConnectionChanged is not even fired when ptt-switching for whatever reason)
        // Furtunately, we like that here.
    }
}

//! Parse Plugin Commands for this module
/*!
 * \brief SnT::ParseCommand Qt Slot
 * \param cmd the command
 * \param arg arguments (if any)
 */
void SnT::ParseCommand(char* cmd, char* arg)
{
    unsigned int error;

    // Get the active server
    uint64 scHandlerID = ts->GetActiveServerConnectionHandlerID();
    if(scHandlerID == NULL)
    {
        ts3Functions.logMessage("Failed to get an active server, falling back to current server", LogLevel_DEBUG, PLUGIN_NAME, 0);
        scHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        if(scHandlerID == NULL) return;
    }

    int status;
    if((error = ts3Functions.getConnectionStatus(scHandlerID, &status)) != ERROR_ok)
        ts->Error(scHandlerID,error,"Error retrieving connection status: ");

    /***** Communication *****/
    if(!strcmp(cmd, "TS3_PTT_ACTIVATE"))
    {
        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, PTT_ACTIVATE);
    }
    else if(!strcmp(cmd, "TS3_PTT_DEACTIVATE"))
    {
        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, PTT_DEACTIVATE);
    }
    else if(!strcmp(cmd, "TS3_PTT_TOGGLE"))
    {
        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, PTT_TOGGLE);
    }
    else if(!strcmp(cmd, "TS3_NEXT_TAB_AND_TALK_START"))
    {
        uint64 nextServer;
        if ((error = ts->GetActiveServerRelative(scHandlerID,true,&nextServer)) != ERROR_ok)
            ts->Error(scHandlerID,error,"Could not get next server.");

        if (scHandlerID == nextServer)
            return;

        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt=true;
        ts->SetActiveServer(nextServer);
    }
    else if(!strcmp(cmd, "TS3_NEXT_TAB_AND_TALK_END"))
    {
        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings
        ts->SetPrevActiveServer(scHandlerID);
    }
    else if (!strcmp(cmd, "TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START"))
    {
        uint64 nextServer;
        if ((error = ts->GetActiveServerRelative(scHandlerID,true,&nextServer)) != ERROR_ok)
            ts->Error(scHandlerID,error,"Could not get next server.");

        if (scHandlerID == nextServer)
            return;

        anyID myID;
        if((error = ts->GetClientId(nextServer,&myID)) != ERROR_ok)
        {
            ts->Error(scHandlerID,error, "Could not get my id on the next server: ");
            return;
        }

        if ((error = ts->SetWhisperList(nextServer,GROUPWHISPERTYPE_CHANNELCOMMANDER,GROUPWHISPERTARGETMODE_ALL)) != ERROR_ok)
        {
            ts->Error(scHandlerID,error, "Could not set whisperlist: ");
            return;
        }

        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt=true;
        ts->SetActiveServer(nextServer);
    }
    else if (!strcmp(cmd, "TS3_NEXT_TAB_AND_WHISPER_END"))
    {
        if(status != STATUS_DISCONNECTED)
        {
            ts->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings
            if ((error = ts3Functions.requestClientSetWhisperList(scHandlerID,NULL,NULL,NULL,NULL)) != ERROR_ok)
                ts->Error(scHandlerID,error, "Could not release whisperlist.");
        }
        ts->SetPrevActiveServer(scHandlerID);
    }
    else if (!strcmp(cmd, "TS3_NEXT_TAB_AND_WHISPER_START"))
    {
        uint64 nextServer;
        if ((error = ts->GetActiveServerRelative(scHandlerID,true,&nextServer)) != ERROR_ok)
            ts->Error(scHandlerID,error,"Could not get next server.");

        if (scHandlerID == nextServer)
            return;

        anyID myID;
        if((error = ts->GetClientId(nextServer,&myID)) != ERROR_ok)
        {
            ts->Error(scHandlerID,error, "Could not get my id on the next server: ");
            return;
        }


        QString arg_qs;
        arg_qs = arg;
        QStringList args_qs = arg_qs.split(" ",QString::SkipEmptyParts);
        if (args_qs.count() < 2)
        {
            ts->Error(scHandlerID,NULL,"Too few arguments.");
            return;
        }

        arg_qs = args_qs.at(0);
        GroupWhisperType groupWhisperType = GROUPWHISPERTYPE_ENDMARKER;
        if (arg_qs.contains("COMMANDER",Qt::CaseInsensitive))
                groupWhisperType = GROUPWHISPERTYPE_CHANNELCOMMANDER;
        else if (arg_qs.contains("ALLC",Qt::CaseInsensitive))
                groupWhisperType = GROUPWHISPERTYPE_ALLCLIENTS;

        if (groupWhisperType == GROUPWHISPERTYPE_ENDMARKER)
        {
            ts->Error(scHandlerID,NULL,"Unsupported group whisper type.");
            return;
        }

        arg_qs = args_qs.at(1);
        GroupWhisperTargetMode groupWhisperTargetMode  = GROUPWHISPERTARGETMODE_ENDMARKER;
        if (arg_qs.contains("ALLPARENT"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ALLPARENTCHANNELS;
        else if (arg_qs.contains("PARENT"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_PARENTCHANNEL;
        else if (arg_qs.contains("CURRENT"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_CURRENTCHANNEL;
        else if (arg_qs.contains("SUB"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_SUBCHANNELS;
        else if (arg_qs.contains("ANCESTORCHANNELFAMILY"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY;
        else if (arg_qs.contains("FAMILY"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_CHANNELFAMILY;
        else if (arg_qs.contains("ALL"))
            groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ALL;

        if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ENDMARKER)
        {
            ts->Error(scHandlerID,NULL,"Could not recognize group whisper target mode.");
            return;
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)
        {
            ts->Error(scHandlerID,NULL,"The group whisper target mode ANCESTORCHANNELFAMILY is not supported.");
            return;
        }

        if ((error = ts->SetWhisperList(nextServer,groupWhisperType,groupWhisperTargetMode)) != ERROR_ok)
        {
            ts->Error(scHandlerID,error, "Could not set whisperlist: ");
            return;
        }

        if(status != STATUS_DISCONNECTED)
            ts->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt=true;
        ts->SetActiveServer(nextServer);
    }
    /***** Error handler *****/
//    else
//    {
//        ts->Error(scHandlerID, NULL, "Command not recognized");
//    }
}


//! Turn Ptt off delayed by the user client setting
/*!
 * \brief SnT::PttDelayFinished Timer slot
 */
void SnT::PttDelayFinished()
{
    // Turn off PTT
    ts->SetPushToTalk(ts->GetActiveServerConnectionHandlerID(), false);
}
