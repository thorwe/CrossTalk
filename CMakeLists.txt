cmake_minimum_required (VERSION 3.6)

project (CrossTalk LANGUAGES CXX)

find_package(Qt5 COMPONENTS Gui Widgets Network REQUIRED)
link_libraries(Qt5::Gui Qt5::Widgets Qt5::Network)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(
    "src"
    "deps"
)

set (QT_WEBSOCKET
    "deps/QtWebsocket/ServerThreaded/ServerThreaded.h"
    "deps/QtWebsocket/ServerThreaded/ServerThreaded.cpp"
    "deps/QtWebsocket/ServerThreaded/SocketThread.h"
    "deps/QtWebsocket/ServerThreaded/SocketThread.cpp"
    "deps/QtWebsocket/functions.h"
    "deps/QtWebsocket/functions.cpp"
    "deps/QtWebsocket/QTlsServer.h"
    "deps/QtWebsocket/QTlsServer.cpp"
    "deps/QtWebsocket/QWsFrame.h"
    "deps/QtWebsocket/QWsFrame.cpp"
    "deps/QtWebsocket/QWsHandshake.h"
    "deps/QtWebsocket/QWsHandshake.cpp"
    "deps/QtWebsocket/QWsServer.h"
    "deps/QtWebsocket/QWsServer.cpp"
    "deps/QtWebsocket/QWsSocket.h"
    "deps/QtWebsocket/QWsSocket.cpp"
    "deps/QtWebsocket/WsEnums.h"
)
source_group("QtWebSocket" FILES ${QT_WEBSOCKET})

set (POSITIONAL_AUDIO
    "src/positional_audio/config_positionalaudio.h"
    "src/positional_audio/config_positionalaudio.cpp"
    "src/positional_audio/definitions_positionalaudio.h"
    "src/positional_audio/groupbox_positionalaudio.h"
    "src/positional_audio/groupbox_positionalaudio.cpp"
    "src/positional_audio/groupbox_positionalaudio_servers.h"
    "src/positional_audio/groupbox_positionalaudio_servers.cpp"
    "src/positional_audio/groupbox_positionalaudio_status.h"
    "src/positional_audio/groupbox_positionalaudio_status.cpp"
    "src/positional_audio/guildwarstwo.h"
    "src/positional_audio/guildwarstwo.cpp"
    "src/positional_audio/minecraft.h"
    "src/positional_audio/minecraft.cpp"
    "src/positional_audio/mod_positionalaudio.h"
    "src/positional_audio/mod_positionalaudio.cpp"
    "src/positional_audio/settings_positionalaudio.h"
    "src/positional_audio/settings_positionalaudio.cpp"
    "src/positional_audio/tsvr_definitions.h"
    "src/positional_audio/tsvr_obj.h"
    "src/positional_audio/tsvr_obj.cpp"
    "src/positional_audio/tsvr_obj_self.h"
    "src/positional_audio/tsvr_obj_self.cpp"
    "src/positional_audio/tsvr_obj_other.h"
    "src/positional_audio/tsvr_obj_other.cpp"
    "src/positional_audio/tsvr_universe.h"
    "src/positional_audio/tsvr_universe.cpp"
    "src/positional_audio/shared_mem_reader.h"
    "src/positional_audio/shared_mem_reader.cpp"
    "src/positional_audio/linked_mem.h"
)
source_group("positional_audio" FILES ${POSITIONAL_AUDIO})

set (PLUGIN_SRC
    "src/banner.h"
    "src/banner.cpp"
    "src/banner_frame.h"
    "src/banner_frame.cpp"
    "src/banner_frame.ui"
    "src/config.h"
    "src/config.cpp"
    "src/config.ui"
    "src/groupbox_position_spread.h"
    "src/groupbox_position_spread.cpp"
    "src/mod_agmu.h"
    "src/mod_agmu.cpp"
    "src/mod_muter_channel.h"
    "src/mod_muter_channel.cpp"
    "src/mod_position_spread.h"
    "src/mod_position_spread.cpp"
    "src/plugin.h"
    "src/plugin.cpp"
    "src/plugin_qt.h"
    "src/plugin_qt.cpp"
    "src/settings_position_spread.h"
    "src/settings_position_spread.cpp"
    "src/simplepanner.h"
    "src/simplepanner.cpp"
    "src/snt.h"
    "src/snt.cpp"
    "src/ts_ptt_qt.h"
    "src/ts_ptt_qt.cpp"

    "crosstalk.qrc"
)
source_group("plugin" FILES ${PLUGIN_SRC})

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
#source_group("" FILES ${PLUGIN_SRC})

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set compiler flags and options. 
# Here it is setting the Visual Studio warning level to 4
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
# message ("cxx Flags:" ${CMAKE_CXX_FLAGS})

# Sub-directories where more CMakeLists.txt exist
#add_subdirectory(app)
#add_subdirectory (math)
set(WITH_VOLUME true)
set(WITH_VOLUME_WIDGETS true)
#add_subdirectory(./deps/teamspeak-plugin-qt-common common_out)
include(./deps/teamspeak-plugin-qt-common/CMakeLists.txt)

add_library(CrossTalk SHARED
    ${PLUGIN_SRC} ${TS_XX} ${TS_CORE} ${TS_QT_CORE} ${TS_QT_VOLUME} ${QT_WEBSOCKET} ${POSITIONAL_AUDIO})

set_property(TARGET CrossTalk PROPERTY CXX_STANDARD 17)
set_property(TARGET CrossTalk PROPERTY CXX_STANDARD_REQUIRED ON)
