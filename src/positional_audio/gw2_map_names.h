#ifndef GW2_MAP_NAMES_H
#define GW2_MAP_NAMES_H

#include <QHash>
#include <QString>

namespace GW2
{

const QHash<int,QString> _Init_GW2_MAP_NAMES_EN() {
    QHash<int,QString> hash;
    hash[50]="Lion's Arch";
    hash[73]="Bloodtide Coast";
    hash[27]="Lornar's Pass";
    hash[15]="Queensdale";
    hash[65]="Malchor's Leap";
    hash[39]="Mount Maelstrom";
    hash[26]="Dredgehaunt Cliffs";
    hash[21]="Fields of Ruin";
    hash[62]="Cursed Shore";
    hash[53]="Sparkfly Fen";
    hash[28]="Wayfarer Foothills";
    hash[25]="Iron Marches";
    hash[34]="Caledon Forest";
    hash[873]="Southsun Cove";
    hash[24]="Gendarran Fields";
    hash[22]="Fireheart Rise";
    hash[17]="Harathi Hinterlands";
    hash[20]="Blazeridge Steppes";
    hash[54]="Brisban Wildlands";
    hash[19]="Plains of Ashford";
    hash[31]="Snowden Drifts";
    hash[23]="Kessex Hills";
    hash[30]="Frostgorge Sound";
    hash[51]="Straits of Devastation";
    hash[29]="Timberline Falls";
    hash[32]="Diessa Plateau";
    hash[35]="Metrica Province";
    hash[943]="Tower of Nightmares";
    return hash;
}

const QHash<int,QString> _Init_GW2_MAP_NAMES_DE() {
    QHash<int,QString> hash;
    hash[50]="Löwenstein";
    hash[73]="Blutstrom-Küste";
    hash[27]="Lornars Pass";
    hash[15]="Königintal";
    hash[65]="Malchors Sprung";
    hash[39]="Mahlstromgipfel";
    hash[26]="Schauflerschreck-Klippen";
    hash[21]="Felder der Verwüstung";
    hash[62]="Fluchküste";
    hash[53]="Funkenschwärmersumpf";
    hash[28]="Wanderer-Hügel";
    hash[25]="Eisenmark";
    hash[34]="Caledon-Wald";
    hash[873]="Südlicht-Bucht";
    hash[24]="Gendarran-Felder";
    hash[22]="Feuerherzhügel";
    hash[17]="Harathi-Hinterland";
    hash[20]="Flammenkamm-Steppe";
    hash[54]="Brisban-Wildnis";
    hash[19]="Ebenen von Aschfurt";
    hash[31]="Schneekuhlenhöhen";
    hash[23]="Kessex-Hügel";
    hash[30]="Eisklamm-Sund";
    hash[51]="Meerenge der Verwüstung";
    hash[29]="Baumgrenzen-Fälle";
    hash[32]="Diessa-Plateau";
    hash[35]="Provinz Metrica";
    hash[943]="Turm der Albträume";
    return hash;
}

const QHash<int,QString> _Init_GW2_MAP_NAMES_FR() {
    QHash<int,QString> hash;
    hash[50]="L'Arche du Lion";
    hash[73]="Côte de la marée sanglante";
    hash[27]="Passage de Lornar";
    hash[15]="La Vallée de la reine";
    hash[65]="Saut de Malchor";
    hash[39]="Mont Maelström";
    hash[26]="Falaises de Hantedraguerre";
    hash[21]="Champs de Ruine";
    hash[62]="Rivage maudit";
    hash[53]="Marais de Lumillule";
    hash[28]="Contreforts du Voyageur";
    hash[25]="Marais de fer";
    hash[34]="Forêt de Caledon";
    hash[873]="Crique de Sud-Soleil";
    hash[24]="Champs de Gendarran";
    hash[22]="Montée de Flambecœur";
    hash[17]="Hinterlands harathis";
    hash[20]="Les Steppes de la Strie flamboyante";
    hash[54]="Terres sauvages de Brisban";
    hash[19]="Plaines d'Ashford";
    hash[31]="Congères d'Antreneige";
    hash[23]="Collines de Kesse";
    hash[30]="Détroit des gorges glacées";
    hash[51]="Détroit de la Dévastation";
    hash[29]="Chutes de la Canopée";
    hash[32]="Plateau de Diessa";
    hash[35]="Province de Metrica";
    hash[943]="Tour des Cauchemars";
    return hash;
}

const QHash<int,QString> _Init_GW2_MAP_NAMES_ES() {
    QHash<int,QString> hash;
    hash[50]="Arco del León";
    hash[73]="Costa Mareasangrienta";
    hash[27]="Paso de Lornar";
    hash[15]="Valle de la Reina";
    hash[65]="Salto de Malchor";
    hash[39]="Monte Vorágine";
    hash[26]="Acantilados de Guaridadraga";
    hash[21]="Campos de la Ruina";
    hash[62]="Ribera Maldita";
    hash[53]="Pantano de las Centellas";
    hash[28]="Colinas del Caminante";
    hash[25]="Fronteras de Hierro";
    hash[34]="Bosque de Caledon";
    hash[873]="Cala del Sol Austral";
    hash[24]="Campos de Gendarran";
    hash[22]="Colina del Corazón de fuego";
    hash[17]="Interior Harathi";
    hash[20]="Estepas Crestafulgurante";
    hash[54]="Selvas Brisbanas";
    hash[19]="Llanuras de Ashford";
    hash[31]="Cúmulos de Guaridanieve";
    hash[23]="Colinas Kessex";
    hash[30]="Estrecho de Gorjaescarcha";
    hash[51]="Estrechos de la Devastación";
    hash[29]="Cataratas de Linarbórea";
    hash[32]="Meseta de Diessa";
    hash[35]="Provincia de Métrica";
    hash[943]="Torre de las Pesadillas";
    return hash;
}

QHash<int,QString> GW2_MAP_NAMES_DE;
QHash<int,QString> GW2_MAP_NAMES_FR;
QHash<int,QString> GW2_MAP_NAMES_ES;
QHash<int,QString> GW2_MAP_NAMES_EN;

QString getMapName(int world_id, QString lang) {
    if (lang == "de_DE")
    {
        if (GW2_MAP_NAMES_DE.isEmpty())
            GW2_MAP_NAMES_DE = _Init_GW2_MAP_NAMES_DE();

        return GW2_MAP_NAMES_DE.value(world_id, "Unbekannt");
    }
    else if (lang == "fr_FR")
    {
        if (GW2_MAP_NAMES_FR.isEmpty())
            GW2_MAP_NAMES_FR = _Init_GW2_MAP_NAMES_FR();

        return GW2_MAP_NAMES_FR.value(world_id, "Unknown");
    }
    else if (lang == "es_ES")
    {
        if (GW2_MAP_NAMES_ES.isEmpty())
            GW2_MAP_NAMES_ES = _Init_GW2_MAP_NAMES_ES();

        return GW2_MAP_NAMES_ES.value(world_id, "Unknown");
    }
    else
    {
        if (GW2_MAP_NAMES_EN.isEmpty())
            GW2_MAP_NAMES_EN = _Init_GW2_MAP_NAMES_EN();

        return GW2_MAP_NAMES_EN.value(world_id, "Unknown");
    }
}

}
#endif // GW2_MAP_NAMES_H
