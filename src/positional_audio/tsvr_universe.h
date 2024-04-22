#pragma once

#include "tsvr_obj_other.h"

#include "core/client_storage.h"
#include "core/definitions.h"

#include "teamspeak/public_definitions.h"

#include <QtCore/QObject>

#include <tuple>

namespace thorwe
{

using com::teamspeak::client_id_t;
using com::teamspeak::connection_id_t;

class TsVrUniverse final : public QObject, public Safe_Client_Storage<TsVrObjOther>
{
    Q_OBJECT

  public:
    // explicit TsVrUniverse(QObject *parent = 0);

    std::pair<TsVrObjOther *, bool> add_obj(connection_id_t connection_id, client_id_t client_id)
    {
        return add_item(connection_id, client_id,
                        std::make_unique<TsVrObjOther>(this, connection_id, client_id));
    }

    // returns remove success & the client_uid of the removed obj
    std::tuple<bool, std::string> remove(connection_id_t connection_id, client_id_t client_id);

    void onConnectStatusChanged(connection_id_t connection_id, int new_status, unsigned int /*error_number*/)
    {
        if (ConnectStatus::STATUS_DISCONNECTED == new_status)
            delete_items(connection_id);
    }
};

}  // namespace thorwe
