#ifndef TS_MISSING_DEFINITIONS_H
#define TS_MISSING_DEFINITIONS_H

enum PermissionGroupDatabaseTypes {
    PermGroupDBTypeTemplate = 0, // 0: template group     (used for new virtual servers)
    PermGroupDBTypeRegular, // 1: regular group      (used for regular clients)
    PermGroupDBTypeQuery // 2: global query group (used for ServerQuery clients)
};

enum PermissionGroupTypes {
    PermGroupTypeServerGroup = 0, // 0: server group permission
    PermGroupTypeGlobalClient, // 1: client specific permission
    PermGroupTypeChannel, // 2: channel specific permission
    PermGroupTypeChannelGroup, // 3: channel group permission
    PermGroupTypeChannelClient // 4: channel-client specific permission
};

enum TokenType {
    TokenServerGroup = 0, // 0: server group token (id1={groupID} id2=0)
    TokenChannelGroup // 1: channel group token (id1={groupID} id2={channelID})
};

#endif // TS_MISSING_DEFINITIONS_H
