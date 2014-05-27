(function() {

    var settings =  {};

    var loadFromStorage = function()
    {
        return JSON.parse(localStorage.getItem('gw2app')) || {lang:"en",home_world_id:2203};
    };

    var saveToStorage = function()
    {
        var stringy = JSON.stringify(settings);
        localStorage.setItem('gw2app', stringy);
        console.log("Estimated localStorage size: " + (stringy.length * 2 / 1024 ) + "kB");
    };

	var getUrlParameters = function(parameter, staticURL, decode){
		var currLocation = (staticURL.length)? staticURL : window.location.search;
		if (currLocation)
		{
			var parArr = currLocation.split("?")[1].split("&");
		   
			for(var i = 0; i < parArr.length; i++){
				parr = parArr[i].split("=");
				if(parr[0] == parameter)
					return (decode) ? decodeURIComponent(parr[1]) : parr[1];
			}
		}
		return false;  
	}
	
    document.observe('dom:loaded', function(){
        [
                "http://cdn.leafletjs.com/leaflet-0.7.2/leaflet.css",	//"../css/leaflet.css",
                "../css/leaflet.label.css",
                "https://d1h9a8s8eodvjz.cloudfront.net/fonts/menomonia/08-02-12/menomonia.css",
                "https://d1h9a8s8eodvjz.cloudfront.net/fonts/menomonia/08-02-12/menomonia-italic.css",
                "../css/gw2maps.css",
                "../css/buttons.css"
        ].each(function(style){// used in the wiki map widget since we cannot access the header there
            $$("head")[0].insert(new Element("link", {"rel": "stylesheet", "type": "text/css", "href": style}));
        });

        $("btn_settings").on("click", function(event) {
            Event.stop(event);
            var elStyle = document.getElementById("settings_connect").style;
            if (elStyle.display == "none")
            {
                Effect.Appear("settings_connect");
                Effect.Appear("settings_misc");
            }
            else
            {
                Effect.Fade("settings_connect");
                Effect.Fade("settings_misc");
            }
        });
        $("ct_host").on("change", function(event) {
			settings.ws_url = $("ct_host").value;
			console.log("CrossTalk url changed: " + settings.ws_url);
			saveToStorage();
        });
		$("ct_port").on("change", function(event) {
			settings.ws_port = parseInt($("ct_port").value);
			console.log("CrossTalk port changed: " + settings.ws_port);
			saveToStorage();
        });
		var lang2Int = {de:1,en:2,es:3,fr:4};
        $("language_list").on("change", function(event) {
            var element = event.element();
            settings.lang = element.getValue();
			$('gw2map').setAttribute('data-language', ""+lang2Int[settings.lang]);
            console.log("Language changed (UI): " + settings.lang);
            saveToStorage();
        });
        $("toggle_players").on("click", function(event) {
            Event.stop(event);
            $("players").setStyle({"max-height": (document.viewport.getHeight()-120)+"px"});
            Effect.toggle("players", "blind");
        });

        var maps = []; //CrossTalk
        var ct_players = {}; //CrossTalk
        var iconStyles = {};
        var ct_me = {};
		var UID_ME = "_§%(%myUID%)%§_";
        settings = loadFromStorage();
		var q_port = getUrlParameters("websocket_port", "", true);
		if (q_port)
		{
			settings.ws_port = parseInt(q_port);
			console.log("Use ws port from url query: " + q_port);
		}
		
        var initLanguageSelection = function(){
            var selectBox = $('language_list');
            $H({en:"english",de:"deutsch",fr:"français",es:"español"}).each(function(e) {	//fran&ccedil;ais	//fran&#231;ais
                selectBox.options.add( new Option(e[1], e[0], (e[0] === settings.lang), (e[0] === settings.lang) ));
            });
        };
        initLanguageSelection();
		$('gw2map').setAttribute('data-language', ""+lang2Int[settings.lang]);
		
        var getPopup = function(m, name, world_id, continent_id, map_id, pos, angle_f, profession, isCommander, c_guild, c_state, c_time, c_vcname)
        {
            var guild_tag = (c_guild.tag) ? " ["+c_guild.tag+"]" : "";
            // popup content (i know, i can get that easier, but this looks cooler XD)
            return new Element("table").insert(
                        new Element("tr").insert(
                            new Element("td", {"style":"width: 44px; text-align: center;"}).update(
                                new Element("img", {"src": m.options.i18n.professions[profession].icon, "alt": m.options.i18n.professions[profession].name, "style": "width:32px; height: 32px;"})
                                )
                            ).insert(
                            new Element("td").update(name + "<br />" + map_id + "<br />" + ((typeof c_vcname === "undefined") ? (c_time) : (c_vcname)))
                            )
                        ).insert(
                        new Element("tr").insert(
                            new Element("td", {"colspan":2, "style":"text-align: center;"}).update(c_guild.name+guild_tag)
                            )
                        )
        };

        var getIcon = function(style)
        {
            var icon = GW2Maps.icons[style];
            if (!icon) icon = GW2Maps.icons.professions[style];
            if (icon)
            {
                if (!icon.iconSize) icon.iconSize = [24,24];
                if (!icon.popupAnchor) icon.popupAnchor = [0, -icon.iconSize[1]/2];
                icon = L.icon(icon);
            }
            return icon;
        }

        /*var getUpdatePlayerMarker = function(m, pos, key, isMe, style)
        {
            if (isMe)
                return L.circleMarker(m.map.unproject(pos, m.map.getMaxZoom()),{radius:24});    //, <Path options> options?

            if (iconStyles[key] !== style)
            {
                var marker;
                var icon = GW2Maps.icons[style];
                if (!icon) icon = GW2Maps.icons.professions[style];
                if (icon)
                {
                    if (!icon.iconSize) icon.iconSize = [24,24];
                    if (!icon.popupAnchor) icon.popupAnchor = [0, -icon.iconSize[1]/2];
                    icon = L.icon(icon);
                    marker = L.marker(m.map.unproject(pos, m.map.getMaxZoom()), {title: key, icon: icon});
                }
                else
                    marker = L.circleMarker(m.map.unproject(pos, m.map.getMaxZoom()));    //, <Path options> options?

                iconStyles[key] = style;
                return marker;
            }
            return false;
        };*/

		// Events API broken by Megaserver
		/*var updateEvents = function(events)
		{
			console.log("Event update");
			for (var i = 0, len = maps.length; i < len; i++) {
				var m = maps[i];
				GW2Maps.parse_events(m,events);
			};
			
		}*/
			
        // is* indicates bool, *_id indicates int; c_* indicates non-API custom stuff;
        function updatePlayer (m, key, name, world_id, continent_id, map_id, pos, angle_f, profession, isCommander, c_guild, c_state, c_time, c_vcname, isMe)
        {
            var icon = getIcon((isCommander) ? "commander" : profession);

            var guild_tag = (c_guild.tag) ? " ["+c_guild.tag+"]" : "",
                                            // popup content (i know, i can get that easier, but this looks cooler XD)
                                            popup = new Element("table").insert(
                                                new Element("tr").insert(
                                                    new Element("td", {"style":"width: 44px; text-align: center;"}).update(
                                                        new Element("img", {"src": m.options.i18n.professions[profession].icon, "alt": m.options.i18n.professions[profession].name, "style": "width:32px; height: 32px;"})
                                                        )
                                                    ).insert(
                                                    new Element("td").update(name + "<br />" + map_id + ", " + world_id + "<br />" + ((typeof c_vcname === "undefined") ? (c_time) : (c_vcname)))
                                                    )
                                                ).insert(
                                                new Element("tr").insert(
                                                    new Element("td", {"colspan":2, "style":"text-align: center;"}).update(c_guild.name+guild_tag)
                                                    )
                                                )


            //console.log("updating player");
            //console.log("Max zoom: " + m.map.getMaxZoom());
            var marker;
            // if theres no marker for the current player, create a new one
            if(typeof m.playermarkers[key] === "undefined")
            {
                //marker = L.circleMarker(m.map.unproject(pos, m.map.getMaxZoom()));    //, <Path options> options?
                marker = L.marker(m.map.unproject(pos, m.map.getMaxZoom()), {title: name, icon: icon});
                marker.bindPopup(popup,{closeButton:false,closeOnClick:true,keepInView:true});
                m.playermarkers[key] = marker;					
                if (isMe)
                    m.layers[m.options.i18n.avatar].addLayer(marker);
                else if (isCommander)
                    m.layers[m.options.i18n[m.options.i18n.commanders]].addLayer(marker);
                else
                    m.layers[m.options.i18n["players"]].addLayer(marker);

                // playerlist content
                var container = new Element("div").insert(
                            new Element("img", {"src": m.options.i18n.professions[profession].icon, "alt":name, "style": "width:18px; height: 18px;"})
                            ).insert(
                            new Element("radio",{"value":key})
                            ).insert(
                            new Element("span", {"title":map_id+", "+world_id}).update(name+guild_tag)
                            );
                // also add a new player-container
                // TODO: sort players
                var oldPlayer = $(key);
                if (oldPlayer)
                    $("players").removeChild(oldPlayer);

                $("players").insert(
                            new Element("div", {"id": key, "data-pos":pos.toString(), "data-continent": continent_id}).update(container).observe("click", function(evt){
                                // NEWER BROWSERS [CROSS-PLATFORM]
                                var shiftPressed=evt.shiftKey;
                                var altPressed  =evt.altKey;
                                var ctrlPressed =evt.ctrlKey;
                                var status=""
                                        +  "shiftKey="+shiftPressed
                                        +", altKey="  +altPressed
                                        +", ctrlKey=" +ctrlPressed;
                                console.log(status);
                                m.map.panTo(m.map.unproject(pos, m.map.getMaxZoom()));
                                //marker.setPopupContent(popup,{closeButton:true,closeOnClick:true,keepInView:true});
                                if (ctrlPressed)
                                {
                                    var popupMarker = m.playermarkers[key];
                                    if (m.layers[m.options.i18n["players"]].hasLayer(popupMarker))  // make him my commander
                                    {
                                        console.log("popupMarker is in players layer");
                                        var oldMyCommanders = m.layers[m.options.i18n["myCommander"]].getLayers();  // should only be one actually, however.
                                        for (var i = 0,len = oldMyCommanders.length; i<len; i++)
                                        {
                                            var oldMyCommander = oldMyCommanders[i];
                                            m.layers[m.options.i18n["myCommander"]].removeLayer(oldMyCommander);
                                            if (ct_players[key].commander)
                                            {
                                                oldMyCommander.setIcon(getIcon("commander"));
                                                m.layers[options.i18n.commanders].addLayer(oldMyCommander)
                                            }
                                            else
                                            {
                                                oldMyCommander.setIcon(getIcon(ct_players[key].profession));
                                                m.layers[m.options.i18n["players"]].addLayer(oldMyCommander);
                                            }
                                        }

                                        m.layers[m.options.i18n["players"]].removeLayer(popupMarker);
                                        popupMarker.setIcon(getIcon("commander_my"));
                                        m.layers[m.options.i18n["myCommander"]].addLayer(popupMarker);
                                    }
                                    else if (m.layers[m.options.i18n["myCommander"]].hasLayer(popupMarker))
                                    {
                                        console.log("popupMarker is in myCommander layer.");
                                        m.layers[m.options.i18n["myCommander"]].removeLayer(popupMarker);

                                        if (ct_players[key].commander)
                                        {
                                            oldMyCommander.setIcon(getIcon("commander"));
                                            m.layers[options.i18n.commanders].addLayer(oldMyCommander)
                                        }
                                        else
                                        {
                                            oldMyCommander.setIcon(getIcon(ct_players[key].profession));
                                            m.layers[m.options.i18n["players"]].addLayer(oldMyCommander);
                                        }
                                    }
                                }
                                else
                                    m.playermarkers[key].openPopup();
                            })
                            );
                marker.on("mouseover", function(event){
                    marker.openPopup();
                });
                marker.on("mouseout", function(event){
                    marker.closePopup();
                });
            }
            // else update the existing one
            else
            {
                marker = m.playermarkers[key];
                marker.setLatLng(m.map.unproject(pos, m.map.getMaxZoom()));
                marker.setPopupContent(popup,{closeButton:false,closeOnClick:true,keepInView:true});
                if (ct_players[key])
                {
                    if(ct_players[key].commander !== isCommander)
                    {
                        if (isCommander)
                        {
                            m.layers["players"].removeLayer(marker);
                            marker.setIcon(getIcon("commander"));
                            m.layers[options.i18n.commanders].addLayer(marker);
                        }
                        else
                        {
                            m.layers[options.i18n.commanders].removeLayer(marker);
                            marker.setIcon(getIcon(profession));
                            m.layers["players"].addLayer(marker);
                        }
                    }
                }
            }
        }

        var onGW2InfoInit = function()
        {
            // CrossTalk
            var initCrossTalk = function()
            {
				var onMessage = function(event)
				{
					console.log('Message received: ' + event.data);
					var jsonObj = JSON.parse(event.data);
					var key = (jsonObj.me)?UID_ME:jsonObj.uid;
					
					if (typeof jsonObj.world_id === "undefined")
					{
						for (var i = 0; i < maps.length; i++) {
							var m = maps[i];
							m.map.removeLayer(m.playermarkers[key])
							delete m.playermarkers[key];
							
							var oldPlayer = $(key);
							if (oldPlayer)
								$("players").removeChild(oldPlayer);
						};
						delete ct_players[key];
						if (jsonObj.me)
							ct_me = {};
						
						return;
					}

					jsonObj.state = jsonObj.me ? 2 : 0;
					jsonObj.time = "";
					jsonObj.guild = {};
					jsonObj.guild.name = "Jianji";
					jsonObj.guild.tag = "JnJ";

					var mapInfo = GW2Info.data.maps[jsonObj.map_id]
					if (!jsonObj.pos) {
						var continent_rect = mapInfo.continent_rect;
						var map_rect = mapInfo.map_rect;
						var x = Math.round(continent_rect[0][0]+(continent_rect[1][0]-continent_rect[0][0])*(jsonObj.px - map_rect[0][0]) / (map_rect[1][0] - map_rect[0][0]));
						var y = Math.round(continent_rect[0][1]+(continent_rect[1][1]-continent_rect[0][1])*(1-(jsonObj.pz - map_rect[0][1])/(map_rect[1][1] - map_rect[0][1])));
						jsonObj.pos = [x,y];
						//console.log("player pos: " + jsonObj.pos[0] + "," + jsonObj.pos[1]);
					};

					// dispatch to maps
					for (var i = 0; i < maps.length; i++) {
						var m = maps[i];
						if ((jsonObj.me) && (mapInfo.continent_id !== m.baselayer.options.continent_id))    //toggle map continent
						{
							console.log("MapInfo continent_id: " + mapInfo.continent_id + " BaseLayer: " + m.baselayer.options.continent_id);
							var oldBaseLayer = m.baselayer;
							m.map.removeLayer(oldBaseLayer);
							m.map.addLayer(m.baselayers[mapInfo.continent_id], true);
						};

						/*if ((jsonObj.me) && (!GW2Info.data.wvw_match) && !(jsonObj.world_id < 1001 || jsonObj.world_id > 2301))
						{
							GW2Info.requestWvWMatches(function(wvw_matches){
								console.log("preloaded wvw_matches");
								if (wvw_matches)
								{
									var myMatch = $H(wvw_matches).values().detect(function(e){ return ((e.red_world_id === jsonObj.world_id) ||
																			  (e.green_world_id === jsonObj.world_id) ||
																			  (e.blue_world_id === jsonObj.world_id))});
									if (myMatch)
									{
											console.log("Found my match-up");
											GW2Info.data.wvw_match = myMatch;
											GW2Info.registerForMatchDetails(function(match_details){
												console.log("Match Details update.");
												GW2Maps.parse_wvwobjectives(m,match_details);
											},false,myMatch.wvw_match_id);
									 }
									else
									{
										console.log("Match-up not found -.-");
									}
								}
							});
						}*/

						//key, name, world_id, continent_id, map_id, pos, angle_f, profession, isCommander, c_guild, c_state, c_time
						updatePlayer(m, key, jsonObj.name, jsonObj.world_id, mapInfo.continent_id, jsonObj.map_id, jsonObj.pos, jsonObj.pa, jsonObj.profession, jsonObj.commander, jsonObj.guild, jsonObj.state, jsonObj.time, jsonObj.vcname, jsonObj.me);
						
						// Events API broken by Megaserver
						/*if (jsonObj.world_id != ct_me.world_id || jsonObj.map_id != ct_me.map_id)
						{
							GW2Info.unregisterForEvents(updateEvents, false, ct_me.world_id, ct_me.map_id);
							GW2Info.registerForEvents(updateEvents, false, jsonObj.world_id, jsonObj.map_id);
						}*/
					}
					ct_players[key] = jsonObj;
					if (jsonObj.me)
						ct_me = jsonObj;
				}
				
				var websocket;
				var doWSConnect = function()
				{
					var baseurl;
					var port;
					
					if (window.MozWebSocket)
					{
						console.log('Info: This browser supports WebSocket using the MozWebSocket constructor');
						window.WebSocket = window.MozWebSocket;
					}
					else if (!window.WebSocket)
					{
						console.log('Error: This browser does not have support for WebSocket');
		                if(typeof(EventSource)!=="undefined") // fallback to sse
						{
							//state = ["lime", "orange", "red", "gray"];// online, away, offline, unknown (css color, rgb or hex notation)
							// we need map data locally for local position recalculation;
							// Doing this in CrossTalk is trouble, Qt is compiled without ssh support so it would need to be a static file.
							baseurl = (settings && settings.ws_url) ? ("http://" + settings.ws_url) : "http://localhost:";
							port = (settings && settings.ws_port) ? settings.ws_port : 64736;
							baseurl = baseurl + port;
							
							var source = new EventSource(baseurl + "/positional_audio/stream");
							source.onerror=function(event)
							{
								if (event.readyState == EventSource.CLOSED) {
									console.log('sse server connection was closed.');
								}
								else
									console.log('Error:' + event.data);
							};
							source.onopen=function(event)
							{
								console.log('sse server connection opened.');
							};
							source.onmessage=function(event)
							{
								onMessage(event);
							};
						}
						else
							console.log("Sorry, your browser does not support server-sent events...");
							
						return;
					}

					baseurl = (settings && settings.ws_url) ? ("ws://" + settings.ws_url) : "ws://localhost";
					port = (settings && settings.ws_port) ? settings.ws_port : 64734;
					baseurl = baseurl + ":" + port;
					// prefer text messages
					var uri = baseurl;
					if (uri.indexOf("?") == -1) {
						uri += "/?encoding=text";
					} else {
						uri += "/&encoding=text";
					}
					console.log('Connecting to ' + baseurl);

					// Websocket Event Handlers
					var ws_onClose = function(evt)
					{
						console.log("Disconnected.");

						// Immediately reconnect!
						doWSConnect();
					};

					websocket = new WebSocket(uri);		
					websocket.onopen = function(evt) { console.log("Connected."); };
					websocket.onclose = function(evt) { ws_onClose(evt) };
					websocket.onmessage = function(evt) { onMessage(evt) };
					websocket.onerror = function(evt) { if (typeof evt.data !== "undefined") console.log('Error: ' + evt.data); };
				};
				
				var doWSDisconnect = function()
				{
					//CONNECTING = 0, OPEN = 1, CLOSED = 2
					if (websocket.readyState == 1)
					{
						console.log("Disconnecting...");
						websocket.close();

						//For some reason onClose is never fired, current bandaid
						ws_onClose();
					}
				};
	
				var doWSSend = function(arg)
				{
					if (arg.id === 'mixer')
					{
						if (activeChannelId == null)
							return;

						arg.id = activeChannelId;		
					}

					arg = JSON.stringify(arg);
					websocket.send(arg);
				}
				
				doWSConnect();
            };

            var initMaps = function()
            {
                $$(".gw2map").each(function(e){
                    var m = GW2Maps.init(e),
                            state = ["lime", "orange", "red", "gray"];// online, away, offline, unknown (css color, rgb or hex notation)

                    m.states = state;	//CrossTalk
                    maps.push(m);
                });
            };

            var preload = {
                continents: false,
                files : false,
                world_names:false,
                maps : false,
                //event_details : false
            };
            var checkPreloadComplete = function()
            {
                var isAllTrue = true;
                $H(preload).each(function(e){
                    if (!e[1]) isAllTrue = false;
                });
                if (isAllTrue)
                {
                    console.log('Preload complete.');
                    initMaps();
                    initCrossTalk();
                };
            };
            GW2Info.requestContinents(function(continents){
                GW2Info.data.continents = continents;
                preload.continents = true;
                console.log("Preloaded continents.");
                GW2Info.data.continents = continents;
                checkPreloadComplete();
            },settings.lang);
            GW2Info.requestFiles(function(files){
                GW2Info.data.files = files;
                preload.files = true;
                console.log("Preloaded Files.");
                checkPreloadComplete();
            });
            GW2Info.requestWorldNames(function(world_names){
                console.log("Request World Names test");
                preload.world_names = true;
                var selectBox = $('world_list');   //document.getElementById('world_list');
                $H(world_names).each(function(e) {
                    selectBox.options.add( new Option(e[1]["name_" + settings.lang], e[1].id, (e[1].id === settings.home_world_id), (e[1].id === settings.home_world_id) ));
                });
                selectBox.observe('change', function(event) {
                    var element = event.element();
                    settings.home_world_id = parseInt(element.getValue());
                    console.log("Home world changed (UI): " + settings.home_world_id);
                    saveToStorage();
                    GW2Info.requestWvWMatches(updateMatchDetailsSubscription);
                });
				checkPreloadComplete();
            },settings.lang);
            GW2Info.requestMaps(function(maps){
                preload.maps = true;
                console.log("Preloaded Maps.");
                GW2Info.data.maps = maps;
                checkPreloadComplete();
            },settings.lang);
			
			// Events API broken by Megaserver
            /*GW2Info.requestEventDetails(function(event_details){
                var obj = {};
                $H(event_details).each(function(e){
                    if (!obj[e[1].map_id])
                        obj[e[1].map_id] = {};

                    obj[e[1].map_id][e[0]] = e[1];
                });
                GW2Info.data.event_details = obj;
                preload.event_details = true;
                console.log("Preloaded Event Details.");
                checkPreloadComplete();
            },settings.lang);*/

            var updateMatchDetailsSubscription = function(wvw_matches)
            {
                var home_world_id = settings.home_world_id;
                if (home_world_id && wvw_matches)
                {
                    var myMatch = $H(wvw_matches).values().detect(function(e){ return ((e.red_world_id === home_world_id) ||
                                                              (e.green_world_id === home_world_id) ||
                                                              (e.blue_world_id === home_world_id))});
                    if (myMatch)
                    {
                        console.log("Found my match-up");

                        var cb_func = function(match_details){
                            //console.log("Match Details update.");
                            // dispatch to maps
                            for (var i = 0, len = maps.length; i < len; i++) {
                                var m = maps[i];
                                GW2Maps.parse_wvwobjectives(m,match_details);
                            };
                        };

                        if (GW2Info.data.wvw_match)
                            GW2Info.unregisterForMatchDetails(cb_func,false,GW2Info.data.wvw_match.wvw_match_id);

                        GW2Info.data.wvw_match = myMatch;
                        GW2Info.registerForMatchDetails(cb_func,false,myMatch.wvw_match_id);
                    }
                    else
                        console.log("Match-up not found -.-");
                }
            }
            GW2Info.requestWvWMatches(updateMatchDetailsSubscription);
        }

        GW2Info.init(onGW2InfoInit,{localFallbackUrl:"../json/GW2Info/"});
    });
})();
