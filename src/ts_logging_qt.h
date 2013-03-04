#ifndef TS_LOGGING_QT_H
#define TS_LOGGING_QT_H

#ifndef PATH_BUFSIZE
#define PATH_BUFSIZE 512
#endif

#include <QtCore>
#include "public_definitions.h"
#include "ts3_functions.h"
#include "plugin.h"

namespace TSLogging
{
    bool GetErrorSound(QString &in);
    bool GetInfoIcon(QString &in);
    void PlayErrorSound(uint64 serverConnectionHandlerID);
    inline void PlayErrorSound()                                                                {PlayErrorSound(ts3Functions.getCurrentServerConnectionHandlerID());}

    void Error(QString message, uint64 serverConnectionHandlerID, unsigned int error, bool isSoundSilent);
    inline void Error(QString message, uint64 serverConnectionHandlerID, unsigned int error)    {Error(message, serverConnectionHandlerID, error, false);}
    inline void Error(QString message, unsigned int error)                                      {Error(message, 0, error, false);}
    inline void Error(QString message, bool isSoundSilent)                                      {Error(message, 0, NULL, isSoundSilent);}
    inline void Error(QString message)                                                          {Error(message, 0, NULL, false);}

    void Print(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel);
    inline void Print(QString message, LogLevel logLevel)                                       {Print(message, 0, logLevel);}
    inline void Print(QString message)                                                          {Print(message, 0, LogLevel_INFO);}

    void Log(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel);
    inline void Log(QString message, LogLevel logLevel)                                         {Log(message, 0, logLevel);}
    inline void Log(QString message)                                                            {Log(message, 0, LogLevel_INFO);}
}

#endif // TS_LOGGING_QT_H
