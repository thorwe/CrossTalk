#include "config.h"

#include <QtCore/QSettings>
#include <QDialog>
#include <QWhatsThis>
#include <QDesktopServices>

#include "MMtoDB.h"
#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

const QUrl PLEDGIE_IMAGE("http://pledgie.com/campaigns/18898.png");
const QUrl PLEDGIE_CAMPAIGN("http://www.pledgie.com/campaigns/18898");
const QUrl JIANJI_CAMPAIGN("http://www.jianji.de");
const QUrl CROSSTALK_CAMPAING("http://addons.teamspeak.com/directory/plugins/miscellaneous/CrossTalk.html");
const QUrl SNT_WIKI("http://github.com/thorwe/CrossTalk/wiki/Switch'n'Talk");

//! Constructor
/*!
 * \brief Config::Config create an instance of this class
 */
Config::Config()
{
    ts = TSFunctions::instance();
    m_netwManager = new QNetworkAccessManager(this);
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
void Config::Translate(QObject* obj)
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
                trans = ts->translator->translate("Config",trans.toLocal8Bit().constData());
                if (!trans.isEmpty())
                    obj->setProperty(prop.toLocal8Bit().constData(),trans.toLocal8Bit().constData());
            }
        }
    }
}

//! Called when the widget should open
/*!
 * \brief Config::SetupUi combine with .ui, initialize
 */
void Config::SetupUi()
{
    setupUi(this);
    //pushButton_SNT->installEventFilter(this);

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup("ducker_global");
    groupBox_Duck_G->setChecked(cfg.value("enabled",true).toBool());
    cfg.endGroup();
    groupBox_Duck->setChecked(cfg.value("ducking_enabled",true).toBool());
    slider_Duck_Mode->setValue((cfg.value("ducking_reverse",false).toBool())?1:0);
    groupBox_SPS->setChecked(cfg.value("stereo_position_spread_enabled",true).toBool());
    groupBox_SPS_expert->setChecked(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
    slider_SPS_Home->setValue(cfg.value("stereo_position_spread_region_home",1).toInt());
    slider_SPS_Whisper->setValue(cfg.value("stereo_position_spread_region_whisper",2).toInt());
    slider_SPS_Other->setValue(cfg.value("stereo_position_spread_region_other",0).toInt());

    connect(groupBox_Duck_G,SIGNAL(toggled(bool)),this,SIGNAL(SetGlobalDuckerEnabled(bool)));
    connect(horizontalSlider_Duck_G,SIGNAL(valueChanged(int)),this, SLOT(onGlobalDuckerSliderValueChanged(int)));
    connect(doubleSpinBox_Duck_G,SIGNAL(valueChanged(double)), this, SLOT(onGlobalDuckerValueChanged(double)));

    connect(groupBox_Duck,SIGNAL(toggled(bool)),this,SIGNAL(SetDuckingEnabled(bool)));
    connect(horizontalSlider_Duck, SIGNAL(valueChanged(int)), this, SLOT(onDuckSliderValueChanged(int)));
    connect(doubleSpinBox_Duck, SIGNAL(valueChanged(double)), this, SLOT(onDuckValueChanged(double)));
    connect(slider_Duck_Mode,SIGNAL(valueChanged(int)),this,SLOT(onDuckModeSliderValueChanged(int)));

    connect(groupBox_SPS,SIGNAL(toggled(bool)),this,SIGNAL(SetStereoPositionSpreadEnabled(bool)));
    connect(groupBox_SPS_expert,SIGNAL(toggled(bool)),this,SIGNAL(SetStereoPositionSpreadExpertEnabled(bool)));
    connect(dial_SPS,SIGNAL(valueChanged(int)),this,SLOT(onSPSDialValueChanged(int)));
    connect(doubleSpinBox_SPS,SIGNAL(valueChanged(double)),this,SLOT(onSPSValueChanged(double)));
    connect(slider_SPS_Home,SIGNAL(valueChanged(int)),this,SIGNAL(SetStereoPositionSpreadExpertValueHome(int)));
    connect(slider_SPS_Whisper,SIGNAL(valueChanged(int)),this,SIGNAL(SetStereoPositionSpreadExpertValueWhisper(int)));
    connect(slider_SPS_Other,SIGNAL(valueChanged(int)),this,SIGNAL(SetStereoPositionSpreadExpertValueOther(int)));

    cfg.beginGroup("ducker_global");
    onGlobalDuckerValueChanged(cfg.value("value",-23.0f).toFloat());
    cfg.endGroup();
    onDuckModeSliderValueChanged((cfg.value("ducking_reverse",false).toBool())?1:0);
    onDuckValueChanged(cfg.value("ducking_value",-23.0).toDouble());
    onSPSValueChanged((cfg.value("stereo_position_spread_value",0.5).toDouble())*100.0);

    // load image
    connect(banner_pledgie,SIGNAL(onClick()),this,SLOT(onPledgieClicked()));
    connect(banner_jianji,SIGNAL(onClick()),this,SLOT(onJianjiClicked()));
    connect(banner_logo,SIGNAL(onClick()),this,SLOT(onCrossTalkClicked()));
    connect(pushButton_SNT,SIGNAL(clicked()),this,SLOT(onSnTButtonClicked()));
    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetwManagerFinished(QNetworkReply*)));
    QNetworkRequest request(PLEDGIE_IMAGE);
    m_netwManager->get(request);

    QString lang = TSHelpers::GetLanguage();
    if (lang != "de_DE") // outside of germany, use larger banner and hide jianji banner
    {
        banner_jianji->setVisible(false);
        banner_logo->setGeometry(10,10,194,37);
        QPixmap logo_int(":/banners/logo_int");
        banner_logo->setPixmap(logo_int);
    }

    if ((ts->translator != NULL) && !(ts->translator->isEmpty()))
        Translate(this);

    label_SPS_Home->setText(qApp->translate("HotkeyDialog","Current Server"));
    label_SPS_Whisper->setText(qApp->translate("WhisperSetup","Whisper"));
    label_SPS_Other->setText(qApp->translate("NotificationsSetup","Other"));
    groupBox_SPS_expert->setTitle(qApp->translate("captureSoundSetupDialog","&Advanced Options"));
    label_Duck_quiet_G->setText(qApp->translate("PlaybackSoundSetupDialog","Quiet"));
    label_Duck_loud_G->setText(qApp->translate("PlaybackSoundSetupDialog","Loud"));
    label_Duck_quiet->setText(qApp->translate("PlaybackSoundSetupDialog","Quiet"));
    label_Duck_loud->setText(qApp->translate("PlaybackSoundSetupDialog","Loud"));

    //this->update();
}

//! used to bring WhatsThis to the SNT Button
/*!
 * \brief Config::eventFilter
 * \param object we care about pushButton_SNT
 * \param ev MouseEnter, MouseLeave
 * \return
 */
//bool Config::eventFilter(QObject *object, QEvent *ev)
//{
//    if (object == pushButton_SNT)
//    {
//        if(ev->type() == QEvent::Enter)
//        {
//            QWhatsThis::enterWhatsThisMode ();
//        }
//        else if(ev->type() == QEvent::Leave)
//        {
//            if (QWhatsThis::inWhatsThisMode())
//                QWhatsThis::leaveWhatsThisMode();
//        }
//    }
//    return false;
//}

//! On Accept, write values to config
/*!
 * \brief Config::accept write values to config
 */
void Config::accept()
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup("ducker_global");
    cfg.setValue("enabled",groupBox_Duck_G->isChecked());
    cfg.setValue("value",static_cast< float >(doubleSpinBox_Duck_G->value()));
    cfg.endGroup();
    cfg.setValue("ducking_enabled",groupBox_Duck->isChecked());
    cfg.setValue("ducking_value", static_cast< float >(doubleSpinBox_Duck->value()));
    cfg.setValue("ducking_reverse",(slider_Duck_Mode->value() == 1));
    cfg.setValue("stereo_position_spread_enabled",groupBox_SPS->isChecked());
    cfg.setValue("stereo_position_spread_value",static_cast< float >((doubleSpinBox_SPS->value()) * 0.01));
    cfg.setValue("stereo_position_spread_expert_enabled",groupBox_SPS_expert->isChecked());
    cfg.setValue("stereo_position_spread_region_home",slider_SPS_Home->value());
    cfg.setValue("stereo_position_spread_region_whisper",slider_SPS_Whisper->value());
    cfg.setValue("stereo_position_spread_region_other",slider_SPS_Other->value());
	QDialog::accept();
}

//! On Cancel, read config and emit values
/*!
 * \brief Config::reject restore the config state
 */
void Config::reject()
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup("ducker_global");
    emit SetGlobalDuckerEnabled(cfg.value("enabled",true).toBool());
    emit SetGlobalDuckerValue(cfg.value("value",-23.0f).toFloat());
    cfg.endGroup();
    emit SetDuckingEnabled(cfg.value("ducking_enabled",true).toBool());
    emit SetDuckingValue(cfg.value("ducking_value",-23.0f).toFloat());
    emit SetDuckingReverse(cfg.value("ducking_reverse",false).toBool());
    emit SetStereoPositionSpreadEnabled(cfg.value("stereo_position_spread_enabled",true).toBool());
    emit SetStereoPositionSpreadValue(cfg.value("stereo_position_spread_value",0.5f).toFloat());
    emit SetStereoPositionSpreadExpertEnabled(cfg.value("stereo_position_spread_expert_enabled",false).toBool());
    emit SetStereoPositionSpreadExpertValueHome(cfg.value("stereo_position_spread_region_home",1).toInt());
    emit SetStereoPositionSpreadExpertValueWhisper(cfg.value("stereo_position_spread_region_whisper",2).toInt());
    emit SetStereoPositionSpreadExpertValueOther(cfg.value("stereo_position_spread_region_other",0).toInt());
    QDialog::reject();
}

//! Retrieves global ducking slider change, converts to dB (float), sets ducker spinbox
/*!
 * \brief Config::onGlobalDuckerSliderValueChanged Retrieves ducking slider change, converts to dB (float), sets ducker spinbox
 * \param val slider value (0-100)
 */
void Config::onGlobalDuckerSliderValueChanged(int val)
{
    double d_val = static_cast< double >(MMtoDB(static_cast< float >(val/100.)));
    d_val -= 10.0;
    d_val = d_val < -200.0 ? -200.0 : d_val;
    doubleSpinBox_Duck_G->setValue(d_val);
}

//! Retrieves ducking value changed from spinbox, sets slider if it's not the initial sender, emits SetGlobalDuckerValue
/*!
 * \brief Config::onGlobalDuckerValueChanged Qt slot to Retrieves ducking value changed from spinbox
 * \param val -200 to 0
 */
void Config::onGlobalDuckerValueChanged(double val)
{
    //sender is doubleSpinBox, ergo update slider
    if (!(horizontalSlider_Duck_G->isSliderDown()))   // loop breaker
    {
        int i_val = static_cast< int >(100*DBtoMM(static_cast< float >(val+10.0)));
        horizontalSlider_Duck_G->setValue(i_val);
    }
    emit SetGlobalDuckerValue(static_cast< float >(val));
}

//! Retrieves channel ducking slider change, converts to dB (float), sets ducker spinbox
/*!
 * \brief Config::onDuckSliderValueChanged Retrieves ducking slider change, converts to dB (float), sets ducker spinbox
 * \param val slider value (0-100)
 */
void Config::onDuckSliderValueChanged(int val)
{
    double d_val = static_cast< double >(MMtoDB(static_cast< float >(val/100.)));
    d_val -= 10.0;
    d_val = d_val < -200.0 ? -200.0 : d_val;
    doubleSpinBox_Duck->setValue(d_val);
}

//! Retrieves ducking value changed from spinbox, sets slider if it's not the initial sender, emits SetDuckingValue
/*!
 * \brief Config::onDuckValueChanged Qt slot to Retrieves ducking value changed from spinbox
 * \param val -200 to 0
 */
void Config::onDuckValueChanged(double val)
{
    //sender is doubleSpinBox, ergo update slider
    if (!(horizontalSlider_Duck->isSliderDown()))   // loop breaker
    {
        int i_val = static_cast< int >(100*DBtoMM(static_cast< float >(val+10.0)));
        horizontalSlider_Duck->setValue(i_val);
    }
    emit SetDuckingValue(static_cast< float >(val));
}

//! Retrieves ducking mode slider change, sets label, emits SetDuckingReverse as bool
/*!
 * \brief Config::onDuckModeSliderValueChanged Retrieves ducking mode slider change, sets label, emits SetDuckingReverse as bool
 * \param val slider value (0-100)
 */
void Config::onDuckModeSliderValueChanged(int val)
{
    bool isReversed = (val==1);
    if (isReversed)
    {
        QString otherServers = qApp->translate("NotificationsSetup","Other");
        otherServers.append(" ");
        otherServers.append(qApp->translate("ImprovedTabBar", "Server Tabs"));
        label_Duck_Target_Dyn->setText(otherServers);
    }
    else
        label_Duck_Target_Dyn->setText(qApp->translate("HotkeyDialog","Current Server"));

    emit SetDuckingReverse(isReversed);
}

//! Retrieves SPS value changed from dial and forwards it to spinbox
/*!
 * \brief Config::onSPSDialValueChanged Qt slot to retrieve SPS value changed from dial and forwards it to spinbox
 * \param val dial value
 */
void Config::onSPSDialValueChanged(int val)
{
    doubleSpinBox_SPS->setValue(static_cast< double > (val));
}

//! Retrieves SPS value changed from spinbox, sets slider if it's not the initial sender, emits SetStereoPositionSpreadValue
/*!
 * \brief Config::onSPSValueChanged Qt slot to retrieve SPS value changed from spinbox
 * \param val spinbox value
 */
void Config::onSPSValueChanged(double val)
{
    if (!(dial_SPS->isSliderDown()))                // loop breaker
        dial_SPS->setValue(static_cast< int >(val));

    emit SetStereoPositionSpreadValue(static_cast< float >(val * 0.01));
}

//! Retrieves the image of the pledgie campaign
/*!
 * \brief Config::onNetwManagerFinished Qt slot to retrieve the pledgie campaing image
 * \param reply a QNetworkReply containing the png data
 */
void Config::onNetwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        TSLogging::Log(reply->errorString(),LogLevel_WARNING);
        return;
    }

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    if (!(pixmap.loadFromData(jpegData)))
        TSLogging::Log("Error loading pledgie image.",LogLevel_WARNING);
    else if ((reply->url()) == PLEDGIE_IMAGE)
        banner_pledgie->setPixmap(pixmap);

    reply->deleteLater();
}

//! Receives the click of the pledgie banner
/*!
 * \brief Config::onPledgieClicked Qt slot to receive the click of the pledgie banner
 */
void Config::onPledgieClicked()
{
    QUrl url(PLEDGIE_CAMPAIGN);
    QDesktopServices::openUrl(url);
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

//! Receives the click of the crosstalk banner
/*!
 * \brief Config::onCrossTalkClicked Qt slot to receive the click of the crosstalk banner
 */
void Config::onCrossTalkClicked()
{
    QUrl url(CROSSTALK_CAMPAING);
    QDesktopServices::openUrl(url);
}

void Config::onSnTButtonClicked()
{
    QDesktopServices::openUrl(SNT_WIKI);
}
