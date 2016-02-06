#ifndef VERSIONQT_H
#define VERSIONQT_H

#include <QObject>
#include <QString>

class VersionQt : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int major
               READ major)
    Q_PROPERTY(int minor
               READ minor)
    Q_PROPERTY(int revision
               READ revision)
    Q_PROPERTY(int build
               READ build)
    Q_PROPERTY(bool valid
               READ valid)
public:
    explicit VersionQt(QObject *parent = 0, QString versionStr = "0.0.0.0");

    int major() const;
    int minor() const;
    int revision() const;
    int build() const;
    bool valid() const;

    QString toString() const;

    bool operator< (const VersionQt& other) const;
    bool operator== (const VersionQt& other) const;

private:
    int m_major = 0;
    int m_minor = 0;
    int m_revision = 0;
    int m_build = 0;
    bool m_isValid = true;
    QString m_versionString;
};

#endif // VERSIONQT_H
