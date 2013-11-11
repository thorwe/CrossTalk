#include "config.h"

#include <QtCore/QSettings>
#include <QDialog>
#include <QWhatsThis>
#include <QDesktopServices>

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

const QUrl JIANJI_CAMPAIGN("http://www.jianji.de");

//! Constructor
/*!
 * \brief Config::Config create an instance of this class
 */
Config::Config()
{
//    loca = Translator::instance();
}

//! Destructor
/*!
 * \brief Config::~Config Destroys the class instance
 */
Config::~Config()
{
}

//! Translate all ui elements
/*!
 * \brief Config::Translate called initially with this, reversing
 * \param obj this
 */
/*void Config::Translate(QObject* obj)
{
    const QObjectList list = obj->children();
    if (!list.isEmpty())
    {
        for (int i=0;i<list.size();++i)
        {
            Translate(list[i]);
        }
    }

    QVariant test;
    QStringList props;
    props << "text" << "whatsThis" << "title";

    for (int j=0;j<props.size();++j)
    {
        QString prop = props[j];
        test = obj->property(prop.toLocal8Bit().constData());
        if (test.isValid())
        {
            QString trans = test.toString();
            if (!trans.isEmpty())
            {
                trans = loca->translator->translate("Config",trans.toLocal8Bit().constData());
                if (!trans.isEmpty())
                    obj->setProperty(prop.toLocal8Bit().constData(),trans.toLocal8Bit().constData());
            }
        }
    }
}*/

//! Called when the widget should open
/*!
 * \brief Config::SetupUi combine with .ui, initialize
 */
void Config::SetupUi()
{
    setupUi(this);
    this->setFixedSize(this->width(),this->height());
    this->setAttribute( Qt::WA_DeleteOnClose );

    // load image
    connect(banner_jianji,SIGNAL(onClick()),this,SLOT(onJianjiClicked()));

    QString lang = TSHelpers::GetLanguage();
    if (lang != "de_DE") // outside of german locale, hide jianji banner
        banner_jianji->setVisible(false);

//    if ((loca->translator != NULL) && !(loca->translator->isEmpty()))
//        Translate(this);
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    betaChannelcheckBox->setChecked(cfg.value("beta",false).toBool());
    connect(betaChannelcheckBox,SIGNAL(toggled(bool)),SLOT(onBetaChannelToggled(bool)));

    bool ok;
    quint16 port = cfg.value("server_port",8080).toUInt(&ok);
    if (!ok)
    {
        TSLogging::Error("Could not read port from settings");
        groupBox_Server->setChecked(false);
    }
    else
    {
        spinBox_Server->setValue(port);
        connect(spinBox_Server,SIGNAL(valueChanged(int)),this,SLOT(onServerPortChanged(int)));
        groupBox_Server->setChecked(cfg.value("server_enabled",false).toBool());
        connect(groupBox_Server,SIGNAL(toggled(bool)),this,SLOT(onServerEnabledToggled(bool)));
    }
}

//! Receives the click of the jianji banner
/*!
 * \brief Config::onJianjiClicked Qt slot to receive the click of the jianji banner
 */
void Config::onJianjiClicked()
{
    QUrl url(JIANJI_CAMPAIGN);
    QDesktopServices::openUrl(url);
}

void Config::onBetaChannelToggled(bool val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("beta",val);
    emit betaChannelToggled(val);
}

void Config::onServerEnabledToggled(bool val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("server_enabled",val);
    emit serverEnabledToggled(val);
}

void Config::onServerPortChanged(int val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("server_port",(quint16)val);
    emit serverPortChanged((quint16)val);
}
