#include "tsvr_universe.h"

namespace thorwe
{

/*TsVrUniverse::TsVrUniverse(QObject *parent) :
    QObject(parent)
{
}*/

//! Remove a specific object
/*!
 * \brief TsVrUniverse::Remove Helper function
 * \param connection_id the connection id of the server
 * \param clientID the client id on the current tab
 */
auto TsVrUniverse::remove(connection_id_t connection_id, client_id_t client_id)
-> std::tuple<bool, std::string>
{
    const auto client_uid = do_for(
    [](TsVrObjOther *obj) { return obj ? obj->get_client_uid() : std::string{}; }, connection_id, client_id);

    const auto was_removed = delete_item(connection_id, client_id);
    return {was_removed, client_uid};
}

}  // namespace thorwe
