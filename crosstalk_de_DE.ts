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
        <location filename="src/config.ui" line="156"/>
        <source>Audio Stream: Clients talking on the currently active server tab.</source>
        <translation>Audio Stream: Klienten, die auf dem derzeit aktiven Server Reiter sprechen.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="172"/>
        <source>Audio Stream: Any incoming whispers.</source>
        <translation>Audio Stream: Sämtliche einkommenden Whispers.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="185"/>
        <source>Audio Stream: Clients talking on currently deselected server tabs.</source>
        <translation>Audio Stream: Klienten, die auf den nicht selektierten Server Reitern sprechen.</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="198"/>
        <location filename="src/config.ui" line="217"/>
        <location filename="src/config.ui" line="236"/>
        <source>Audio Region: Left, Middle, Right</source>
        <translation>Audio Region: Links, Mitte, Rechts</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="277"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Home Channel Ducking&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;In short:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Reduces the volume of speakers in the channel of the currently active server tab as soon as a whisper comes in or someone talks on another server tab (in common scenarios this is some kind of commander channel).&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;Longer version:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In case you haven&apos;t heard of ducking, let&apos;s start with that.&lt;/p&gt;&lt;p&gt;Ducking is the reduction of a target channels volume dependant on the activity on a source channel. Usually, the actual sound volume of the source channel is analyzed to modify the reduction of the target channel. Here, however, keeping in mind we don&apos;t want to either sound or reaction latency, the talk status is used as trigger, the reduction solely determined by the setting.&lt;/p&gt;&lt;p&gt;So now, the only question left would be: What&apos;s the source? What&apos;s the target?&lt;/p&gt;&lt;p&gt;Target: Your &amp;quot;home&amp;quot; channel. That&apos;s what I call the channel of your currently active server tab.&lt;/p&gt;&lt;p&gt;Sources: Whispers and everything that&apos;s coming in from another server tab.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Home Channel Ducking&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;In Kurz:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Reduziert die Lautstärke der Sprecher im Channel des derzeit aktiven Server Reiters, sobald ein Whisper eintrifft oder jemand auf einem anderen Reiter spricht. (in üblichen Anwendungsszenarien z.B. ein CommanderChannel).&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; text-decoration: underline;&quot;&gt;Längere Version:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Falls dir Ducking nichts sagt, zunächst eine Erklärung.&lt;/p&gt;&lt;p&gt;Ducking bezeichnet die Reduktion der Lautstärke eines Ziels abhängig von der Aktivität eines Ursprungskanals. Üblicherweise wird die Lautstärke des Ursprungskanals analysiert, um die Modifikation der Lautstärke des Zielkanals zu berechnen. Hier allerdings benutzen wir schlicht den Gesprächsstatus als Aktivierung, um zusätzliche Latenzen zu vermeiden und den Rechenaufwand zu reduzieren.&lt;/p&gt;&lt;p&gt;Bleibt nur noch die Frage: Was ist der Ursprung, was ist das Ziel?&lt;/p&gt;&lt;p&gt;Einkommende Whispers sind immer Ursprung, also Auslöser, wie in normaler Operation auch alles von nicht selektierten Server Reitern, das Ziel: Dein &amp;quot;home&amp;quot; channel, der Channel in dem du auf dem derzeit selektierten Server bist.&lt;/p&gt;&lt;p&gt;Läuft das Modul im Umkehrmodus, sind Whispers und der Home Tab Ursprung und nicht selektierte Server Reiter das Ziel.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="283"/>
        <source>Channel Ducking</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/config.ui" line="445"/>
        <source>Target</source>
        <translation>Ziel</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="465"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Setup:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Options-&amp;gt; Hotkeys-&amp;gt; Add -&amp;gt;&lt;/p&gt;&lt;p&gt;1) Set HotKey with &amp;quot;On key down&amp;quot; :&lt;/p&gt;&lt;p&gt;Show Advanced Options -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk Start.&lt;/p&gt;&lt;p&gt;2) Set same HotKey with &amp;quot;On key up&amp;quot;:&lt;/p&gt;&lt;p&gt;Show Advanced Options -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk End.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;For &lt;span style=&quot; font-weight:600;&quot;&gt;whispering&lt;/span&gt;, I&apos;ve provided one often used variant, whispering to all channelcommander on all channels. Setup is equivalent. If you need a more specific whisper list, you have to use &amp;quot;Plugin Command&amp;quot; for the &amp;quot;On key down&amp;quot; action.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;is the basic command, which takes two arguments:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;1) &lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;GroupWhisperType:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt; or &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALLCLIENTS&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;2)&lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;GroupWhisperTargetMode&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALL CURRENTCHANNEL ALLPARENTCHANNELS SUBCHANNELS PARENTCHANNEL CHANNELFAMILYANCESTORCHANNELFAMILY&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;So, the equivalent for the provided hotkey would be: &lt;/span&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER ALL&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600; text-decoration: underline;&quot;&gt;Setup:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Options-&amp;gt; Hotkeys-&amp;gt; Hinzufügen -&amp;gt;&lt;/p&gt;&lt;p&gt;1) HotKey anlegen mit: &amp;quot;Beim Drücken der Taste&amp;quot; :&lt;/p&gt;&lt;p&gt;Erweiterte Aktionen anzeigen -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk Start.&lt;/p&gt;&lt;p&gt;2) Äquivalenten HotKey anlegen mit: &amp;quot;Beim Loslassen der Taste&amp;quot;:&lt;/p&gt;&lt;p&gt;Erweiterte Aktionen anzeigen -&amp;gt; Plugins -&amp;gt; Plugin Hotkey -&amp;gt; CrossTalk -&amp;gt; Next Tab and Talk End.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;Für &lt;span style=&quot; font-weight:600;&quot;&gt;whispern&lt;/span&gt; have ich eine oft genutzte Variante hinzugefügt, whispern an alle Channel Commander in allen Channels. Setup äquivalent. Falls du eine speziellere Whisperliste brauchst, musst du &amp;quot;Plugin Command&amp;quot; für die &amp;quot;Beim Loslassen der Taste&amp;quot;-Aktion verwenden.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;is das grundlegende Kommando, das zwei Argumente annimmt:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;1) &lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;Gruppenwhispertyp:&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt; or &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALLCLIENTS&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;2)&lt;/span&gt;&lt;span style=&quot; color:#800080;&quot;&gt;Gruppenwhisperzielmodus&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;ALL CURRENTCHANNEL ALLPARENTCHANNELS SUBCHANNELS PARENTCHANNEL CHANNELFAMILYANCESTORCHANNELFAMILY&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;Das Äquivalent zum Bereitgestellten Hotkey wäre also bspw.: &lt;/span&gt;&lt;span style=&quot; color:#008000;&quot;&gt;TS3_NEXT_TAB_AND_WHISPER_START &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#800080;&quot;&gt;CHANNELCOMMANDER ALL&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="src/config.ui" line="468"/>
        <source>Switch&apos;n&apos;Talk</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/config.ui" line="487"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;a href=&quot;http://www.pledgie.com/campaigns/15624&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#0000ff;&quot;&gt;Support the author!&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;a href=&quot;http://www.pledgie.com/campaigns/15624&quot;&gt;&lt;span style=&quot; text-decoration: underline; color:#0000ff;&quot;&gt;Unterstütze die Entwicklung!&lt;/span&gt;&lt;/a&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
</context>
</TS>
