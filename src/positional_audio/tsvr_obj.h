#pragma once

#include "teamspeak/public_definitions.h"
#include <QtCore/QObject>
#include <QtCore/QTextStream>

#include "definitions_positionalaudio.h"
#include "tsvr_definitions.h"

#include <string_view>

#include <string>
#include <vector>

class TsVrObj : public QObject
{
    Q_OBJECT

  public:
    std::wstring get_vr() const;
    std::wstring get_vr_description() const;
    std::vector<std::byte> get_context() const;
    std::string get_context_as_string() const;
    std::wstring getIdentity() const;
    std::wstring getIdentityRaw() const;
    thorwe::Three_Dee_Info get_avatar() const;

    bool onInfoDataChanged(QTextStream &data);

    void setCustomEnvironmentSupport(QObject *val);

    void set_vr(std::wstring_view val);
    void set_vr_description(std::wstring_view val = L"");
    void set_context(const std::vector<std::byte> &);
    void setIdentityRaw(std::wstring_view val);
    void reset_avatar();

  protected:
    thorwe::Three_Dee_Info m_avatar;

  private:
    std::wstring m_vr;
    std::wstring m_vr_desc;
    std::vector<std::byte> m_context;
    std::wstring m_identityRaw;

    QObject *m_CustomEnvironmentSupport = nullptr;
};

bool operator==(const TS3_VECTOR &veca, const TS3_VECTOR &vecb);
