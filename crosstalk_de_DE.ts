<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="de_DE">
<context>
    <name>Config</name>
    <message>
        <location filename="src/config.ui" line="70"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Stereo Position Spread&lt;/span&gt;&lt;/p&gt;&lt;p&gt;When multiple people are talking at the same time, which tends to happen, they are harder to understand than necessary.&lt;/p&gt;&lt;p&gt;First, our brain is a lot better at decyphering those noises into language if they are located at seperate positions.&lt;/p&gt;&lt;p&gt;Second, the Teamspeak client is apparently mixing down at 16 bit. In consequence, when two or more people are talking, the mixer will not have the headroom to keep things from distorting.&lt;/p&gt;&lt;p&gt;This module shall help on both of these issues.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Stereo Position Spread&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Wenn mehrere Leute gleichzeitig sprechen, was tendenziell passiert, sind sie schwerer zu verstehen als nötig.&lt;/p&gt;&lt;p&gt;Erstens ist unser Gehirn weitaus besser darin, diese Geräuche zu Sprache zu dechiffrieren, wenn diese in der räumlichen Wahrnehmung getrennt sind.&lt;/p&gt;&lt;p&gt;Zweitens mixt der Teamspeak Klient mit 16 bit. Daraus folgt, dass wenn zwei oder mehr Leute gleichzeitig sprechen, der Mixer nicht mehr genug Übersteuerungsreserven hat - und damit verzerrt.&lt;/p&gt;&lt;p&gt;Dieses Modul soll hier helfen.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="79"/>
        <source>Stereo Position Spread</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/config.ui" line="103"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:10pt; text-decoration: underline;&quot;&gt;Position Spread: Width&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Use low values to keep speakers closer together, increase it to set them further apart.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:10pt; text-decoration: underline;&quot;&gt;Position Spread: Breite&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Benutze niedrige Werte um die Sprecher näher beieinander zu positionieren, erhöhe ihn um sie weiter auseinander zu schieben.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="132"/>
        <source>Reserve regions in the stereo field for specific types of incoming audio streams.</source>
        <translation>Reserviere Regionen im Stereo Feld für spezifische Arten von einkommenden Audio Streams.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="168"/>
        <source>Audio Stream: Clients talking on the currently active server tab.</source>
        <translation>Audio Stream: Klienten, die auf dem derzeit aktiven Server Reiter sprechen.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="190"/>
        <source>Audio Stream: Any incoming whispers.</source>
        <translation>Audio Stream: Sämtliche einkommenden Whispers.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="209"/>
        <source>Audio Stream: Clients talking on currently deselected server tabs.</source>
        <translation>Audio Stream: Klienten, die auf den nicht selektierten Server Reitern sprechen.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="222"/>
        <location filename="src/config.ui" line="241"/>
        <location filename="src/config.ui" line="260"/>
        <source>Audio Region: Left, Middle, Right</source>
        <translation>Audio Region: Links, Mitte, Rechts</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="420"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Ducking is the reduction of a target channels volume dependant on the activity on a source channel. Usually, the actual sound volume of the source channel is analyzed to modify the reduction of the target channel. Here, however, keeping in mind we don&apos;t want to either sound or reaction latency, the talk status is used as trigger, the reduction solely determined by the setting.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Ducking bedeutet die Reduktion der Lautstärke eines Zielkanals basierend auf der Aktivität eines Steuerkanals. Üblicherweise wird die Lautstärke des Steuerkanals analysiert, um den Grad der Reduktion des Zielkanals zu bestimmen. Hier allerdings, unter Rücksichtnahme darauf, dass weder Ton noch die Reaktionszeit verzögert werden sollen, wird ausschließlich der Redestatus als Auslöser genutzt und die Reduktion durch das Setting bestimmt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="455"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The Global (Music Bots, Very Unimportant Persons) Ducking module enables you to add global ducking targets via the clients context menu.&lt;/p&gt;&lt;p&gt;Those will get ducked by any other audio stream, no matter the origin.&lt;/p&gt;&lt;p&gt;Global Ducking Targets are ignored by the Channel Ducking module.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Das Global (Music Bots, Very Unimportant Persons) Ducking Modul ermöglicht es, globale Ducking Ziele mittels dem Kontextmenu des Teamspeak Klienten zu bestimmen.&lt;/p&gt;&lt;p&gt;Diese werden durch alle anderen Audio Streams geduckt, egal welchen Ursprungs.&lt;/p&gt;&lt;p&gt;Globale Ducking Ziele werden im Channel Ducking Modul ignoriert.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="580"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Home Channel Ducking&lt;/span&gt;&lt;/p&gt;&lt;p&gt;To find out about Ducking, whatsThis? the Ducking module.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;In short:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Per default reduces the volume of speakers in the channel of the currently active server tab as soon as a whisper comes in or someone talks on another server tab (in common scenarios this is some kind of commander channel).&lt;/p&gt;&lt;p&gt;Target: Your &amp;quot;home&amp;quot; channel. That&apos;s what I call the channel of your currently active server tab.&lt;/p&gt;&lt;p&gt;Sources: Whispers and everything that&apos;s coming in from another server tab.&lt;/p&gt;&lt;p&gt;You can switch the target in so far, that:&lt;/p&gt;&lt;p&gt;Target: Talking on another server tab&lt;/p&gt;&lt;p&gt;Sources: Your &amp;quot;home&amp;quot; channel and any whispers&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Home Channel Ducking&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Um herauszufinden, was Ducking überhaupt bedeutet, whatsThis? das Ducking Modul.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;In Kürze:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Per default wird die Lautstärke von Sprechern im Channel des derzeit aktiven Server Tabs reduziert, sobald man angewhispert wird oder jemand auf einem anderen Server Tab spricht (in üblichen Szenarios bspw. ein Commander Channel).&lt;/p&gt;&lt;p&gt;Ziel: Dein &amp;quot;home&amp;quot; Channel, der Channel deines derzeit aktiven Server Tabs&lt;/p&gt;&lt;p&gt;Ursprung/Steuerung: Whispers und alle Audio Streams von einem anderen Server Tab.&lt;/p&gt;&lt;p&gt;Du kannst das Ziel insofern umschalten, dass:&lt;/p&gt;&lt;p&gt;Ziel: Sprecher auf einem anderen Server Tab.&lt;/p&gt;&lt;p&gt;Ursprung/Steuerung: Dein &amp;quot;home&amp;quot; Channel und jegliche Whispers&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="586"/>
        <source>Channel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/config.ui" line="748"/>
        <source>Target</source>
        <translation>Ziel</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="293"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Setup:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Options-&amp;gt; Hotkeys-&amp;gt; Add -&amp;gt;&lt;/p&gt;&lt;p&gt;1) Set HotKey with &amp;quot;On key down&amp;quot; :&lt;/p&gt;&lt;p&gt;Show Advanced Options -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk Start.&lt;/p&gt;&lt;p&gt;2) Set same HotKey with &amp;quot;On key up&amp;quot;:&lt;/p&gt;&lt;p&gt;Show Advanced Options -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk End.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;For &lt;span style=&quot; font-weight:600;&quot;&gt;whispering&lt;/span&gt;, I&apos;ve provided one often used variant, whispering to all channelcommander on all channels. Setup is equivalent. If you need a more specific whisper list, you have to use &amp;quot;Plugin Command&amp;quot; for the &amp;quot;On key down&amp;quot; action.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;is the basic command, which takes two arguments:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;1) &lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;GroupWhisperType:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt; or &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALLCLIENTS&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;2)&lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;GroupWhisperTargetMode&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALL CURRENTCHANNEL ALLPARENTCHANNELS SUBCHANNELS PARENTCHANNEL CHANNELFAMILYANCESTORCHANNELFAMILY&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;So, the equivalent for the provided hotkey would be: &lt;/span&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER ALL&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Setup:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Options-&amp;gt; Hotkeys-&amp;gt; Hinzufügen -&amp;gt;&lt;/p&gt;&lt;p&gt;1) HotKey anlegen mit: &amp;quot;Beim Drücken der Taste&amp;quot; :&lt;/p&gt;&lt;p&gt;Erweiterte Aktionen anzeigen -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk Start.&lt;/p&gt;&lt;p&gt;2) Äquivalenten HotKey anlegen mit: &amp;quot;Beim Loslassen der Taste&amp;quot;:&lt;/p&gt;&lt;p&gt;Erweiterte Aktionen anzeigen -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk End.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;Für &lt;span style=&quot; font-weight:600;&quot;&gt;whispern&lt;/span&gt; have ich eine oft genutzte Variante hinzugefügt, whispern an alle Channel Commander in allen Channels. Setup äquivalent. Falls du eine speziellere Whisperliste brauchst, musst du &amp;quot;Plugin Command&amp;quot; für die &amp;quot;Beim Loslassen der Taste&amp;quot;-Aktion verwenden.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;is das grundlegende Kommando, das zwei Argumente annimmt:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;1) &lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;Gruppenwhispertyp:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt; or &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALLCLIENTS&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;2)&lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;Gruppenwhisperzielmodus&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALL CURRENTCHANNEL ALLPARENTCHANNELS SUBCHANNELS PARENTCHANNEL CHANNELFAMILYANCESTORCHANNELFAMILY&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;Das Äquivalent zum Bereitgestellten Hotkey wäre also bspw.: &lt;/span&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER ALL&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="296"/>
        <source>Switch&apos;n&apos;Talk</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/config.ui" line="315"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;a href=&quot;http://www.pledgie.com/campaigns/15624&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#0000ff;&quot;&gt;Support the author!&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;a href=&quot;http://www.pledgie.com/campaigns/15624&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#0000ff;&quot;&gt;Unterstütze die Entwicklung!&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
</context>
<context>
    <name>context_menu</name>
    <message>
        <location filename="src/plugin.cpp" line="394"/>
        <source>Toggle Global Ducking Target</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/plugin.cpp" line="395"/>
        <source>Toggle ChannelMuter Whitelisting [temp]</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/plugin.cpp" line="396"/>
        <source>Toggle Channel Mute [temp]</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
