/********************************************************************************
** Form generated from reading UI file 'config.ui'
**
** Created: Thu Dec 20 01:51:52 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>
#include "banner.h"

QT_BEGIN_NAMESPACE

class Ui_Config
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_SPS;
    QDial *dial_SPS;
    QDoubleSpinBox *doubleSpinBox_SPS;
    QGroupBox *groupBox_SPS_expert;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_SPS_Home;
    QLabel *label_SPS_Whisper;
    QLabel *label_SPS_Other;
    QSlider *slider_SPS_Home;
    QSlider *slider_SPS_Whisper;
    QSlider *slider_SPS_Other;
    QGroupBox *groupBox_Duck;
    QSlider *horizontalSlider_Duck;
    QDoubleSpinBox *doubleSpinBox_Duck;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *label_Duck_quiet;
    QLabel *label_Duck_loud;
    QSlider *slider_Duck_Mode;
    QLabel *label_Duck_Target_Dyn;
    QLabel *label_Duck_Target_Fix;
    QPushButton *pushButton_SNT;
    Banner *banner_pledgie;
    Banner *banner_jianji;
    Banner *banner_logo;
    QFrame *frame;

    void setupUi(QDialog *Config)
    {
        if (Config->objectName().isEmpty())
            Config->setObjectName(QString::fromUtf8("Config"));
        Config->resize(370, 460);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Config->sizePolicy().hasHeightForWidth());
        Config->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("Times New Roman"));
        Config->setFont(font);
        Config->setWindowTitle(QString::fromUtf8("CrossTalk"));
        Config->setStyleSheet(QString::fromUtf8(""));
        Config->setModal(true);
        buttonBox = new QDialogButtonBox(Config);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(60, 423, 301, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox_SPS = new QGroupBox(Config);
        groupBox_SPS->setObjectName(QString::fromUtf8("groupBox_SPS"));
        groupBox_SPS->setGeometry(QRect(45, 80, 281, 191));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Times New Roman"));
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setWeight(50);
        groupBox_SPS->setFont(font1);
        groupBox_SPS->setAutoFillBackground(true);
        groupBox_SPS->setStyleSheet(QString::fromUtf8(""));
        groupBox_SPS->setFlat(false);
        groupBox_SPS->setCheckable(true);
        groupBox_SPS->setChecked(false);
        dial_SPS = new QDial(groupBox_SPS);
        dial_SPS->setObjectName(QString::fromUtf8("dial_SPS"));
        dial_SPS->setGeometry(QRect(17, 24, 151, 71));
        dial_SPS->setAutoFillBackground(false);
        dial_SPS->setNotchesVisible(true);
        doubleSpinBox_SPS = new QDoubleSpinBox(groupBox_SPS);
        doubleSpinBox_SPS->setObjectName(QString::fromUtf8("doubleSpinBox_SPS"));
        doubleSpinBox_SPS->setGeometry(QRect(170, 30, 62, 22));
        groupBox_SPS_expert = new QGroupBox(groupBox_SPS);
        groupBox_SPS_expert->setObjectName(QString::fromUtf8("groupBox_SPS_expert"));
        groupBox_SPS_expert->setGeometry(QRect(9, 100, 261, 81));
        groupBox_SPS_expert->setTitle(QString::fromUtf8("Expert"));
        groupBox_SPS_expert->setCheckable(true);
        groupBox_SPS_expert->setChecked(false);
        layoutWidget = new QWidget(groupBox_SPS_expert);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 20, 241, 60));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(12);
        gridLayout->setVerticalSpacing(6);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_SPS_Home = new QLabel(layoutWidget);
        label_SPS_Home->setObjectName(QString::fromUtf8("label_SPS_Home"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_SPS_Home->sizePolicy().hasHeightForWidth());
        label_SPS_Home->setSizePolicy(sizePolicy1);
        label_SPS_Home->setFrameShape(QFrame::NoFrame);
        label_SPS_Home->setText(QString::fromUtf8("Home"));
        label_SPS_Home->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(label_SPS_Home, 0, 0, 1, 1);

        label_SPS_Whisper = new QLabel(layoutWidget);
        label_SPS_Whisper->setObjectName(QString::fromUtf8("label_SPS_Whisper"));
        sizePolicy1.setHeightForWidth(label_SPS_Whisper->sizePolicy().hasHeightForWidth());
        label_SPS_Whisper->setSizePolicy(sizePolicy1);
        label_SPS_Whisper->setText(QString::fromUtf8("Whisper"));
        label_SPS_Whisper->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(label_SPS_Whisper, 0, 1, 1, 1);

        label_SPS_Other = new QLabel(layoutWidget);
        label_SPS_Other->setObjectName(QString::fromUtf8("label_SPS_Other"));
        sizePolicy1.setHeightForWidth(label_SPS_Other->sizePolicy().hasHeightForWidth());
        label_SPS_Other->setSizePolicy(sizePolicy1);
        label_SPS_Other->setText(QString::fromUtf8("Other"));
        label_SPS_Other->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(label_SPS_Other, 0, 2, 1, 1);

        slider_SPS_Home = new QSlider(layoutWidget);
        slider_SPS_Home->setObjectName(QString::fromUtf8("slider_SPS_Home"));
        slider_SPS_Home->setMaximum(2);
        slider_SPS_Home->setPageStep(1);
        slider_SPS_Home->setOrientation(Qt::Horizontal);
        slider_SPS_Home->setTickPosition(QSlider::TicksAbove);

        gridLayout->addWidget(slider_SPS_Home, 1, 0, 1, 1);

        slider_SPS_Whisper = new QSlider(layoutWidget);
        slider_SPS_Whisper->setObjectName(QString::fromUtf8("slider_SPS_Whisper"));
        slider_SPS_Whisper->setMaximum(2);
        slider_SPS_Whisper->setPageStep(1);
        slider_SPS_Whisper->setOrientation(Qt::Horizontal);
        slider_SPS_Whisper->setTickPosition(QSlider::TicksAbove);

        gridLayout->addWidget(slider_SPS_Whisper, 1, 1, 1, 1);

        slider_SPS_Other = new QSlider(layoutWidget);
        slider_SPS_Other->setObjectName(QString::fromUtf8("slider_SPS_Other"));
        slider_SPS_Other->setMaximum(2);
        slider_SPS_Other->setPageStep(1);
        slider_SPS_Other->setOrientation(Qt::Horizontal);
        slider_SPS_Other->setTickPosition(QSlider::TicksAbove);

        gridLayout->addWidget(slider_SPS_Other, 1, 2, 1, 1);

        groupBox_Duck = new QGroupBox(Config);
        groupBox_Duck->setObjectName(QString::fromUtf8("groupBox_Duck"));
        groupBox_Duck->setGeometry(QRect(45, 290, 281, 121));
        groupBox_Duck->setFont(font1);
        groupBox_Duck->setAutoFillBackground(true);
        groupBox_Duck->setCheckable(true);
        groupBox_Duck->setChecked(false);
        horizontalSlider_Duck = new QSlider(groupBox_Duck);
        horizontalSlider_Duck->setObjectName(QString::fromUtf8("horizontalSlider_Duck"));
        horizontalSlider_Duck->setGeometry(QRect(10, 69, 191, 31));
        horizontalSlider_Duck->setCursor(QCursor(Qt::ArrowCursor));
        horizontalSlider_Duck->setContextMenuPolicy(Qt::NoContextMenu);
        horizontalSlider_Duck->setMinimum(0);
        horizontalSlider_Duck->setMaximum(10000);
        horizontalSlider_Duck->setPageStep(1000);
        horizontalSlider_Duck->setOrientation(Qt::Horizontal);
        horizontalSlider_Duck->setTickPosition(QSlider::TicksBothSides);
        horizontalSlider_Duck->setTickInterval(10000);
        doubleSpinBox_Duck = new QDoubleSpinBox(groupBox_Duck);
        doubleSpinBox_Duck->setObjectName(QString::fromUtf8("doubleSpinBox_Duck"));
        doubleSpinBox_Duck->setGeometry(QRect(210, 71, 62, 22));
        doubleSpinBox_Duck->setMinimum(-200);
        doubleSpinBox_Duck->setMaximum(0);
        layoutWidget1 = new QWidget(groupBox_Duck);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 100, 191, 17));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_Duck_quiet = new QLabel(layoutWidget1);
        label_Duck_quiet->setObjectName(QString::fromUtf8("label_Duck_quiet"));
        QFont font2;
        font2.setPointSize(7);
        label_Duck_quiet->setFont(font2);
        label_Duck_quiet->setText(QString::fromUtf8("quiet"));

        horizontalLayout->addWidget(label_Duck_quiet);

        label_Duck_loud = new QLabel(layoutWidget1);
        label_Duck_loud->setObjectName(QString::fromUtf8("label_Duck_loud"));
        label_Duck_loud->setFont(font2);
        label_Duck_loud->setText(QString::fromUtf8("loud"));
        label_Duck_loud->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_Duck_loud);

        slider_Duck_Mode = new QSlider(groupBox_Duck);
        slider_Duck_Mode->setObjectName(QString::fromUtf8("slider_Duck_Mode"));
        slider_Duck_Mode->setGeometry(QRect(55, 30, 31, 24));
        sizePolicy.setHeightForWidth(slider_Duck_Mode->sizePolicy().hasHeightForWidth());
        slider_Duck_Mode->setSizePolicy(sizePolicy);
        slider_Duck_Mode->setMaximum(1);
        slider_Duck_Mode->setPageStep(1);
        slider_Duck_Mode->setOrientation(Qt::Horizontal);
        slider_Duck_Mode->setTickPosition(QSlider::TicksBelow);
        slider_Duck_Mode->setTickInterval(1);
        label_Duck_Target_Dyn = new QLabel(groupBox_Duck);
        label_Duck_Target_Dyn->setObjectName(QString::fromUtf8("label_Duck_Target_Dyn"));
        label_Duck_Target_Dyn->setGeometry(QRect(100, 30, 151, 13));
        QFont font3;
        font3.setItalic(true);
        label_Duck_Target_Dyn->setFont(font3);
        label_Duck_Target_Dyn->setAutoFillBackground(false);
        label_Duck_Target_Dyn->setFrameShape(QFrame::NoFrame);
        label_Duck_Target_Dyn->setText(QString::fromUtf8("TextLabel"));
        label_Duck_Target_Fix = new QLabel(groupBox_Duck);
        label_Duck_Target_Fix->setObjectName(QString::fromUtf8("label_Duck_Target_Fix"));
        label_Duck_Target_Fix->setGeometry(QRect(0, 30, 46, 13));
        label_Duck_Target_Fix->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButton_SNT = new QPushButton(Config);
        pushButton_SNT->setObjectName(QString::fromUtf8("pushButton_SNT"));
        pushButton_SNT->setGeometry(QRect(60, 420, 75, 23));
        pushButton_SNT->setMouseTracking(false);
        banner_pledgie = new Banner(Config);
        banner_pledgie->setObjectName(QString::fromUtf8("banner_pledgie"));
        banner_pledgie->setGeometry(QRect(210, 10, 149, 37));
        sizePolicy.setHeightForWidth(banner_pledgie->sizePolicy().hasHeightForWidth());
        banner_pledgie->setSizePolicy(sizePolicy);
        banner_pledgie->setAlignment(Qt::AlignCenter);
        banner_pledgie->setOpenExternalLinks(true);
        banner_jianji = new Banner(Config);
        banner_jianji->setObjectName(QString::fromUtf8("banner_jianji"));
        banner_jianji->setGeometry(QRect(10, 10, 89, 37));
        sizePolicy.setHeightForWidth(banner_jianji->sizePolicy().hasHeightForWidth());
        banner_jianji->setSizePolicy(sizePolicy);
        banner_jianji->setPixmap(QPixmap(QString::fromUtf8(":/banners/jianji")));
        banner_jianji->setAlignment(Qt::AlignCenter);
        banner_jianji->setOpenExternalLinks(true);
        banner_logo = new Banner(Config);
        banner_logo->setObjectName(QString::fromUtf8("banner_logo"));
        banner_logo->setGeometry(QRect(107, 10, 97, 37));
        sizePolicy.setHeightForWidth(banner_logo->sizePolicy().hasHeightForWidth());
        banner_logo->setSizePolicy(sizePolicy);
        banner_logo->setPixmap(QPixmap(QString::fromUtf8(":/banners/logo_deDE")));
        banner_logo->setAlignment(Qt::AlignCenter);
        banner_logo->setOpenExternalLinks(true);
        frame = new QFrame(Config);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 371, 61));
        frame->setStyleSheet(QString::fromUtf8("background-color: rgb(232, 232, 232);"));
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Raised);
        frame->raise();
        buttonBox->raise();
        groupBox_SPS->raise();
        groupBox_Duck->raise();
        pushButton_SNT->raise();
        banner_pledgie->raise();
        banner_jianji->raise();
        banner_logo->raise();

        retranslateUi(Config);
        QObject::connect(buttonBox, SIGNAL(accepted()), Config, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Config, SLOT(reject()));

        QMetaObject::connectSlotsByName(Config);
    } // setupUi

    void retranslateUi(QDialog *Config)
    {
#ifndef QT_NO_TOOLTIP
        groupBox_SPS->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        groupBox_SPS->setWhatsThis(QApplication::translate("Config", "<html><head/><body><p><span style=\" font-weight:600; text-decoration: underline;\">Stereo Position Spread</span></p><p>When multiple people are talking at the same time, which tends to happen, they are harder to understand than necessary.</p><p>First, our brain is a lot better at decyphering those noises into language if they are located at seperate positions.</p><p>Second, the Teamspeak client is apparently mixing down at 16 bit. In consequence, when two or more people are talking, the mixer will not have the headroom to keep things from distorting.</p><p>This module shall help on both of these issues.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        groupBox_SPS->setTitle(QApplication::translate("Config", "Stereo Position Spread", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        dial_SPS->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        dial_SPS->setWhatsThis(QApplication::translate("Config", "<html><head/><body><p><span style=\" font-size:10pt; text-decoration: underline;\">Position Spread: Width</span></p><p>Use low values to keep speakers closer together, increase it to set them further apart.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        groupBox_SPS_expert->setWhatsThis(QApplication::translate("Config", "Reserve regions in the stereo field for specific types of incoming audio streams.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        label_SPS_Home->setWhatsThis(QApplication::translate("Config", "Audio Stream: Clients talking on the currently active server tab.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        label_SPS_Whisper->setWhatsThis(QApplication::translate("Config", "Audio Stream: Any incoming whispers.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        label_SPS_Other->setWhatsThis(QApplication::translate("Config", "Audio Stream: Clients talking on currently deselected server tabs.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        slider_SPS_Home->setWhatsThis(QApplication::translate("Config", "Audio Region: Left, Middle, Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        slider_SPS_Whisper->setWhatsThis(QApplication::translate("Config", "Audio Region: Left, Middle, Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        slider_SPS_Other->setWhatsThis(QApplication::translate("Config", "Audio Region: Left, Middle, Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        groupBox_Duck->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        groupBox_Duck->setWhatsThis(QApplication::translate("Config", "<html><head/><body><p><span style=\" font-weight:600; text-decoration: underline;\">Home Channel Ducking</span></p><p><br/></p><p><span style=\" text-decoration: underline;\">In short:</span></p><p>Reduces the volume of speakers in the channel of the currently active server tab as soon as a whisper comes in or someone talks on another server tab (in common scenarios this is some kind of commander channel).</p><p><span style=\" text-decoration: underline;\">Longer version:</span></p><p>In case you haven't heard of ducking, let's start with that.</p><p>Ducking is the reduction of a target channels volume dependant on the activity on a source channel. Usually, the actual sound volume of the source channel is analyzed to modify the reduction of the target channel. Here, however, keeping in mind we don't want to either sound or reaction latency, the talk status is used as trigger, the reduction solely determined by the setting.</p><p>So now, the only question left would be: What's the source? What's the target?</p>"
                        "<p>Target: Your &quot;home&quot; channel. That's what I call the channel of your currently active server tab.</p><p>Sources: Whispers and everything that's coming in from another server tab.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        groupBox_Duck->setTitle(QApplication::translate("Config", "Channel Ducking", 0, QApplication::UnicodeUTF8));
        label_Duck_Target_Fix->setText(QApplication::translate("Config", "Target", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        pushButton_SNT->setWhatsThis(QApplication::translate("Config", "<html><head/><body><p><span style=\" font-weight:600; text-decoration: underline;\">Setup:</span></p><p>Options-&gt; Hotkeys-&gt; Add -&gt;</p><p>1) Set HotKey with &quot;On key down&quot; :</p><p>Show Advanced Options -&gt; Plugins -&gt; Plugin Hotkey -&gt; CrossTalk -&gt; Next Tab and Talk Start.</p><p>2) Set same HotKey with &quot;On key up&quot;:</p><p>Show Advanced Options -&gt; Plugins -&gt; Plugin Hotkey -&gt; CrossTalk -&gt; Next Tab and Talk End.</p><p><br/></p><p>For <span style=\" font-weight:600;\">whispering</span>, I've provided one often used variant, whispering to all channelcommander on all channels. Setup is equivalent. If you need a more specific whisper list, you have to use &quot;Plugin Command&quot; for the &quot;On key down&quot; action.</p><p><span style=\" color:#008000;\">TS3_NEXT_TAB_AND_WHISPER_START</span></p><p><span style=\" color:#000000;\">is the basic command, which takes two arguments:</span></p><p><span style=\" color:#000000;\">1) </span><span style=\" color:#800080;\">Grou"
                        "pWhisperType:</span></p><p><span style=\" font-family:'Courier New,courier'; color:#800080;\">CHANNELCOMMANDER</span><span style=\" font-family:'Courier New,courier'; color:#000000;\"> or </span><span style=\" font-family:'Courier New,courier'; color:#800080;\">ALLCLIENTS</span></p><p><span style=\" font-family:'Courier New,courier'; color:#000000;\">2)</span><span style=\" color:#800080;\">GroupWhisperTargetMode</span></p><p><span style=\" font-family:'Courier New,courier'; color:#800080;\">ALL CURRENTCHANNEL ALLPARENTCHANNELS SUBCHANNELS PARENTCHANNEL CHANNELFAMILYANCESTORCHANNELFAMILY</span></p><p><span style=\" font-family:'Courier New,courier'; color:#000000;\">So, the equivalent for the provided hotkey would be: </span><span style=\" color:#008000;\">TS3_NEXT_TAB_AND_WHISPER_START </span><span style=\" font-family:'Courier New,courier'; color:#800080;\">CHANNELCOMMANDER ALL</span></p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        pushButton_SNT->setText(QApplication::translate("Config", "Switch'n'Talk", 0, QApplication::UnicodeUTF8));
        banner_pledgie->setText(QApplication::translate("Config", "<html><head/><body><p><a href=\"http://www.pledgie.com/campaigns/15624\"><span style=\" text-decoration: underline; color:#0000ff;\">Support the author!</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
        banner_jianji->setText(QString());
        banner_logo->setText(QString());
        Q_UNUSED(Config);
    } // retranslateUi

};

namespace Ui {
    class Config: public Ui_Config {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIG_H
