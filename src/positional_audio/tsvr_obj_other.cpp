#include "tsvr_obj_other.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"
#include "core/ts_logging_qt.h"
#include "teamspeak/public_errors.h"

TsVrObjOther::TsVrObjOther(QObject *parent, uint64 connection_id, anyID client_id)
    : m_connection_id(connection_id)
    , m_client_id(client_id)
{
    this->setParent(parent);
    reset_avatar();

    const auto [error_client_uid, client_uid] =
    com::teamspeak::pluginsdk::funcs::get_client_property_as_string(connection_id, client_id,
                                                                    CLIENT_UNIQUE_IDENTIFIER);
    if (error_client_uid)
    {
        TSLogging::Error("(TsVrObjOther::TsVrObjOther)", connection_id, error_client_uid, true);
    }
    else
        m_client_uid = client_uid;
}

void TsVrObjOther::set_avatar(const thorwe::Three_Dee_Info &value)
{
    m_avatar = value;
}
