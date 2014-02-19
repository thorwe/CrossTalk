/**
 * gw2Maps.js
 * created: 21.06.13
 *
 * GW2 Location by Smiley
 *
 * based on Cliff's example
 * http://jsfiddle.net/cliff/CRRGC/
 *
 * and Dr. Ishmaels proof of concept
 * http://wiki.guildwars2.com/wiki/User:Dr_ishmael/leaflet
 *
 * requires:
 *
 * - intval from phpjs (included)
 */

// Enable XSS... errr... CORS for Prototype: http://kourge.net/node/131
// the console will tell you: Refused to get unsafe header "X-JSON"
Ajax.Responders.register({
	onCreate: function(response) {
		var t = response.transport;
		t.setRequestHeader = t.setRequestHeader.wrap(function(original, k, v) {
			if (/^(accept|accept-language|content-language)$/i.test(k)){
				return original(k, v);
			}
			if (/^content-type$/i.test(k) && /^(application\/x-www-form-urlencoded|multipart\/form-data|text\/plain)(;.+)?$/i.test(v)){
				return original(k, v);
			}
			return null;
		});
	}
});


var GW2Maps = {
	init: function(container){
		if(typeof container !== "object"){
			return false;
		}
		google.maps.visualRefresh = true;
		var options = GW2Maps.options(container),
//			ll2p = function(latlng){
//				return GW2Maps.fromLatLngToPoint(latlng, options.max_zoom);
//			},
			p2ll = function(point){
				return GW2Maps.fromPointToLatLng(point, options.max_zoom);
			},
			mapobject = {
				continent: options.continent_id,
				linkbox: new Element("div", {"class":"linkbox"}).setStyle({"width": options.linkbox, "height": options.height}),
				map: new google.maps.Map(container, {
					zoom: 3,
					minZoom: 1,
					maxZoom: 7,
					center: new google.maps.LatLng(0,0),
					streetViewControl: false,
					panControl: options.map_controls,
					zoomControl: options.map_controls,
					mapTypeId: options.continent_id.toString(), // string for gmaps' sake
					mapTypeControlOptions: {
						mapTypeIds: ["1","2"]
					}
				}),
				layers: {},
				markers: {},
				popups: [],
				options: options
			},
			get_tile = function(coords,zoom){
				if(coords.y < 0 || coords.x < 0 || coords.y >= (1 << zoom) || coords.x >= (1 << zoom)){
					return options.i18n.errortile;
				}
				return "https://tiles.guildwars2.com/"+mapobject.map.getMapTypeId()+"/1/"+zoom+"/"+coords.x+"/"+coords.y+".jpg";
			},
			tile_size = new google.maps.Size(256,256),
			// TODO: i18n
			tyria = new google.maps.ImageMapType({maxZoom: 7, alt: "Tyria", name: "Tyria", tileSize: tile_size, getTileUrl: get_tile}),
			mists = new google.maps.ImageMapType({maxZoom: 6, alt: "The Mists", name: "The Mists", tileSize: tile_size, getTileUrl: get_tile}),
			max_bounds = new google.maps.LatLngBounds(p2ll([0, (1<<options.max_zoom)*256]), p2ll([(1<<options.max_zoom)*256, 0]));

		mapobject.map.mapTypes.set("1",tyria);
		mapobject.map.mapTypes.set("2",mists);

		container.setStyle({"width": "100%", "height": "100%"});

		// lock panning
		google.maps.event.addListener(mapobject.map, "center_changed", function(){
			if(!max_bounds.contains(mapobject.map.getCenter())){
				var center = mapobject.map.getCenter(),
					lng = center.lng(),
					lat = center.lat(),
					lng_max = max_bounds.getNorthEast().lng(),
					lat_max = max_bounds.getNorthEast().lat(),
					lng_min = max_bounds.getSouthWest().lng(),
					lat_min = max_bounds.getSouthWest().lat();
				if(lng < lng_min){lng = lng_min;}
				if(lng > lng_max){lng = lng_max;}
				if(lat < lat_min){lat = lat_min;}
				if(lat > lat_max){lat = lat_max;}
				mapobject.map.panTo(new google.maps.LatLng(lat,lng));
			}
		});

		// return the mapobject for later use
		return mapobject;
	},

	/**
	 *
	 * @param ll
	 * @param max_zoom
	 * @returns {google.maps.Point}
	 */
	fromLatLngToPoint: function(ll, max_zoom){
		var point = new google.maps.Point(0, 0),
			origin = new google.maps.Point(128, 128),
			tiles = 1 << max_zoom,
			bound = function(value, min, max){
				if (min != null) value = Math.max(value, min);
				if (max != null) value = Math.min(value, max);
				return value;
			},
			sin_y = bound(Math.sin(ll.lat() * (Math.PI / 180)), -0.9999, 0.9999);
		point.x = origin.x + ll.lng() * (256 / 360);
		point.y = origin.y + 0.5 * Math.log((1 + sin_y) / (1 - sin_y)) * -(256 / (2 * Math.PI));
		return new google.maps.Point(Math.floor(point.x * tiles), Math.floor(point.y * tiles));
	},

	/**
	 *
	 * @param points
	 * @param max_zoom
	 * @returns {google.maps.LatLng}
	 */
	fromPointToLatLng: function(points, max_zoom) {
		var point = new google.maps.Point(points[0], points[1]),
			size = (1 << max_zoom) * 256,
			lat = (2 * Math.atan(Math.exp((point.y - size/2) / -(size/(2 * Math.PI)))) - (Math.PI / 2)) * (180/Math.PI),
			lng = (point.x - size/2) * (360/size);
		return new google.maps.LatLng(lat, lng);
	},

	/**
	 *
	 * dataset {
	 *     language: int (1=de, 2=en, 3=es, 4=fr),
	 *     continent_id: (1=Tyria ,2=The Mists),
	 *     floor_id: int,
	 *     region_id: non negative int,
	 *     map_id: non negative int,
	 *     poi_id: non negative int,
	 *     poi_type: int (1=landmark, 2=sector, 3=skill, 4=task, 5=vista, 6=waypoint),
	 *     disable_controls: bool,
	 *     width: non negative int,
	 *     w_percent: bool,
	 *     height: non negative int,
	 *     h_percent: bool
	 *     linkbox: non negative int >= 100
	 * }
	 *
	 * @param container
	 * @returns object
	 */
	options: function(container){
		// make sure that any dataset values are number - for wiki security reasons
		// (using filter type integer in the widget extension)
		// exception: the polyline will be a string of comma and space seperated number pairs
		// like: 16661,16788 17514,15935...
		// using preg_replace("#[^,;=\-\d\s\w]#", "", $str), so we need to check for valid pairs
		// i don't bother reading the elements dataset for compatibility reasons
		var dataset = {};
		$A(container.attributes).each(function(c){
			if(c.name.match(/^data-/)){
				dataset[c.name.substr(5)] = (c.name === "data-polyline") ? c.value : phpjs.intval(c.value);
			}
		});

		// check the option values and fall back to defaults if needed
		var lang = ["en","de","en","es","fr"], // 0 is the default language, change to suit your needs
			poi_types = [false, "landmark", "sector", "skill", "task", "vista", "waypoint"],
			continent_id = typeof dataset.continent_id === "number" && dataset.continent_id >=1 && dataset.continent_id <= 2 ? dataset.continent_id : 1;

		return {
			max_zoom: continent_id == 1 ? 7 : 6,
			continent_id: continent_id,
			floor_id: typeof dataset.floor_id === "number" ? dataset.floor_id : 2,
			region_id: typeof dataset.region_id === "number" && dataset.region_id > 0 ? dataset.region_id : false,
			map_id: typeof dataset.map_id === "number" && dataset.map_id > 0 ? dataset.map_id : false,
			poi_id: typeof dataset.poi_id === "number" && dataset.poi_id > 0 ? dataset.poi_id : false,
			poi_type: typeof dataset.poi_type === "number" && dataset.poi_type > 0 && dataset.poi_type <= 6 ? poi_types[dataset.poi_type] : false,
			width: typeof dataset.width === "number" && dataset.width > 0 ? dataset.width+(dataset.w_percent == true ? "%" : "px") : "800px",
			height: typeof dataset.height === "number" && dataset.height > 0 ? dataset.height+(dataset.h_percent == true ? "%" : "px") : "450px",
			map_controls: dataset.disable_controls != true,
			linkbox: false,
			polyline: dataset.polyline && dataset.polyline.length > 7 ? dataset.polyline : false,
			i18n: typeof dataset.language === "number" && dataset.language >=1 && dataset.language <= 4 ? GW2Maps.i18n[lang[dataset.language]] : GW2Maps.i18n[lang[0]]
		};
	},

	/**
	 *
	 */
	i18n: {
		de: {
			lang: "de",
			wiki: "http://wiki-de.guildwars2.com/wiki/",
			icon_event: {link: "http://wiki-de.guildwars2.com/images/7/7a/Event_Angriff_Icon.png", size: [24,24]},
			icon_landmark: {link: "http://wiki-de.guildwars2.com/images/0/0f/Sehenswürdigkeit_Icon.png", size: [16,16]},
			icon_skill: {link: "http://wiki-de.guildwars2.com/images/c/c3/Fertigkeitspunkt_Icon.png", size: [20,20]},
			icon_task: {link: "http://wiki-de.guildwars2.com/images/b/b7/Aufgabe_Icon.png", size: [20,20]},
			icon_vista: {link: "http://wiki-de.guildwars2.com/images/9/9f/Aussichtspunkt_Icon.png", size: [20,20]},
			icon_waypoint: {link: "http://wiki-de.guildwars2.com/images/d/df/Wegmarke_Icon.png", size: [24,24]},
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "Events",
			landmark: "Sehenswürdigkeiten",
			players: "Spieler",
			polyline: "Polylinien",
			sector: "Zonen",
			skill: "Fertigkeitspunkte",
			task: "Aufgaben",
			vista: "Aussichtspunkte",
			waypoint: "Wegpunkte",
			attribution: "Kartendaten und -bilder",
			professions:{
				0: {name: "Unbekannt", icon: "http://wiki-de.guildwars2.com/images/c/ce/Allgemein_Icon.png"},
				1: {name: "Wächter", icon: "http://wiki-de.guildwars2.com/images/f/f4/Wächter_Icon.png"},
				2: {name: "Krieger", icon: "http://wiki-de.guildwars2.com/images/9/90/Krieger_Icon.png"},
				3: {name: "Ingenieur", icon: "http://wiki-de.guildwars2.com/images/c/c9/Ingenieur_Icon.png"},
				4: {name: "Waldläufer", icon: "http://wiki-de.guildwars2.com/images/5/5e/Waldläufer_Icon.png"},
				5: {name: "Dieb", icon: "http://wiki-de.guildwars2.com/images/0/07/Dieb_Icon.png"},
				6: {name: "Elementarmagier", icon: "http://wiki-de.guildwars2.com/images/b/b4/Elementarmagier_Icon.png"},
				7: {name: "Mesmer", icon: "http://wiki-de.guildwars2.com/images/0/05/Mesmer_Icon.png"},
				8: {name: "Nekromant", icon: "http://wiki-de.guildwars2.com/images/2/23/Nekromant_Icon.png"}
			}
		},
		en: {
			lang: "en",
			wiki: "http://wiki.guildwars2.com/wiki/",
			icon_event: {link: "http://wiki-de.guildwars2.com/images/7/7a/Event_Angriff_Icon.png", size: [24,24]},
			icon_landmark: {link: "http://wiki.guildwars2.com/images/f/fb/Point_of_interest.png", size: [20,20]},
			icon_skill: {link: "http://wiki.guildwars2.com/images/8/84/Skill_point.png", size: [20,20]},
			icon_task: {link: "http://wiki.guildwars2.com/images/f/f8/Complete_heart_(map_icon).png", size: [20,20]},
			icon_vista: {link: "http://wiki.guildwars2.com/images/d/d9/Vista.png", size: [20,20]},
			icon_waypoint: {link: "http://wiki.guildwars2.com/images/d/d2/Waypoint_(map_icon).png", size: [20,20]},
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "Events",
			landmark: "Points of Interest",
			players: "Players",
			polyline: "Polylines",
			sector: "Sector Names",
			skill: "Skill Challenges",
			task: "Tasks",
			vista: "Vistas",
			waypoint: "Waypoints",
			attribution: "Map data and imagery",
			professions:{
				0: {name: "Unknown", icon: "http://wiki-de.guildwars2.com/images/c/ce/Allgemein_Icon.png"},
				1: {name: "Guardian", icon: "http://wiki-de.guildwars2.com/images/f/f4/Wächter_Icon.png"},
				2: {name: "Warrior", icon: "http://wiki-de.guildwars2.com/images/9/90/Krieger_Icon.png"},
				3: {name: "Engineer", icon: "http://wiki-de.guildwars2.com/images/c/c9/Ingenieur_Icon.png"},
				4: {name: "Ranger", icon: "http://wiki-de.guildwars2.com/images/5/5e/Waldläufer_Icon.png"},
				5: {name: "Thief", icon: "http://wiki-de.guildwars2.com/images/0/07/Dieb_Icon.png"},
				6: {name: "Elementalist", icon: "http://wiki-de.guildwars2.com/images/b/b4/Elementarmagier_Icon.png"},
				7: {name: "Mesmer", icon: "http://wiki-de.guildwars2.com/images/0/05/Mesmer_Icon.png"},
				8: {name: "Necromancer", icon: "http://wiki-de.guildwars2.com/images/2/23/Nekromant_Icon.png"}
			}
		},
		// TODO add es & fr language snippets, es icons
		es: {
			lang:"es",
			wiki: "http://wiki-es.guildwars2.com/wiki/",
			icon_event: {link: "http://wiki-de.guildwars2.com/images/7/7a/Event_Angriff_Icon.png", size: [24,24]},
			icon_landmark: {link: "http://wiki.guildwars2.com/images/f/fb/Point_of_interest.png", size: [20,20]},
			icon_skill: {link: "http://wiki.guildwars2.com/images/8/84/Skill_point.png", size: [20,20]},
			icon_task: {link: "http://wiki.guildwars2.com/images/f/f8/Complete_heart_(map_icon).png", size: [20,20]},
			icon_vista: {link: "http://wiki.guildwars2.com/images/d/d9/Vista.png", size: [20,20]},
			icon_waypoint: {link: "http://wiki.guildwars2.com/images/d/d2/Waypoint_(map_icon).png", size: [20,20]},
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "event-es",
			landmark: "poi-es",
			players: "players-es",
			polyline: "polyline-es",
			sector: "sector-es",
			skill: "skill-es",
			task: "task-es",
			vista: "vista-es",
			waypoint: "waypoint-es",
			attribution: "attribution-es",
			professions:{
				0: {name: "Unknown", icon: "http://wiki-de.guildwars2.com/images/c/ce/Allgemein_Icon.png"},
				1: {name: "Guardian", icon: "http://wiki-de.guildwars2.com/images/f/f4/Wächter_Icon.png"},
				2: {name: "Warrior", icon: "http://wiki-de.guildwars2.com/images/9/90/Krieger_Icon.png"},
				3: {name: "Engineer", icon: "http://wiki-de.guildwars2.com/images/c/c9/Ingenieur_Icon.png"},
				4: {name: "Ranger", icon: "http://wiki-de.guildwars2.com/images/5/5e/Waldläufer_Icon.png"},
				5: {name: "Thief", icon: "http://wiki-de.guildwars2.com/images/0/07/Dieb_Icon.png"},
				6: {name: "Elementalist", icon: "http://wiki-de.guildwars2.com/images/b/b4/Elementarmagier_Icon.png"},
				7: {name: "Mesmer", icon: "http://wiki-de.guildwars2.com/images/0/05/Mesmer_Icon.png"},
				8: {name: "Necromancer", icon: "http://wiki-de.guildwars2.com/images/2/23/Nekromant_Icon.png"}
			}
		},
		fr: {
			lang: "fr",
			wiki: "http://wiki-fr.guildwars2.com/wiki/",
			icon_event: {link: "http://wiki-de.guildwars2.com/images/7/7a/Event_Angriff_Icon.png", size: [24,24]},
			icon_landmark: {link: "http://wiki-fr.guildwars2.com/images/d/d2/Icône_site_remarquable_découvert.png", size: [20,20]},
			icon_skill: {link: "http://wiki-fr.guildwars2.com/images/5/5c/Progression_défi.png", size: [20,20]},
			icon_task: {link: "http://wiki-fr.guildwars2.com/images/a/af/Icône_coeur_plein.png", size: [20,20]},
			icon_vista: {link: "http://wiki-fr.guildwars2.com/images/8/82/Icône_panorama.png", size: [20,20]},
			icon_waypoint: {link: "http://wiki-fr.guildwars2.com/images/5/56/Progression_passage.png", size: [20,20]},
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "event-fr",
			landmark: "Sites remarquables",
			players: "players-fr",
			polyline: "polyline-fr",
			sector: "Secteurs",
			skill: "Défis de compétences",
			task: "Cœurs",
			vista: "Panoramas",
			waypoint: "Points de passage",
			attribution: "attribution-fr",
			professions:{
				0: {name: "Unknown", icon: "http://wiki-de.guildwars2.com/images/c/ce/Allgemein_Icon.png"},
				1: {name: "Guardian", icon: "http://wiki-de.guildwars2.com/images/f/f4/Wächter_Icon.png"},
				2: {name: "Warrior", icon: "http://wiki-de.guildwars2.com/images/9/90/Krieger_Icon.png"},
				3: {name: "Engineer", icon: "http://wiki-de.guildwars2.com/images/c/c9/Ingenieur_Icon.png"},
				4: {name: "Ranger", icon: "http://wiki-de.guildwars2.com/images/5/5e/Waldläufer_Icon.png"},
				5: {name: "Thief", icon: "http://wiki-de.guildwars2.com/images/0/07/Dieb_Icon.png"},
				6: {name: "Elementalist", icon: "http://wiki-de.guildwars2.com/images/b/b4/Elementarmagier_Icon.png"},
				7: {name: "Mesmer", icon: "http://wiki-de.guildwars2.com/images/0/05/Mesmer_Icon.png"},
				8: {name: "Necromancer", icon: "http://wiki-de.guildwars2.com/images/2/23/Nekromant_Icon.png"}
			}
		}
	}
};


/**
 *  excerpts from phpJS
 *  @link http://phpjs.org
 */
var phpjs = {
	intval: function(mixed_var, base){
		var tmp,
			type = typeof(mixed_var);

		if(type === 'boolean'){
			return +mixed_var;
		}
		else if(type === 'string'){
			tmp = parseInt(mixed_var, base || 10);
			return (isNaN(tmp) || !isFinite(tmp)) ? 0 : tmp;
		}
		else if(type === 'number' && isFinite(mixed_var)){
			return mixed_var|0;
		}
		else{
			return 0;
		}
	},

	utf8_encode: function(argString){
		if(argString === null || typeof argString === "undefined"){
			return "";
		}

		var string = (argString+''), // .replace(/\r\n/g, "\n").replace(/\r/g, "\n");
			utftext = "",
			start, end, stringl;

		start = end = 0;
		stringl = string.length;
		for(var n = 0; n < stringl; n++){
			var c1 = string.charCodeAt(n),
				enc = null;

			if(c1 < 128){
				end++;
			}
			else if(c1 > 127 && c1 < 2048){
				enc = String.fromCharCode((c1 >> 6)|192)+String.fromCharCode((c1&63)|128);
			}
			else{
				enc = String.fromCharCode((c1 >> 12)|224)+String.fromCharCode(((c1 >> 6)&63)|128)+String.fromCharCode((c1&63)|128);
			}
			if(enc !== null){
				if(end > start){
					utftext += string.slice(start, end);
				}
				utftext += enc;
				start = end = n+1;
			}
		}

		if(end > start){
			utftext += string.slice(start, stringl);
		}

		return utftext;
	},

	sha1: function(str){
		var rotate_left = function(n, s){
				return (n << s)|(n >>> (32-s));
			},
			cvt_hex = function(val){
				var str = "", v;
				for(var i = 7; i >= 0; i--){
					v = (val >>> (i*4))&0x0f;
					str += v.toString(16);
				}
				return str;
			},
			blockstart,
			i, j,
			W = new Array(80),
			H0 = 0x67452301,
			H1 = 0xEFCDAB89,
			H2 = 0x98BADCFE,
			H3 = 0x10325476,
			H4 = 0xC3D2E1F0,
			A, B, C, D, E,
			temp;

		str = phpjs.utf8_encode(str);
		var str_len = str.length,
			word_array = [];
		for(i = 0; i < str_len-3; i += 4){
			j = str.charCodeAt(i) << 24|str.charCodeAt(i+1) << 16|str.charCodeAt(i+2) << 8|str.charCodeAt(i+3);
			word_array.push(j);
		}

		switch(str_len%4){
			case 0:
				i = 0x080000000;
				break;
			case 1:
				i = str.charCodeAt(str_len-1) << 24|0x0800000;
				break;
			case 2:
				i = str.charCodeAt(str_len-2) << 24|str.charCodeAt(str_len-1) << 16|0x08000;
				break;
			case 3:
				i = str.charCodeAt(str_len-3) << 24|str.charCodeAt(str_len-2) << 16|str.charCodeAt(str_len-1) << 8|0x80;
				break;
		}

		word_array.push(i);

		while((word_array.length%16) != 14){
			word_array.push(0);
		}

		word_array.push(str_len >>> 29);
		word_array.push((str_len << 3)&0x0ffffffff);

		for(blockstart = 0; blockstart < word_array.length; blockstart += 16){
			for(i = 0; i < 16; i++){
				W[i] = word_array[blockstart+i];
			}
			for(i = 16; i <= 79; i++){
				W[i] = rotate_left(W[i-3]^W[i-8]^W[i-14]^W[i-16], 1);
			}


			A = H0;
			B = H1;
			C = H2;
			D = H3;
			E = H4;

			for(i = 0; i <= 19; i++){
				temp = (rotate_left(A, 5)+((B&C)|(~B&D))+E+W[i]+0x5A827999)&0x0ffffffff;
				E = D;
				D = C;
				C = rotate_left(B, 30);
				B = A;
				A = temp;
			}

			for(i = 20; i <= 39; i++){
				temp = (rotate_left(A, 5)+(B^C^D)+E+W[i]+0x6ED9EBA1)&0x0ffffffff;
				E = D;
				D = C;
				C = rotate_left(B, 30);
				B = A;
				A = temp;
			}

			for(i = 40; i <= 59; i++){
				temp = (rotate_left(A, 5)+((B&C)|(B&D)|(C&D))+E+W[i]+0x8F1BBCDC)&0x0ffffffff;
				E = D;
				D = C;
				C = rotate_left(B, 30);
				B = A;
				A = temp;
			}

			for(i = 60; i <= 79; i++){
				temp = (rotate_left(A, 5)+(B^C^D)+E+W[i]+0xCA62C1D6)&0x0ffffffff;
				E = D;
				D = C;
				C = rotate_left(B, 30);
				B = A;
				A = temp;
			}

			H0 = (H0+A)&0x0ffffffff;
			H1 = (H1+B)&0x0ffffffff;
			H2 = (H2+C)&0x0ffffffff;
			H3 = (H3+D)&0x0ffffffff;
			H4 = (H4+E)&0x0ffffffff;
		}

		temp = cvt_hex(H0)+cvt_hex(H1)+cvt_hex(H2)+cvt_hex(H3)+cvt_hex(H4);
		return temp.toLowerCase();
	}

};