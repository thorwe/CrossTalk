#pragma once

#include <QtCore/QString>
#include <QtCore/QTextStream>

#include <string_view>

class CustomEnvironmentSupportInterface
{
  public:
    virtual bool on_identity_raw_dirty(std::wstring_view raw_identity) = 0;
    virtual std::wstring get_identity() const = 0;
    virtual bool onInfoData(QTextStream &data) = 0;
};
Q_DECLARE_INTERFACE(CustomEnvironmentSupportInterface,"net.thorwe.CrossTalk.CustomEnvironmentSupportInterface/1.0")
