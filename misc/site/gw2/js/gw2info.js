// This needs prototypejs

// requires cdnjs.cloudflare.com/ajax/libs/idbwrapper/1.4.1/idbstore.min.js

(function(){

	var global = (function(){return this;}).call();	// for waffle's sake

    var publisher = {
		subscribers: {
			any: []
		},
		on: function (type, fn, context) {
			type = type || 'any';
			fn = typeof fn === "function" ? fn : context[fn];

			if (typeof this.subscribers[type] === "undefined") {
				this.subscribers[type] = [];
			}
			this.subscribers[type].push({fn: fn, context: context || this});
		},
		remove: function (type, fn, context) {
			this.visitSubscribers('unsubscribe', type, fn, context);
		},
		fire: function (type, publication) {
			this.visitSubscribers('publish', type, publication);
		},
		visitSubscribers: function (action, type, arg, context) {
			var pubtype = type || 'any',
				subscribers = this.subscribers[pubtype],
				i,
				max = subscribers ? subscribers.length : 0;

			for (i = 0; i < max; i += 1) {
				if (action === 'publish') {
					subscribers[i].fn.call(subscribers[i].context, arg);
				} else {
					if (subscribers[i].fn === arg && subscribers[i].context === context) {
						subscribers.splice(i, 1);
					}
				}
			}
		}
    };
	
    /*console.log("IDBDatabase:");
    console.log(IDBDatabase? 'true' : 'false');
    console.log(webkitIDBDatabase ? 'true' : 'false');
    console.log(window.webkitIDBDatabase ? 'true' :'false');*/
    // awesomium lagging behind
    var indexedDB = indexedDB || window.indexedDB || window.webkitIndexedDB || window.mozIndexedDB || window.oIndexedDB || window.msIndexedDB,
        IDBKeyRange = IDBKeyRange || window.IDBKeyRange || window.webkitIDBKeyRange,
        IDBTransaction = IDBTransaction || window.IDBTransaction || window.webkitIDBTransaction || { READ_WRITE: "readwrite" },
        IDBDatabase = IDBDatabase || window.IDBDatabase || window.webkitIDBDatabase || ((typeof webkitIDBDatabase !== "undefined")?webkitIDBDatabase:false);

    var deleteDatabase = function() {
       var deleteDbRequest = indexedDB.deleteDatabase('gw2info');
       deleteDbRequest.onsuccess = function (event) {
          // database deleted successfully
           console.log("Db deleted successfully.");
       };
       deleteDbRequest.onerror = function (e) {
          console.log("Database error: " + e.target.errorCode);
       };
    };

    var data = {
        wvw_mapTypeId : {"Center": 38, "BlueHome": 94, "GreenHome" :95, "RedHome":96, 38:"Center", 94:"BlueHome", 95:"GreenHome", 96:"RedHome"},

        wvw_objectives : {
            "Center": {
                1: {
                    "score": 25,
                    "coords": [10766, 13656],
                    "type": "keep",
                    "name": {
                        "de": "Aussichtspunkt",
                        "en": "Overlook",
                        "es": "Mirador",
                        "fr": "Belvédère"
                    }
                },
                2: {
                    "score": 25,
                    "coords": [11497, 15121],
                    "type": "keep",
                    "name": {
                        "de": "Tal",
                        "en": "Valley",
                        "es": "Valle",
                        "fr": "Vallée"
                    }
                },
                3: {
                    "score": 25,
                    "coords": [9606, 15132],
                    "type": "keep",
                    "name": {
                        "de": "Tiefland",
                        "en": "Lowlands",
                        "es": "Vega",
                        "fr": "Basses terres"
                    }
                },
                4: {
                    "score": 5,
                    "coords": [10211, 15437],
                    "type": "camp",
                    "name": {
                        "de": "Golanta-Lichtung",
                        "en": "Golanta Clearing",
                        "es": "Claro Golanta",
                        "fr": "Clairière de Golanta"
                    }
                },
                5: {
                    "score": 5,
                    "coords": [11226, 13754],
                    "type": "camp",
                    "name": {
                        "de": "Pangloss-Anhöhe",
                        "en": "Pangloss Rise",
                        "es": "Colina Pangloss",
                        "fr": "Montée de Pangloss"
                    }
                },
                6: {
                    "score": 5,
                    "coords": [9850, 13561],
                    "type": "camp",
                    "name": {
                        "de": "Speldan Kahlschlag",
                        "en": "Speldan Clearcut",
                        "es": "Claro Espeldia",
                        "fr": "Forêt rasée de Speldan"
                    }
                },
                7: {
                    "score": 5,
                    "coords": [11030, 15559],
                    "type": "camp",
                    "name": {
                        "de": "Danelon-Passage",
                        "en": "Danelon Passage",
                        "es": "Pasaje Danelon",
                        "fr": "Passage Danelon"
                    }
                },
                8: {
                    "score": 5,
                    "coords": [11571, 14468],
                    "type": "camp",
                    "name": {
                        "de": "Umberlichtung-Forst",
                        "en": "Umberglade Woods",
                        "es": "Bosques Clarosombra",
                        "fr": "Bois d'Ombreclair"
                    }
                },
                9: {
                    "score": 35,
                    "coords": [10608, 14583],
                    "type": "castle",
                    "name": {
                        "de": "Schloss Steinnebel",
                        "en": "Stonemist Castle",
                        "es": "Castillo Piedraniebla",
                        "fr": "Château Brumepierre"
                    }
                },
                10: {
                    "score": 5,
                    "coords": [9574, 14435],
                    "type": "camp",
                    "name": {
                        "de": "Schurkenbruch",
                        "en": "Rogue's Quarry",
                        "es": "Cantera del Pícaro",
                        "fr": "Carrière des voleurs"
                    }
                },
                11: {
                    "score": 10,
                    "coords": [9416, 14793],
                    "type": "tower",
                    "name": {
                        "de": "Aldons Vorsprung",
                        "en": "Aldon's Ledge",
                        "es": "Cornisa de Aldon",
                        "fr": "Corniche d'Aldon"
                    }
                },
                12: {
                    "score": 10,
                    "coords": [9908, 14626],
                    "type": "tower",
                    "name": {
                        "de": "Wildbachstrecke",
                        "en": "Wildcreek Run",
                        "es": "Pista Arroyosalvaje",
                        "fr": "Piste du Ruisseau sauvage"
                    }
                },
                13: {
                    "score": 10,
                    "coords": [9807, 15409],
                    "type": "tower",
                    "name": {
                        "de": "Jerrifers Sumpfloch",
                        "en": "Jerrifer's Slough",
                        "es": "Cenagal de Jerrifer",
                        "fr": "Bourbier de Jerrifer"
                    }
                },
                14: {
                    "score": 10,
                    "coords": [10172, 15082],
                    "type": "tower",
                    "name": {
                        "de": "Klovan-Senke",
                        "en": "Klovan Gully",
                        "es": "Barranco Klovan",
                        "fr": "Petit ravin de Klovan"
                    }
                },
                15: {
                    "score": 10,
                    "coords": [11456, 15492],
                    "type": "tower",
                    "name": {
                        "de": "Langor-Schlucht",
                        "en": "Langor Gulch",
                        "es": "Barranco Langor",
                        "fr": "Ravin de Langor"
                    }
                },
                16: {
                    "score": 10,
                    "coords": [10852, 15224],
                    "type": "tower",
                    "name": {
                        "de": "Quentinsee",
                        "en": "Quentin Lake",
                        "es": "Lago Quentin",
                        "fr": "Lac Quentin"
                    }
                },
                17: {
                    "score": 10,
                    "coords": [10257, 13515],
                    "type": "tower",
                    "name": {
                        "de": "Mendons Spalt",
                        "en": "Mendon's Gap",
                        "es": "Zanja de Mendon",
                        "fr": "Faille de Mendon"
                    }
                },
                18: {
                    "score": 10,
                    "coords": [10190, 14082],
                    "type": "tower",
                    "name": {
                        "de": "Anzalias-Pass",
                        "en": "Anzalias Pass",
                        "es": "Paso Anzalias",
                        "fr": "Col d'Anzalias"
                    }
                },
                19: {
                    "score": 10,
                    "coords": [11012, 13849],
                    "type": "tower",
                    "name": {
                        "de": "Ogerwacht-Kanal",
                        "en": "Ogrewatch Cut",
                        "es": "Tajo de la Guardia del Ogro",
                        "fr": "Percée de Gardogre"
                    }
                },
                20: {
                    "score": 10,
                    "coords": [11093, 13488],
                    "type": "tower",
                    "name": {
                        "de": "Veloka-Hang",
                        "en": "Veloka Slope",
                        "es": "Pendiente Veloka",
                        "fr": "Flanc de Veloka"
                    }
                },
                21: {
                    "score": 10,
                    "coords": [11157, 14529],
                    "type": "tower",
                    "name": {
                        "de": "Durios-Schlucht",
                        "en": "Durios Gulch",
                        "es": "Barranco Durios",
                        "fr": "Ravin de Durios"
                    }
                },
                22: {
                    "score": 10,
                    "coords": [11768, 14792],
                    "type": "tower",
                    "name": {
                        "de": "Bravost-Abhang",
                        "en": "Bravost Escarpment",
                        "es": "Escarpadura Bravost",
                        "fr": "Falaise de Bravost"
                    }
                }
            },
            "BlueHome": {
                23: {
                    "score": 25,
                    "coords": [14058, 12432],
                    "type": "keep",
                    "name": {
                        "de": "Festung",
                        "en": "Garrison",
                        "es": "Fuerte",
                        "fr": "Garnison"
                    }
                },
                24: {
                    "score": 5,
                    "coords": [14084, 14041],
                    "type": "camp",
                    "name": {
                        "de": "Landgut des Champions",
                        "en": "Champion's demense",
                        "es": "Dominio del Campeón",
                        "fr": "Fief du champion"
                    }
                },
                25: {
                    "score": 10,
                    "coords": [13750, 13344],
                    "type": "tower",
                    "name": {
                        "de": "Rotdornstrauch",
                        "en": "Redbriar",
                        "es": "Zarzarroja",
                        "fr": "Bruyerouge"
                    }
                },
                26: {
                    "score": 10,
                    "coords": [14582, 13410],
                    "type": "tower",
                    "name": {
                        "de": "Grünsee",
                        "en": "Greenlake",
                        "es": "Lagoverde",
                        "fr": "Lac Vert"
                    }
                },
                27: {
                    "score": 25,
                    "coords": [13036, 12956],
                    "type": "keep",
                    "name": {
                        "de": "Bucht des Aufstiegs",
                        "en": "Ascension Bay",
                        "es": "Bahía de la Ascensión",
                        "fr": "Baie de l'Ascension"
                    }
                },
                28: {
                    "score": 10,
                    "coords": [14686, 12030],
                    "type": "tower",
                    "name": {
                        "de": "Horst der Morgendämmerung",
                        "en": "Dawn's Eyrie",
                        "es": "Aguilera del Alba",
                        "fr": "Promontoire de l'aube"
                    }
                },
                29: {
                    "score": 5,
                    "coords": [14088, 11227],
                    "type": "camp",
                    "name": {
                        "de": "Der Geisterholm",
                        "en": "The Spiritholme",
                        "es": "La Isleta Espiritual",
                        "fr": "L'antre des esprits"
                    }
                },
                30: {
                    "score": 10,
                    "coords": [13446, 12079],
                    "type": "tower",
                    "name": {
                        "de": "Wald-Freistatt",
                        "en": "Woodhaven",
                        "es": "Refugio Forestal",
                        "fr": "Gentesylve"
                    }
                },
                31: {
                    "score": 25,
                    "coords": [15253, 12882],
                    "type": "keep",
                    "name": {
                        "de": "Askalion-Hügel",
                        "en": "Askalion Hills",
                        "es": "Colinas Askalion",
                        "fr": "Collines d'Askalion"
                    }
                },
                58: {
                    "score": 5,
                    "coords": [13210, 12208],
                    "type": "camp",
                    "name": {
                        "de": "Götterkunde",
                        "en": "Godslore",
                        "es": "Sabiduría de los Dioses",
                        "fr": "Divination"
                    }
                },
                59: {
                    "score": 5,
                    "coords": [13277, 13454],
                    "type": "camp",
                    "name": {
                        "de": "Rottal-Zuflucht",
                        "en": "Redvale Refuge",
                        "es": "Refugio Vallerojo",
                        "fr": "Refuge de Valrouge"
                    }
                },
                60: {
                    "score": 5,
                    "coords": [15037, 12151],
                    "type": "camp",
                    "name": {
                        "de": "Sternenhain",
                        "en": "Stargrove",
                        "es": "Arboleda de las Estrellas",
                        "fr": "Bosquet stellaire"
                    }
                },
                61: {
                    "score": 5,
                    "coords": [15023, 13504],
                    "type": "camp",
                    "name": {
                        "de": "Grünwasser-Tiefland",
                        "en": "Greenwater Lowlands",
                        "es": "Tierras Bajas de Aguaverde",
                        "fr": "Basses terres d'Eau-Verdoyante"
                    }
                },
                67: {
                    "score": 0,
                    "coords": [14363, 13110],
                    "type": "ruin",
                    "fileApiBase":"wvw_carvers_ascent_",
                    "name": {
                        "de": "Aufstieg des Schnitzers",
                        "en": "Carver's Ascent",
                        "es": "Ascenso del Trinchador",
                        "fr": "Côte du couteau"
                    }
                },
                68: {
                    "score": 0,
                    "coords": [14332, 12759],
                    "type": "ruin",
                    "fileApiBase":"wvw_orchard_overlook_",
                    "name": {
                        "de": "Obstgarten-Aussichtspunkt",
                        "en": "Orchard Overlook",
                        "es": "Mirador del Huerto",
                        "fr": "Belvédère du Verger"
                    }
                },
                69: {
                    "score": 0,
                    "coords": [13866, 12704],
                    "type": "ruin",
                    "fileApiBase":"wvw_bauers_estate_",
                    "name": {
                        "de": "Bauers Anwesen",
                        "en": "Bauer's Estate",
                        "es": "Hacienda de Bauer",
                        "fr": "Domaine de Bauer"
                    }
                },
                70: {
                    "score": 0,
                    "coords": [13756, 13068],
                    "type": "ruin",
                    "fileApiBase":"wvw_battles_hollow_",
                    "name": {
                        "de": "Schlachten-Senke",
                        "en": "Battle's Hollow",
                        "es": "Hondonada de la Battalla",
                        "fr": "Vallon de bataille"
                    }
                },
                71: {
                    "score": 0,
                    "coords": [14068, 13344],
                    "type": "ruin",
                    "fileApiBase":"wvw_temple_of_lost_prayers_",
                    "name": {
                        "de": "Tempel der Verlorenen Gebete",
                        "en": "Temple of Lost Prayers",
                        "es": "Templo de las Pelgarias",
                        "fr": "Temple des prières perdues"
                    }
                }
            },
            "GreenHome": {
                41: {
                    "score": 25,
                    "coords": [8085, 13519],
                    "type": "keep",
                    "name": {
                        "de": "Shadaran-Hügel",
                        "en": "Shadaran Hills",
                        "es": "Colinas Shadaran",
                        "fr": "Collines de Shadaran"
                    }
                },
                42: {
                    "score": 10,
                    "coords": [7414, 14051],
                    "type": "tower",
                    "name": {
                        "de": "Rotsee",
                        "en": "Redlake",
                        "es": "Lagorrojo",
                        "fr": "Rougelac"
                    }
                },
                43: {
                    "score": 5,
                    "coords": [6911, 14679],
                    "type": "camp",
                    "name": {
                        "de": "Hütte des Helden",
                        "en": "Hero's Lodge",
                        "es": "Albergue del Héroe",
                        "fr": "Pavillon du Héros"
                    }
                },
                44: {
                    "score": 25,
                    "coords": [5869, 13598],
                    "type": "keep",
                    "name": {
                        "de": "Schreckensfall-Bucht",
                        "en": "Dreadfall Bay",
                        "es": "Bahía Salto Aciago",
                        "fr": "Baie du Noir déclin"
                    }
                },
                45: {
                    "score": 10,
                    "coords": [6584, 13983],
                    "type": "tower",
                    "name": {
                        "de": "Blaudornstrauch",
                        "en": "Bluebriar",
                        "es": "Zarzazul",
                        "fr": "Bruyazur"
                    }
                },
                46: {
                    "score": 25,
                    "coords": [6890, 13073],
                    "type": "keep",
                    "name": {
                        "de": "Festung",
                        "en": "Garrison",
                        "es": "Fuerte",
                        "fr": "Garnison"
                    }
                },
                47: {
                    "score": 10,
                    "coords": [6279, 12719],
                    "type": "tower",
                    "name": {
                        "de": "Sonnenlichthügel",
                        "en": "Sunnyhill",
                        "es": "Colina Soleada",
                        "fr": "Colline ensoleillée"
                    }
                },
                48: {
                    "score": 5,
                    "coords": [6046, 12838],
                    "type": "camp",
                    "name": {
                        "de": "Glaubenssprung",
                        "en": "Faithleap",
                        "es": "Salto de Fe",
                        "fr": "Ferveur"
                    }
                },
                49: {
                    "score": 5,
                    "coords": [6107, 14092],
                    "type": "camp",
                    "name": {
                        "de": "Blautal-Zuflucht",
                        "en": "Bluevale Refuge",
                        "es": "Refugio Valleazul",
                        "fr": "Refuge de bleubal"
                    }
                },
                54: {
                    "score": 5,
                    "coords": [7873, 12800],
                    "type": "camp",
                    "name": {
                        "de": "Nebel-Freistatt",
                        "en": "Foghaven",
                        "es": "Refugio Neblinoso",
                        "fr": "Havre gris"
                    }
                },
                55: {
                    "score": 5,
                    "coords": [7851, 14136],
                    "type": "camp",
                    "name": {
                        "de": "Rotwasser-Tiefland",
                        "en": "Redwater Lowlands",
                        "es": "Tierras Bajas de Aguarroja",
                        "fr": "Basses terres de Rubicon"
                    }
                },
                56: {
                    "score": 5,
                    "coords": [6920, 11867],
                    "type": "camp",
                    "name": {
                        "de": "Die Titanenpranke",
                        "en": "The Titanpaw",
                        "es": "La Garra del Titán",
                        "fr": "Bras du titan"
                    }
                },
                57: {
                    "score": 10,
                    "coords": [7517, 12673],
                    "type": "tower",
                    "name": {
                        "de": "Felsenspitze",
                        "en": "Cragtop",
                        "es": "Cumbrepeñasco",
                        "fr": "Sommet de l'escarpement"
                    }
                },
                72: {
                    "score": 0,
                    "coords": [7196, 13754],
                    "type": "ruin",
                    "fileApiBase":"wvw_carvers_ascent_",
                    "name": {
                        "de": "Aufstieg des Schnitzers",
                        "en": "Carver's Ascent",
                        "es": "Ascenso del Trinchador",
                        "fr": "Côte du couteau"
                    }
                },
                73: {
                    "score": 0,
                    "coords": [7163, 13403],
                    "type": "ruin",
                    "fileApiBase":"wvw_orchard_overlook_",
                    "name": {
                        "de": "Obstgarten-Aussichtspunkt",
                        "en": "Orchard Overlook",
                        "es": "Mirador del Huerto",
                        "fr": "Belvédère du Verger"
                    }
                },
                74: {
                    "score": 0,
                    "coords": [6694, 13349],
                    "type": "ruin",
                    "fileApiBase":"wvw_bauers_estate_",
                    "name": {
                        "de": "Bauers Anwesen",
                        "en": "Bauer's Estate",
                        "es": "Hacienda de Bauer",
                        "fr": "Domaine de Bauer"
                    }
                },
                75: {
                    "score": 0,
                    "coords": [6591, 13715],
                    "type": "ruin",
                    "fileApiBase":"wvw_battles_hollow_",
                    "name": {
                        "de": "Schlachten-Senke",
                        "en": "Battle's Hollow",
                        "es": "Hondonada de la Battalla",
                        "fr": "Vallon de bataille"
                    }
                },
                76: {
                    "score": 0,
                    "coords": [6899, 13987],
                    "type": "ruin",
                    "fileApiBase":"wvw_temple_of_lost_prayers_",
                    "name": {
                        "de": "Tempel der Verlorenen Gebete",
                        "en": "Temple of Lost Prayers",
                        "es": "Templo de las Pelgarias",
                        "fr": "Temple des prières perdues"
                    }
                }
            },
            "RedHome": {
                32: {
                    "score": 25,
                    "coords": [11669, 10959],
                    "type": "keep",
                    "name": {
                        "de": "Etheron-Hügel",
                        "en": "Etheron Hills",
                        "es": "Colinas Etheron",
                        "fr": "Collines d'Etheron"
                    }
                },
                33: {
                    "score": 25,
                    "coords": [9453, 11037],
                    "type": "keep",
                    "name": {
                        "de": "Traumbucht",
                        "en": "Dreaming Bay",
                        "es": "Bahía Onírica",
                        "fr": "Baie des rêves"
                    }
                },
                34: {
                    "score": 5,
                    "coords": [10497, 12119],
                    "type": "camp",
                    "name": {
                        "de": "Sieger-Hütte",
                        "en": "Victors's Lodge",
                        "es": "Albergue del Vencedor",
                        "fr": "Pavillon du vainqueur"
                    }
                },
                35: {
                    "score": 10,
                    "coords": [10167, 11423],
                    "type": "tower",
                    "name": {
                        "de": "Grünstrauch",
                        "en": "Greenbriar",
                        "es": "Zarzaverde",
                        "fr": "Vertebranche"
                    }
                },
                36: {
                    "score": 10,
                    "coords": [10998, 11490],
                    "type": "tower",
                    "name": {
                        "de": "Blausee",
                        "en": "Bluelake",
                        "es": "Lagoazul",
                        "fr": "Lac bleu"
                    }
                },
                37: {
                    "score": 25,
                    "coords": [10474, 10513],
                    "type": "keep",
                    "name": {
                        "de": "Festung",
                        "en": "Garrison",
                        "es": "Fuerte",
                        "fr": "Garnison"
                    }
                },
                38: {
                    "score": 10,
                    "coords": [9861, 10159],
                    "type": "tower",
                    "name": {
                        "de": "Weitsicht",
                        "en": "Longview",
                        "es": "Vistaluenga",
                        "fr": "Longuevue"
                    }
                },
                39: {
                    "score": 5,
                    "coords": [10505, 9307],
                    "type": "camp",
                    "name": {
                        "de": "Das Gottschwert",
                        "en": "The Godsword",
                        "es": "La Hoja Divina",
                        "fr": "L'Epée divine"
                    }
                },
                40: {
                    "score": 10,
                    "coords": [11102, 10113],
                    "type": "tower",
                    "name": {
                        "de": "Felswand",
                        "en": "Cliffside",
                        "es": "Despeñadero",
                        "fr": "Flanc de falaise"
                    }
                },
                50: {
                    "score": 5,
                    "coords": [11441, 11583],
                    "type": "camp",
                    "name": {
                        "de": "Blauwasser-Tiefland",
                        "en": "Bluewater Lowlands",
                        "es": "Tierras Bajas de Aguazul",
                        "fr": "Basses terres d'Eau-Azur"
                    }
                },
                51: {
                    "score": 5,
                    "coords": [11456, 10238],
                    "type": "camp",
                    "name": {
                        "de": "Astralholm",
                        "en": "Astralholme",
                        "es": "Isleta Astral",
                        "fr": "Astralholme"
                    }
                },
                52: {
                    "score": 5,
                    "coords": [9633, 10277],
                    "type": "camp",
                    "name": {
                        "de": "Arahs Hoffnung",
                        "en": "Arah's Hope",
                        "es": "Esperanza de Arah",
                        "fr": "Espoir d'Arah"
                    }
                },
                53: {
                    "score": 5,
                    "coords": [9692, 11535],
                    "type": "camp",
                    "name": {
                        "de": "Grüntal-Zuflucht",
                        "en": "Greenvale Refuge",
                        "es": "Refugio de Valleverde",
                        "fr": "Refuge de Valvert"
                    }
                },
                62: {
                    "score": 0,
                    "coords": [10484, 11421],
                    "type": "ruin",
                    "fileApiBase":"wvw_temple_of_lost_prayers_",
                    "name": {
                        "de": "Tempel der Verlorenen Gebete",
                        "en": "Temple of Lost Prayers",
                        "es": "Templo de las Pelgarias",
                        "fr": "Temple des prières perdues"
                    }
                },
                63: {
                    "score": 0,
                    "coords": [10172, 11150],
                    "type": "ruin",
                    "fileApiBase":"wvw_battles_hollow_",
                    "name": {
                        "de": "Schlachten-Senke",
                        "en": "Battle's Hollow",
                        "es": "Hondonada de la Battalla",
                        "fr": "Vallon de bataille"
                    }
                },
                64: {
                    "score": 0,
                    "coords": [10279, 10787],
                    "type": "ruin",
                    "fileApiBase":"wvw_bauers_estate_",
                    "name": {
                        "de": "Bauers Anwesen",
                        "en": "Bauer's Estate",
                        "es": "Hacienda de Bauer",
                        "fr": "Domaine de Bauer"
                    }
                },
                65: {
                    "score": 0,
                    "coords": [10750, 10845],
                    "type": "ruin",
                    "fileApiBase":"wvw_orchard_overlook_",
                    "name": {
                        "de": "Obstgarten-Aussichtspunkt",
                        "en": "Orchard Overlook",
                        "es": "Mirador del Huerto",
                        "fr": "Belvédère du Verger"
                    }
                },
                66: {
                    "score": 0,
                    "coords": [10779, 11196],
                    "type": "ruin",
                    "fileApiBase":"wvw_carvers_ascent_",
                    "name": {
                        "de": "Aufstieg des Schnitzers",
                        "en": "Carver's Ascent",
                        "es": "Ascenso del Trinchador",
                        "fr": "Côte du couteau"
                    }
                }
            }
        }
    };

	var GW2Info = {
		isInit: false,
		init: function(callback,options)
		{
            //localStorage.clear();
            //deleteDatabase();
            console.log("GW2Info: init");
            if (typeof options === 'undefined') options = {};
            GW2Info.options = options;

            var doAjaxRequest = function(callback,file,language,isLocalFallback)
            {
                if ((file !== 'files') && (!language)) language = 'en';
                var url;
                if (isLocalFallback)
                {
                    if (!options.localFallbackUrl)
                        callback(false);

                    url = options.localFallbackUrl + file + (language ? ('_' + language):'') + '.json';
                }
                else
                {
                    var url_base = 'https://api.guildwars2.com/v1/' + file + '.json';
                    url = language ? (url_base + '?lang=' + language) : url_base;
                }

                console.log('Requesting ' + url);
                new Ajax.Request(url, {
                    method:'get',
                    requestHeaders: {Accept: 'application/json'},
                    onSuccess: function(request) {
                        console.log('GW2Info: Received ' + file);
                        callback(request.responseJSON);
                    },
                    onFailure: function(response) {
                        console.log('GW2Info: ajax requesting gw2 api failure (' + file + ')');
                        callback(false);
                    }
                });
            };

            // data
            var dbDesiredVersion = 1;
            var db;
            var dbrequest = indexedDB.open('gw2info', dbDesiredVersion);
            // Create/Change-Event
            var dbDoUpgrade = function(oldVersion,trans)
            {
                var store;
                if(!db.objectStoreNames.contains('build_ids')){
                    store = db.createObjectStore('build_ids', {
                                                     //keyPath: 'key',
                                                     //autoIncrement: true
                                                 });
                };
                if(!db.objectStoreNames.contains('files')){
                    store = db.createObjectStore('files', {});
                };
                if(!db.objectStoreNames.contains('wvw_matches')){
                    store = db.createObjectStore('wvw_matches', {
                                                     keyPath: "wvw_match_id"
                                                 });
                };
                if(!db.objectStoreNames.contains('continents')){
                    store = db.createObjectStore('continents', {});
                };
                if(!db.objectStoreNames.contains('world_names')){
                    store = db.createObjectStore('world_names', {
                                                     keyPath: "id"
                                                 });
                };
                if(!db.objectStoreNames.contains('maps')){
                    store = db.createObjectStore('maps', {});
                };
                if(!db.objectStoreNames.contains('event_details')){
                    store = db.createObjectStore('event_details', {});
                };
                if(!db.objectStoreNames.contains('map_floor')){
                    store = db.createObjectStore('map_floor', {
                                                     //keyPath: "id"
                                                 });
                };
            }

            dbrequest.onupgradeneeded = function(event)
            {
                console.log('dbrequest.onupgradeneeded');
                db = this.result;
                // This version of chrome supports the new API and the version was less than 9, or a string.
                dbDoUpgrade(event.oldVersion, event.target.transaction);
            };

            // Open-Event (fires after upgradeneeded)
            dbrequest.onsuccess = function(event){
                console.log('db opened successfully');
                db = this.result || event.target.result;

                if (db.version === dbDesiredVersion) {
                    onDbInit();
                } else {
                    var oldVersion = db.version;
                    // This version of chrome doesn't support the new API.
                    var versionChangeRequest = db.setVersion(dbDesiredVersion.toString());
                    versionChangeRequest.onsuccess = function (event) {
                        var versionChangeTransaction = versionChangeRequest.result;
                        dbDoUpgrade(oldVersion, versionChangeTransaction);
                        versionChangeTransaction.oncomplete = function(event) {
                            db = event.target.db;
                            onDbInit();
                        }
                    }
                };
            };
            dbrequest.error = function(e){
                console.log("Database error: " + e.target.errorCode);
            };

            var getTransaction = function(store_names, isNeedWrite)
            {
                //console.log('get store');
                //console.log(db ? 'have db': 'have not db');
                if (isNeedWrite)
                    isNeedWrite = IDBTransaction.READ_WRITE || 'readwrite';
                else
                    isNeedWrite = 'readonly';

                //console.log(isNeedWrite);
                return db.transaction(store_names, isNeedWrite);
            }

            var getStore = function(store_name, isNeedWrite)
            {
                var trans = getTransaction(store_name,isNeedWrite);
                //console.log(trans ? 'Have trans' : 'Have no trans');
                return trans.objectStore(store_name);
            }

            var requestFromDbByKey = function(store_name, key, callback, args)
            {
                //Todo: type checks
                var trans = db.transaction([store_name]);
                var store = trans.objectStore(store_name);
                store.get(key).onsuccess = function(event) {
                    var result = event.target.result;
                    if (result == null) {
                        console.log('Key not found: '+ key + " in " + store_name);
                        callback(false,args);
                    }
                    else {
                        callback(result,args);
                    }
                };
            };

            var requestFromDbAll = function(store_name, callback, args)
            {
                //Todo: type checks
                var trans = db.transaction([store_name]);
                var store = trans.objectStore(store_name);

                // Cursor for all entries, 0 to finish
                var range = IDBKeyRange.lowerBound(0);
                var cursorRequest = store.openCursor(range);

                // Called once for every entry...and one time extra
                var obj = {};
                cursorRequest.onsuccess = function(evt){
                    var result = evt.target.result;
                    if(result)
                    {
                        obj[result.key] = result.value;
                        result.continue();
                    }
                    else
                        callback(((Object.keys(obj).length === 0) ? false : obj),args);
                };
            }

            /*var loadFromStorage = function()
			{
				return JSON.parse(localStorage.getItem('gw2info'));
			};
			
			var saveToStorage = function()
			{
				var stringy = JSON.stringify(GW2Info.data);
				localStorage.setItem('gw2info', stringy);
				console.log("Estimated localStorage size: " + (stringy.length * 2 / 1024 ) + "kB");
			};
			var data = loadFromStorage();
            if (!data) data = {};*/
            //var data = {};
			GW2Info.data = data;
			// /data
			
			// states
			var states = {
				invalid: 0,
				valid: 1,
				updating: 2,
				ok: 3,
				updated:4
			};
			GW2Info.states = states;
			var status = {
				build_id : states.updating,
				files: options.disable_files ? states.invalid : states.updating,
				continents : options.disable_continents ? states.invalid : states.updating,
                //world_names : options.disable_worldnames ? states.invalid : states.updating,
				maps : options.disable_maps ? states.invalid : states.updating,
                event_details : options.disable_eventdetails ? states.invalid : states.updating,
				wvw_matches : options.disable_matches ? states.invalid : states.updating
			};
			GW2Info.status = status;
			// /states
			
			var addLanguages = ['de','es','fr'];
			
            var checkFireInit = function()
			{
				//for each in status != states.updating;
				if ((status.build_id != states.updating)
                    //&& (status.files != states.updating)
                    //&& (status.wvw_matches != states.updating)
                    //&& (status.continents != states.updating)
                    //&& (status.world_names != states.updating)
                    //&& (status.maps != states.updating)
                    //&& (status.event_details != states.updating)
                        )
				{
					GW2Info.isInit = true;
                    //saveToStorage();
					console.log("GW2Info: init done");
					console.log("Status: build_id:"+status.build_id+" files:"+status.files+" wvw_matches:"+status.wvw_matches+" continents:"+status.continents+" world_names:"+status.world_names+" maps:"+status.maps);
					callback();
				}
			};
			
            var onDbInit = function()
            {

                var updateDbEntry = function(callback, store_name, responseJSON, ool_key)
                {
                    var store = getStore(store_name, true);
                    var key;
                    if (ool_key)
                        key = ool_key;
                    else if (store.keyPath)	// NOTE: This apparently doesn't work with the indexedDbShim polyfill; use ool
                        key = responseJSON[store.keyPath];
                    else
                    {
                        console.log('No access to key, aborting.');
                        callback(false);
                        return;
                    };

                    store.get(key).onsuccess = function(event) {
                        var result = event.target.result;
                        console.log('updateDbEntry: Key ' + (result ? '' : 'not ') + 'found: ' + key + ' in ' + store_name);
                        if(!result)
                            result = {};

                        result = $H(result).merge(responseJSON).toObject();

                        var dbputrequest = ool_key ? (store.put(result, key)):(store.put(result));
                        dbputrequest.onsuccess = function(evt){
							console.log('updateDbEntry: Key ' + key + ' added to '  + store_name);
                            callback(result);
                        };
                    };
                };

                var updateDbEntries = function(callback, store_name, responseJSON, inlineKeyPath)
                {
                    var obj = {};

                    //Todo: type checks
                    var trans = getTransaction([store_name], true);
                    var store = trans.objectStore(store_name);

                    trans.oncomplete = function() {
                        // All requests have succeeded and the transaction has committed.
                        console.log(store_name + ' transaction completed');
                        callback(obj);
                    };

                    var doUpdateForEntry = function(key, value)
                    {
                        store.get(key).onsuccess = function(event) {
                            var result = event.target.result;
                            //console.log('Key ' + (result ? '' : 'not ') + 'found: ' + key + ' in ' + store_name);
                            if(!result)
                                result = {};

                            result = $H(result).merge(value).toObject();

                            var dbputrequest = inlineKeyPath ? (store.put(result)) : (store.put(result, key));
                            dbputrequest.onsuccess = function(evt){
                                //console.log('Entry ' + evt.target.result + ' saved.');
                            };
                            obj[key] = result;
                        };
                    }

                    if (inlineKeyPath)  //using inline iff array instead of obj
                    {
                        for (var i = 0, len = responseJSON.length; i < len; i++) {
                            doUpdateForEntry(responseJSON[i][inlineKeyPath],responseJSON[i]);
                        }
                    }
                    else
                    {
                        $H(responseJSON).each(function(e){
                            doUpdateForEntry(e[0],e[1]);
                        });
                    }

                    /*$H(responseJSON).each(function(e){
                        store.get(e[0]).onsuccess = function(event) {
                            var result = event.target.result;
                            //console.log('Key ' + (result ? '' : 'not ') + 'found: ' + e[0] + ' in ' + store_name);
                            if(!result)
                                result = {};

                            result = $H(result).merge(e[1]).toObject();

                            var dbputrequest = isInline ? (store.put(result)) : (store.put(result, e[0]));
                            dbputrequest.onsuccess = function(evt){
                                //console.log('Entry ' + evt.target.result + ' saved.');
                            };
                            obj[e[0]] = result;
                        };
                    });*/
                };

                var bulkAddToDb = function(store_name,obj,language)
                {
                    var trans = getTransaction([store_name,'build_ids'], true);
                    var store = trans.objectStore(store_name);
                    var request;

                    if ((store_name === 'files') || (store_name === 'continents') || (store_name === 'event_details'))
                    {
                        $H(obj).each(function(e){
                            request = store.put(e[1],e[0]); // value,key
                            request.onsuccess = function(evt){
                                console.log('Entry ' + evt.target.result + ' saved.');
                            };
                        });
                    }
                    else if (store_name === 'wvw_matches')
                    {
                        for (var index = 0, len = obj.length; index < len; ++index) {
                            var item = obj[index];
                            request = store.put(item);
                            request.onsuccess = function(evt){
                                console.log('Entry ' + evt.target.result + ' saved.');
                            };
                        }
                    }
                    else if (store_name === 'map_floor')
                    {
                        request = store.put(obj);
                        request.onsuccess = function(evt){
                            console.log('Entry ' + evt.target.result + ' saved.');
                        };
                    };

                    if ((store_name !== 'wvw_matches') && data.build_id)
                    {
                        store = trans.objectStore('build_ids');
                        request = store.put(data.build_id,((language) ? (store_name + '_' + language) : store_name)); // value,key
                        request.onsuccess = function(evt){
                            console.log('Entry ' + evt.target.result + ' saved.');
                        };
                    }
                };

                /********************************************************************************/
                // Files, Continents, World Names
                // Here we're using the build id to determine if cache needs update
                /********************************************************************************/


                GW2Info.requestEventDetails = function(callback,language)
                {
                    if (!language) language = 'en';
                    var file = 'event_details';
                    var cleanup = function(obj,language)
                    {
                        if (typeof obj === 'object')
                        {
                            $H(obj).each(function(e){
                                if ((e[0]) && (e[1]))
                                {
                                    if (e[1].name)
                                    {
                                        e[1]['name_' + language] = e[1].name;
                                        delete e[1].name;
                                    }
                                }
                            });
                        };
                    };

                    // try database
                    requestFromDbByKey('build_ids', file + '_' + language, function(resultBuildId){
                        if (resultBuildId !== data.build_id)
                        {
                            console.log(file + '_' + language + ' outdated or not present.');
                            doAjaxRequest(function(responseJSON){
                                if(responseJSON)
                                {
                                    responseJSON = responseJSON.events;
                                    cleanup(responseJSON,language);

                                    updateDbEntries(function(obj){
                                        var store = getStore('build_ids', true);
                                        var dbputrequest = store.put(data.build_id,file + '_' + language);
                                        dbputrequest.onsuccess = function(evt){
                                            console.log('Entry ' + evt.target.result + ' saved.');
                                            callback(obj);
                                        };
                                    }, file, responseJSON, false);
                                }
                                else
                                    callback(false);
                            },file,language);
                        }
                        else
                        {
                            console.log(file + '_' + language + ' up to date.');
                            var store = getStore(file);
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var obj = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.key;
                                    obj[key] = result.value;
                                    //obj[key].name = obj[key]["name_" + language];
                                    result.continue();
                                }
                                else
                                    callback(obj);
                            };
                        };
                    });
                }

                GW2Info.requestMaps = function(callback,language)
                {
                    if (!language) language = 'en';
                    var file = 'maps';
                    var cleanup = function(obj,language)
                    {
                        var returnObj = {};
                        if (typeof obj === 'object')
                        {
                            $H(obj.maps).each(function(e){
                                if ((e[0]) && (e[1]))
                                {
                                    if (e[1].map_name){
                                        e[1]['name_' + language] = e[1].map_name;
                                        delete e[1].map_name;
                                    };
                                    if (e[1].region_name)
                                        delete e[1].region_name;
                                    if (e[1].continent_name)
                                        delete e[1].continent_name;

                                    var keyInt = parseInt(e[0]);
                                    returnObj[keyInt] = e[1];
                                }
                            });
                        };
                        return returnObj;
                    };

                    // try database
                    requestFromDbByKey('build_ids', file + '_' +language, function(resultBuildId){
                        if (resultBuildId !== data.build_id)
                        {
                            console.log(file + '_' + language + ' outdated or not present.');
                            doAjaxRequest(function(responseJSON){
                                if(responseJSON)
                                {
                                    responseJSON = cleanup(responseJSON,language);

                                    updateDbEntries(function(obj){
                                        var store = getStore('build_ids', true);
                                        var dbputrequest = store.put(data.build_id,file + '_' + language);
                                        dbputrequest.onsuccess = function(evt){
                                            console.log('Entry ' + evt.target.result + ' saved.');
                                            callback(obj);
                                        };
                                    }, file, responseJSON, false);
                                }
                                else
                                    callback(false);
                            },file,language);
                        }
                        else
                        {
                            console.log(file + ' up to date.');
                            requestFromDbAll(file, function(dBresult, args){
                                callback(dBresult);
                            });
                        };
                    });
                }

                GW2Info.requestWorldNames = function(callback,language)
                {
                    if (!language) language = 'en';
                    var file = 'world_names';
                    var cleanup = function(obj,language)
                    {
                        if (typeof obj === 'object')
                        {
                            if (Array.isArray(obj))
                            {
                                for (var i = 0, len = obj.length; i < len; i++) {
                                    if (obj[i].name)
                                    {
                                        obj[i]['name_' + language] = obj[i].name;
                                        obj[i].id = parseInt(obj[i].id);
                                        delete obj[i].name;
                                    }
                                }
                            };
                        };
                    }

                    // try database
                    requestFromDbByKey('build_ids', file + '_' + language, function(resultBuildId){
                        if (resultBuildId !== data.build_id)
                        {
                            console.log(file + '_' + language + ' outdated or not present.');
                            doAjaxRequest(function(responseJSON){
                                if(responseJSON)
                                {
                                    cleanup(responseJSON,language);

                                    // update db
                                    updateDbEntries(function(obj){
                                        var store = getStore('build_ids', true);
                                        var dbputrequest = store.put(data.build_id,file + '_' + language);
                                        dbputrequest.onsuccess = function(evt){
                                            console.log('Entry ' + evt.target.result + ' saved.');
                                            callback(obj);
                                        };
                                    }, file, responseJSON, "id");
                                }
                                else
                                    callback(false);
                            },file,language);
                        }
                        else
                        {
                            console.log(file + '_' + language + ' up to date.');
                            var store = getStore(file);
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var obj = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.key;
                                    obj[key] = result.value;
                                    //obj[key].name = obj[key]["name_" + language];
                                    result.continue();
                                }
                                else
                                    callback(obj);
                            };
                        };
                    });
                };

                GW2Info.requestContinents = function(callback,language)
                {
                    if (!language) language = 'en';
                    var file = 'continents';
                    var cleanup = function(obj,language)
                    {
                        var returnObj = {};
                        if (typeof obj === 'object')
                        {
                            $H(obj).each(function(e){
                                if ((e[0]) && (e[1]))
                                {
                                    if (e[1].name)
                                    {
                                        e[1]['name_' + language] = e[1].name;
                                        delete e[1].name;
                                    };
                                    var keyInt = parseInt(e[0]);
                                    returnObj[keyInt] = e[1];
                                }
                            });
                        };
                        return returnObj;
                    };

                    // try database
                    requestFromDbByKey('build_ids', file + '_' +language, function(resultBuildId){
                        if (resultBuildId !== data.build_id)
                        {
                            console.log('continents outdated or not present.');
                            doAjaxRequest(function(responseJSON){
                                if(responseJSON)
                                {
                                    responseJSON = responseJSON.continents;
                                    responseJSON = cleanup(responseJSON,language);

                                    updateDbEntries(function(obj){
                                        var store = getStore('build_ids', true);
                                        var dbputrequest = store.put(data.build_id,file + '_' + language);
                                        dbputrequest.onsuccess = function(evt){
                                            console.log('Entry ' + evt.target.result + ' saved.');
                                            callback(obj);
                                        };
                                    }, file, responseJSON, false);
                                }
                                else
                                    callback(false);
                            },file,language);
                        }
                        else
                        {
                            console.log(file + ' up to date.');
                            requestFromDbAll(file, function(dBresult, args){
                                callback(dBresult);
                            });
                        };
                    });
                };

                GW2Info.requestFiles = function(callback)
                {
                    // try database
                    requestFromDbByKey('build_ids', 'files', function(result){
                        if (result !== data.build_id)
                        {
                            console.log('files outdated or not present.');
                            doAjaxRequest(function(responseJSON){
                                if(responseJSON)
                                {
                                    status.files = states.updated;
                                    bulkAddToDb('files', responseJSON);
                                    callback(responseJSON);
                                }
                                else
                                    callback(false);
                            },'files');
                        }
                        else
                        {
                            console.log('files up to date.');
                            var store = getStore('files');
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var files = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.key;
                                    files[key] = result.value;
                                    result.continue();
                                }
                                else
                                    callback(files);
                            };
                        };
                    });
                };

                //************
                var onNewBuild = function(build_id){
                    var store = getStore('build_ids',true);
                    data.build_id = build_id ? build_id : 0;
                    status.build_id = (data.build_id)?states.updated:states.invalid;
                    console.log("onNewBuild: " + data.build_id);
                    /*console.log(store ? "still have store" : "store gone");
                    for (var i = 0,len = db.objectStoreNames.length; i < len; i++)
                    {
                        console.log(db.objectStoreNames.item(i));
                    }*/

                    var dbputrequest = store.put((build_id ? build_id : 0),'_current');
                    dbputrequest.onsuccess = function(evt){
                        console.log('Entry ' + evt.target.result + ' saved.');
                        checkFireInit();
                    };
                    /*if (!options.disable_files)
                        GW2Info.requestFiles(function(result){console.log("Files: Request completed.");});
                    if (!options.disable_continents)
                        GW2Info.requestContinents(function(result){console.log("Continents: Request completed.");},'en');
                    if (!options.disable_worldnames)
                        GW2Info.requestWorldNames(function(result){console.log("World Names: Request completed.");},'en');
                    if (!options.disable_maps)
                        GW2Info.requestMaps(function(result){console.log("Maps: Request completed.");},'en');*/
                    /*if (!options.disable_eventdetails)
                        GW2Info.requestEventDetails(function(result){console.log("Event Details: Request completed.");},'en');*/
                };

				requestFromDbByKey('build_ids', '_current', function(result){
					if (result)
						data.build_id = result;
						
					new Ajax.Request('https://api.guildwars2.com/v1/build.json', {
                        method:'get',
                        requestHeaders: {Accept: 'application/json'},
                        onSuccess: function(request) {
                            var build_id = request.responseJSON.build_id;
                            console.log('GW2Info: Received build id: ' + build_id);
                            console.log('GW2Info: Build Storage: ' + ((typeof data.build_id === 'undefined') ? 'none' : data.build_id));
		                    							
                            if (build_id !== data.build_id)
                            {
                                status.build_id = states.updated;
                                onNewBuild(build_id);
                            }
                            else
                            {
                                status.build_id = states.ok;
                                checkFireInit();
                            };
                        },
                        onFailure: function(response) {
                            status.build_id = (data.build_id) ? states.valid : states.invalid;
                            console.log('GW2Info: ajax requesting gw2 api failure (build_id)');
                            checkFireInit();
                        }
                    });
				});

                /********************************************************************************/
                // WvW Matches
                // Here we're using the end date to determine if cache needs update
                /********************************************************************************/
                GW2Info.requestWvWMatches = function(callback)
                {
                    //Do we need to update matches?
                    var store_name = 'wvw_matches';
                    /*var store = getStore(store_name);
                    var testkey = "1-1";
                    store.get(testkey).onsuccess = function(event) {
                        var needUpdate = true;
                        var result = event.target.result;
                        if (result)
                        {
                            console.log("GW2Info: WvW Match ends: " + result.end_time);
                            //var endTime = new Date(data.wvw_matches[0].end_time);
                            var endTime = new Date(result.end_time);
                            var d = new Date();
                            if (d<endTime)
                            {
                                needUpdate = false;
                                //Todo: At some point some schedule magic could be implemented to auto-update on match change
                                console.log('GW2Info: WvW Matches are still ok.');
                                status.wvw_matches = states.ok;
                            }
                            else
                                console.log('Match too old, ends '+ endTime.toDateString() + "; current time: " + d.toDateString() + ". Updating...");
                        }
                        else
                            console.log('Key not found: '+ testkey + " in " + store_name + ". Updating...");*/

                    requestFromDbAll(store_name, function(result){
                        var needUpdate = true;
                        if (result)
                        {
                            data.wvw_matches = result;
                            var endTime = new Date(result["1-1"].end_time);
                            var d = new Date();
                            if (d<endTime)
                            {
                                needUpdate = false;
                                //Todo: At some point some schedule magic could be implemented to auto-update on match change
                                console.log('GW2Info: WvW Matches are still ok.');
                                status.wvw_matches = states.ok;
                                callback(data.wvw_matches);
                            }
                            else
                                console.log('Match too old, ends '+ endTime.toDateString() + "; current time: " + d.toDateString() + ". Updating...");
                        }
                        else
                            console.log('No matches in db. Updating...');

                        if (needUpdate)
                        {
                            new Ajax.Request('https://api.guildwars2.com/v1/wvw/matches.json', {
                                method:'get',
                                requestHeaders: {Accept: 'application/json'},
                                onSuccess: function(request) {
                                    data.wvw_matches = request.responseJSON.wvw_matches;
                                    status.wvw_matches = states.updated;
                                    console.log('GW2Info: Received WvW Matches.');
                                    bulkAddToDb('wvw_matches', request.responseJSON.wvw_matches);
                                    callback(data.wvw_matches);
                                },
                                onFailure: function(response) {
                                    delete data.wvw_matches;
                                    status.wvw_matches = states.invalid;
                                    console.log('GW2Info: ajax requesting gw2 api failure (matches)');
                                    callback(false);
                                }
                            });
                        };
                    }, false);  //args
                };

                // will be cached, but only after first request and until build_id changes
                // localized names in here too that are available no where else.
                // The way localization is handled in the API is sort of the opposite of what I like -.-
                GW2Info.requestMapFloor = function(callback, language, continent_id, floor_id)
                {
                    if (!GW2Info.isInit) return false;
                    if (!language) language = 'en';

                    var file = 'map_floor';
                    var cleanup = function(responseJSON,language)
                    {
                        if (typeof responseJSON === 'object')
                        {
                            var regions = {};
                            $H(responseJSON.regions).each(function(e){
                                if (e[0])
                                    regions[parseInt(e[0])] = e[1];
                                var maps = {};
                                $H(e[1].maps).each(function(e){
                                    if (e[0])
                                        maps[parseInt(e[0])] = e[1];
                                });
                                regions.maps = maps;
                            });
                            responseJSON.regions = regions;

                            // localize
                            var localize = function(obj)
                            {
                                if (obj.name)
                                {
                                    obj['name_'+language] = obj.name;
                                    delete obj.name;
                                }
                            };

                            $H(responseJSON.regions).each(function(e){
                                if ((e[0]) && (e[1])) {
                                    localize(e[1]);
                                    $H(e[1].maps).each(function(f){
                                        if (f[1]) {
                                            localize(f[1]);
                                            $A(["points_of_interest","skill_challenges","sectors"]).each(function(g){
                                                if (f[1][g]) {
                                                    $A(f[1][g]).each(function(h){
                                                        localize(h);
                                                    });
                                                };
                                            });
                                            if (f[1].tasks)
                                            {
                                                $A(f[1].tasks).each(function(g)
                                                {
                                                    if (g.objective)
                                                    {
                                                        g['name_'+language] = g.objective;
                                                        delete g.objective;
                                                    }
                                                });
                                            }
                                        };
                                    });
                                };
                            });
                        };
                        return responseJSON;
                    };

                    // try database
                    requestFromDbByKey('build_ids', (file + '_' +language + '_' + continent_id + '_' + floor_id), function(resultBuildId){
                        if (resultBuildId !== data.build_id)
                        {
                            console.log(file + '_' +language + '_' + continent_id + '_' + floor_id + ' outdated or not present.');
                            var url = 'https://api.guildwars2.com/v1/map_floor.json?continent_id=' + continent_id + '&floor=' + floor_id + '&lang=' + language;
                            new Ajax.Request(url, {
                                method:'get',
                                requestHeaders: {Accept: 'application/json'},
                                onSuccess: function(request) {
                                    // we don't need map "name", "min_level", "max_level", "default_floor", "map_rect", "continent_rect" a gazillion times, it's in our maps.json -.-
                                    var responseJSON = request.responseJSON;
                                    responseJSON = cleanup(responseJSON,language);
                                    responseJSON.continent_id = continent_id;
                                    responseJSON.floor_id = floor_id;
                                    //responseJSON.id = '' + continent_id + '_' + floor_id;

                                    updateDbEntry(function(obj){
                                        var store = getStore('build_ids', true);
                                        var dbputrequest = store.put(data.build_id, (file + '_' +language + '_' + continent_id + '_' + floor_id));
                                        dbputrequest.onsuccess = function(evt){
                                            console.log('Entry ' + evt.target.result + ' saved.');
                                            callback(obj);
                                        };
                                    }, file, responseJSON, ('' + continent_id + '_' + floor_id));	//false);

                                    //data.map_floor[continent_id][floor_id] = responseJSON;
                                    callback(responseJSON);
                                },
                                onFailure: function(response) {
                                    console.log('GW2Info: ajax requesting gw2 api failure ('+url+')');
                                    callback(false);
                                }
                            });
                        }
                        else
                        {
                            console.log(file + '_' +language + '_' + continent_id + '_' + floor_id + ' up to date.');
							
							// This doesn't work with IndexedDbShim on IOS
							// Todo: detect
                            requestFromDbByKey(file, ('' + continent_id + '_' + floor_id), function(dBresult){
                                callback(dBresult);
                            });
							/*requestFromDbAll(file,function(result){
								if (typeof result === "undefined")
									console.log("requestMapFloor: undefined db result");
								else if (result === false)
									console.log("requestMapFloor: false db result");
									
								callback(result['' + continent_id + '_' + floor_id]);
							}, false);  //args
							*/
                        };
                    });

                    /*if (data.map_floor && data.map_floor[continent_id] && data.map_floor[continent_id][floor_id] && data.map_floor[continent_id][floor_id].languages[language])
                        callback(data.map_floor[continent_id][floor_id]);		//return data.map_floor[continent_id][floor_id];	// Fire event!
                    else
                    {
                        if (!data.map_floor) data.map_floor = {};
                        if (!data.map_floor[continent_id]) data.map_floor[continent_id] = {};
                        if (!data.map_floor[continent_id][floor_id]) data.map_floor[continent_id][floor_id] = {};


                    };*/
                };
            };
		},
		
        // no caching, event style
        registerForMatchDetails : function(callback, context, match_id)
		{
            var cleanup = function(responseJSON)
            {
                var obj = {};
                $A(responseJSON.maps).each(function(e){
                    var objObj = {};
                    $A(e.objectives).each(function(f){
                        objObj[f.id] = f;   // coerced to string. very intuitive js..
                        //delete f.id;
                    });
                    e.objectives = objObj;

                    if (e.bonuses)
                    {
                        objObj = {};    //recycle
                        $A(e.bonuses).each(function(f){
                            objObj[f.type] = f.owner;
                        });
                        e.bonuses = objObj;
                    };

                    obj[e.type] = e;
                    //delete e.type;
                });
                responseJSON.maps = obj;
            };

            var diffArrayScores = function(last,current)
            {
                if (!last.scores)
                    return;

                var isChanged = false;
                for (var i = 0,len = current.scores.length; i<len;i++)
                {
                    if (current.scores[i] !== last.scores[i])
                    {
                        isChanged = true;
                        break;
                    }

                };
                if (!isChanged)
                    delete current.scores;
            }

            var lastMatchDetail = {};
            var getDiffMatchDetail = function(responseJSON)
            {
                var clone = JSON.parse(JSON.stringify(responseJSON));
                diffArrayScores(lastMatchDetail,clone);

                if (lastMatchDetail.maps)
                {
                    $H(clone.maps).each(function(e){
                        diffArrayScores(lastMatchDetail.maps[e[0]],e[1]);

                        $H(e[1].objectives).each(function(f){
                            if ((f[1].owner === lastMatchDetail.maps[e[0]].objectives[f[0]].owner) && (f[1].guild === lastMatchDetail.maps[e[0]].objectives[f[0]].guild))
                                delete e[1].objectives[f[0]];  // Not sure if the iterator can handle that
                        });

                        $H(e[1].bonuses).each(function(f){
                            if (f[1].owner === lastMatchDetail.maps[e[0]].bonuses[f[0]].owner)
                                delete e[1].bonuses[f[0]];  // Not sure if the iterator can handle that
                        });
                        $A(['objectives','bonuses']).each(function(f){
                            if ($H(e[1][f]).keys().length === 0)
                                delete e[1][f];
                        });

                        if ($H(e[1]).keys().length === 1)   //"type":"RedHome"
                            delete clone.maps[e[0]];        // Not sure if the iterator can handle that
                    });

                    if ($H(clone.maps).keys().length === 0)
                        delete clone.maps;
                };
				
                if ($H(clone).keys().length === 1)  //"match_id":"2-2"
                    clone = false;

                lastMatchDetail = responseJSON;
                return clone;
            };

            publisher.on(match_id,callback,context);
            if ((publisher.subscribers[match_id].length) === 1)
            {
                new PeriodicalExecuter( function(pe) {
                    var url = 'https://api.guildwars2.com/v1/wvw/match_details.json?match_id=' + match_id;
                    new Ajax.Request(url, {
                        method:'get',
                        requestHeaders: {Accept: 'application/json'},
                        onSuccess: function(request) {
                            if (publisher.subscribers[match_id].length === 0)
                                pe.stop();
                            else
                            {
                                var response = request.responseJSON;
                                cleanup(response);
                                //console.log(JSON.stringify(response.maps));
                                //console.log("Getting Match Detail DIFF");
                                var matchDetailsDiff = getDiffMatchDetail(response);
                                if (matchDetailsDiff)
                                {
                                    //console.log(JSON.stringify(matchDetailsDiff));
                                    response._diff = matchDetailsDiff;
                                }
                                /*else
                                    console.log("No change.");*/

                                publisher.fire(match_id,response);
                            }
                        },
                        onFailure: function(response) {
                            console.log('GW2Info: ajax requesting gw2 api failure ('+url+')');
                            if (publisher.subscribers[match_id].length === 0)
                                pe.stop();
                        }
                    });
                }, 5);
            };
        },
        unregisterForMatchDetails : function(callback, context, match_id)
        {
            publisher.remove(match_id,callback,context);
        },

        registerForEvents : function(callback, context, world_id, map_id, event_id)
        {
            var cleanup = function(responseJSON)
            {
				// Do we want that?
				/*var obj = {};
                $A(responseJSON.events).each(function(e){
					if (!obj[e.world_id]) obj[e.world_id] = {};
					if (!obj[e.world_id][e.map_id]) obj[e.world_id][e.map_id] = {};
					obj[e.world_id][e.map_id][e.event_id] = e.state;
                });
                responseJSON = obj;*/
				responseJSON = responseJSON.events;
            };

            var baseUrl = 'https://api.guildwars2.com/v1/events.json'   //?world_id=2005&event_id=AFCF031A-F71D-4CEA-85E1-957179414B25';
            var urlOptions = [
                        world_id ? ('world_id='+world_id) : false,
                                   map_id ? ('map_id=' + map_id) : false,
                                            event_id ? ('event_id=' + event_id) : false];
            var url = baseUrl;
            $A(urlOptions).each(function(e){
                if (e)
                {
                    if (url === baseUrl)
                        url = url + '?' + e;
                    else
                        url = url + '&' + e;
                };
            });
            publisher.on(url,callback,context);

            new PeriodicalExecuter( function(pe) {
                new Ajax.Request(url, {
                    method:'get',
                    requestHeaders: {Accept: 'application/json'},
                    onSuccess: function(request) {
                        if (publisher.subscribers[url].length === 0)
                            pe.stop();
                        else
                        {
                            var response = request.responseJSON;
                            cleanup(response);
                            publisher.fire(url,response);
                        }
                    },
                    onFailure: function(response) {
                        console.log('GW2Info: ajax requesting gw2 api failure ('+url+')');
                        if (Array.length(publisher.subscribers[url]) === 0)
                            pe.stop();
                    }
                });
            }, 13);
        },
        unregisterForEvents : function(callback, context, world_id, map_id, event_id)
        {
            var baseUrl = 'https://api.guildwars2.com/v1/events.json'   //?world_id=2005&event_id=AFCF031A-F71D-4CEA-85E1-957179414B25';
            var urlOptions = [
                        world_id ? ('world_id='+world_id) : false,
                                   map_id ? ('map_id=' + map_id) : false,
                                            event_id ? ('event_id=' + event_id) : false];
            var url = baseUrl;
            $A(urlOptions).each(function(e){
                if (e)
                {
                    if (url === baseUrl)
                        url = url + '?' + e;
                    else
                        url = url + '&' + e;
                };
            });
            publisher.remove(url,callback,context);
        }
    }
	global.GW2Info = GW2Info;
})();
