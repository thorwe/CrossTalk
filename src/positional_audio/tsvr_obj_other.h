#pragma once

#include "tsvr_obj.h"

#include "core/definitions.h"

#include <QtCore/QObject>

#include <string>

class TsVrObjOther : public TsVrObj
{
    Q_OBJECT

  public:
    explicit TsVrObjOther(QObject *parent = 0, uint64 connection_id = 0, anyID client_id = 0);

    uint64 get_connection_id() const { return m_connection_id; };
    anyID get_client_id() const { return m_client_id; }
    std::string get_client_uid() const { return m_client_uid; }

    void set_avatar(const thorwe::Three_Dee_Info &);

  private:
    const uint64 m_connection_id;
    const anyID m_client_id;
    std::string m_client_uid;
};
