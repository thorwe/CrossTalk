/**
 * gw2Maps.js
 * created: 21.06.13
 *
 * Awesome GW2 maps by Smiley
 *
 * based on Cliff's example
 * http://jsfiddle.net/cliff/CRRGC/
 *
 * and Dr. Ishmaels proof of concept
 * http://wiki.guildwars2.com/wiki/User:Dr_ishmael/leaflet
 *
 * requires:
 *
 * - array_multisort and intval from phpjs (included)
 * - Leaflet polyline decorator (not included, https://github.com/bbecquet/Leaflet.PolylineDecorator)
 *
 *
 * TODO
 *
 * switch floors for maps with multiple floors e.g. like Rata Sum
 */

// Enable XSS... errr... CORS for Prototype: http://kourge.net/node/131
// the console will tell you: Refused to get unsafe header "X-JSON"
Ajax.Responders.register({
	onCreate: function(response) {
//		if (response.request.isSameOrigin())
//			return;
		var t = response.transport;
		t.setRequestHeader = t.setRequestHeader.wrap(function(original, k, v) {
			if (/^(accept|accept-language|content-language)$/i.test(k) || (/^content-type$/i.test(k) && /^(application\/x-www-form-urlencoded|multipart\/form-data|text\/plain)(;.+)?$/i.test(v)))
				return original(k, v);
			return;
		});
	}
});

var isNeedFullMatchDetails = false;

var GW2Maps = {
	/**
	 * @param container
	 * @returns object|bool
	 */
	init: function(container){
		if(typeof container !== "object"){
			return false;
		}
		var options = GW2Maps.options(container),
			mapobject = {
				map: L.map(container, {
					//minZoom: 0,
                    maxZoom: options.continent_id == 1 ? 7 : 6,	//options.max_zoom,
					crs: L.CRS.Simple,
					zoomControl: options.map_controls,
					attributionControl: options.map_controls
				}),
				baselayers: {},
				layers: {},
				playermarkers: {},
                wvw_objective_markers: {},
				linkbox: new Element("div", {"class":"linkbox"}).setStyle({"width": options.linkbox, "height": options.height}),
				options: options//,
				//continent: options.continent_id
			};

		// resize the container to the current viewport size
		//container.setStyle({"height":document.viewport.getHeight()+"px"});

		// first lets prepare our container
		if(options.linkbox){
			// oh, we want a list containing a list of points - no problem! we'll wrap the map container with a table like construct.
			var map_cell = new Element("div", {"class": "table-cell"}).setStyle({"width": "100%"});
			container.setStyle({"width": "100%", "height": options.height}).wrap(map_cell).wrap(new Element("div",{"class": "table-row"}).setStyle({"width": options.width}));
			map_cell.insert({after:mapobject.linkbox.wrap(new Element("div", {"class": "table-cell"}))});
		}
		else{
			// just set the map container to the given size
			container.setStyle({"width": options.width, "height": options.height});
		}

		function getDefaultFloorId() {
			return (options.floor_id != -1) ? options.floor_id : ((options.continent_id == 1) ? 2 :1);
		}
		
		// set the base tiles and add a little copyright info
		var defLayer = L.tileLayer("https://tiles.guildwars2.com/{continent_id}/{floor_id}/{z}/{x}/{y}.jpg", {
			errorTileUrl: options.i18n.errortile,
			//minZoom: 0,
			maxZoom: options.continent_id == 1 ? 7 : 6,	//set on map level actually
			continuousWorld: true,
			continent_id: options.continent_id,
			continent_name: (options.continent_id == 1) ? "Tyria" : "The Mists",
			floor_id: getDefaultFloorId(),
			attribution: options.i18n.attribution+': <a href="https://forum-en.guildwars2.com/forum/community/api/API-Documentation" target="_blank">GW2 Maps API</a>, &copy;<a href="http://www.arena.net/" target="_blank">ArenaNet</a>'
		}).addTo(mapobject.map);
        mapobject.baselayers[defLayer.options.continent_id] = defLayer;
		mapobject.baselayer = defLayer;
		
		// add layergroups and show them on the map
		mapobject.layers[options.i18n.event] = L.layerGroup();
		mapobject.layers[options.i18n.landmark] = L.layerGroup();
		mapobject.layers[options.i18n.polyline] = L.layerGroup();
		mapobject.layers[options.i18n.players] = L.layerGroup();
		mapobject.layers[options.i18n.players].addTo(mapobject.map);
		mapobject.layers[options.i18n.skill] = L.layerGroup();
		mapobject.layers[options.i18n.task] = L.layerGroup();
		mapobject.layers[options.i18n.vista] = L.layerGroup();
		mapobject.layers[options.i18n.waypoint] = L.layerGroup();
		mapobject.layers[options.i18n.waypoint].addTo(mapobject.map);
        mapobject.layers["WvW-Timers"] = L.layerGroup();   // I don't see this requiring translation anytime soon?
        mapobject.layers["WvW-Timers"].addTo(mapobject.map);
        mapobject.layers["WvW"] = L.layerGroup();   // I don't see this requiring translation anytime soon?
        mapobject.layers["WvW"].addTo(mapobject.map);
		mapobject.layers[options.i18n.sector] = L.layerGroup();
		mapobject.layers[options.i18n.commanders] = L.layerGroup();
        mapobject.layers[options.i18n.commanders].addTo(mapobject.map);
        mapobject.layers[options.i18n.avatar] = L.layerGroup();
        mapobject.layers[options.i18n.avatar].addTo(mapobject.map);
        mapobject.layers[options.i18n.myCommander] = L.layerGroup();
        mapobject.layers[options.i18n.myCommander].addTo(mapobject.map);


		// showing all the stuff on the initial map would be confusing in most cases,
		// so we'll show it automatically only on higher zoom levels - it's in the layer menu anyway
		if(mapobject.map.getZoom() > 3){
			mapobject.layers[options.i18n.landmark].addTo(mapobject.map);
			mapobject.layers[options.i18n.skill].addTo(mapobject.map);
			mapobject.layers[options.i18n.task].addTo(mapobject.map);
			mapobject.layers[options.i18n.vista].addTo(mapobject.map);
		}
		if(mapobject.map.getZoom() > 4){
			mapobject.layers[options.i18n.polyline].addTo(mapobject.map);
		}
		if(options.region_id && options.map_id || mapobject.map.getZoom() > 5){
			mapobject.layers[options.i18n.sector].addTo(mapobject.map);
			mapobject.layers[options.i18n.event].addTo(mapobject.map);
		}

		// a Layer control if wanted
		if(options.map_controls){
			if (options.map_controls_continent)
			{
				var nondefLayer = L.tileLayer("https://tiles.guildwars2.com/{continent_id}/{floor_id}/{z}/{x}/{y}.jpg", {
					errorTileUrl: options.i18n.errortile,
					//minZoom: 0,
					maxZoom: options.continent_id == 2 ? 7 : 6,
					continuousWorld: true,
					continent_id: options.continent_id == 1 ? 2 : 1,
					continent_name: options.continent_id == 1 ? "The Mists" : "Tyria",
					floor_id: (options.continent_id == 1) ? 1 : 2,
					attribution: defLayer.attribution
				});
                mapobject.baselayers[nondefLayer.options.continent_id] = nondefLayer;

                var staticLayers = {};
                $A(["event","landmark","skill","task","vista","waypoint","sector"]).each(function(a){
                    console.log(options.i18n[a]);
                    staticLayers[options.i18n[a]] = mapobject.layers[options.i18n[a]];
                });
                var playerLayers = {};
                $A(["avatar","players","commanders","myCommander"]).each(function(a){
                    console.log(options.i18n[a]);
                    playerLayers[options.i18n[a]] = mapobject.layers[options.i18n[a]];
                });
                var otherLayers = {};
                otherLayers[options.i18n.polyline] = mapobject.layers[options.i18n.polyline];
                otherLayers["WvW"] = mapobject.layers["WvW"];

                //var groupedOverlays = {};
                //groupedOverlays["Landmarks"] = staticLayers;
                //groupedOverlays["Players"] = playerLayers;
                //groupedOverlays["Other"] = otherLayers;

                L.control.layers(mapobject.baselayers, mapobject.layers).addTo(mapobject.map);
                //L.control.groupedLayers(mapobject.baselayers, groupedOverlays).addTo(mapobject.map);
			}
			else
				L.control.layers(null, mapobject.layers).addTo(mapobject.map);
		}

		// we have polylines to display?
		if(options.polyline && options.polyline.length > 7){
			GW2Maps.parse_polylines(mapobject, options);
		}

		// magically display/remove icons
		mapobject.map.on("zoomend", function(){
			var z = mapobject.map.getZoom();
			z > 5 ? mapobject.layers[options.i18n.event].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.event]);
			z > 5 ? mapobject.layers[options.i18n.sector].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.sector]);
			z > 4 ? mapobject.layers[options.i18n.polyline].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.polyline]);
			z > 3 ? mapobject.layers[options.i18n.landmark].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.landmark]);
			z > 3 ? mapobject.layers[options.i18n.skill].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.skill]);
			z > 3 ? mapobject.layers[options.i18n.task].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.task]);
			z > 3 ? mapobject.layers[options.i18n.vista].addTo(mapobject.map) : mapobject.map.removeLayer(mapobject.layers[options.i18n.vista]);
		});

		// you may specify more mapevent handlers over here - for example a click handler to annoy people ;)
        function requestMapUpdate()
		{
			console.log("Request Map Update");
            $H(mapobject.layers).each(function(pair) {pair.value.clearLayers();});

            GW2Info.requestMapFloor(function(responseJSON){
                if (responseJSON)
                    GW2Maps.parse_response(mapobject, options, responseJSON);
                else
                {
                    // if we don't get any floordata, we try at least to render the map
                    options.region_id = false;
                    GW2Maps.parse_response(mapobject, options, {texture_dims: mapobject.baselayer.options.continent_id === 1 ? [32768,32768] : [16384,16384], regions:[]}, {});
                    //mapobject.linkbox.append();
                }
            }, options.i18n.lang, mapobject.baselayer.options.continent_id, mapobject.baselayer.options.floor_id);
		}

        requestMapUpdate();
        var isInit = true;
		mapobject.map.on('baselayerchange',function(e){
            if (isInit) //suppress the first baselayerchange
            {
                isInit = false;
                return;
            }

			mapobject.baselayer = e.layer;
            mapobject.playermarkers = {};
            mapobject.wvw_objective_markers = {};
            mapobject.map.options.maxZoom = mapobject.baselayer.options.maxZoom;
            console.log("New Base Layer: " + mapobject.baselayer.options.continent_id + " MaxZoom: " + mapobject.baselayer.options.maxZoom);
            if (mapobject.baselayer.options.continent_id === 2)
                isNeedFullMatchDetails = true;

            requestMapUpdate();
		});
		mapobject.map.on("click", function(event){
//			L.popup().setLatLng(event.latlng).setContent(mapobject.map.project(event.latlng, mapobject.map.getMaxZoom()).toString()).openOn(mapobject.map);
            console.log(mapobject.map.project(event.latlng, mapobject.map.getMaxZoom()).toString());
		});
		

		// return the mapobject for later use
		return mapobject;
	},
	
	/**
	 * @param mapobject
	 * @param options
	 * @param floordata
	 */
	//parse_response: function(mapobject, options, floordata, eventdata){
	parse_response: function(mapobject, options, floordata){
		var bounds, clamp,
			p2ll = function(coords){
                return mapobject.map.unproject(coords, mapobject.map.getMaxZoom());
			};
		// the map has a clamped view? ok, we use this as bound
		if(floordata.clamped_view){
			clamp = floordata.clamped_view;
			bounds = new L.LatLngBounds(p2ll([clamp[0][0], clamp[1][1]]), p2ll([clamp[1][0], clamp[0][1]])).pad(0.2);
		}
		// we display a specific map? so lets use the maps bounds
		else if(options.region_id && options.map_id){
			clamp = floordata.regions[options.region_id].maps[options.map_id].continent_rect;
			bounds = new L.LatLngBounds(p2ll([clamp[0][0], clamp[1][1]]), p2ll([clamp[1][0], clamp[0][1]])).pad(0.4);
		}
		// else use the texture_dims as bounds
		else{
//			bounds = new L.LatLngBounds(p2ll([0, (1 << mapobject.map.getMaxZoom())*256]), p2ll([(1 << mapobject.map.getMaxZoom())*256, 0]));
			bounds = new L.LatLngBounds(p2ll([0, floordata.texture_dims[1]]), p2ll([floordata.texture_dims[0], 0])).pad(0.1);
		}
		mapobject.map.setMaxBounds(bounds).fitBounds(bounds);

		// ok, we want to display a single map
		if(options.region_id && options.map_id){
			GW2Maps.parse_map(mapobject, options, floordata.regions[options.region_id].maps[options.map_id]);
		}
		// else render anything we get
		else{
			$H(floordata.regions).each(function(region){
				$H(region[1].maps).each(function(map){
                    map[1].id = map[0];
					GW2Maps.parse_map(mapobject, options, map[1]);
				});
			});
		}
	},

	/**
	 * the no more so ugly map parser of uglyness (Anet, please get your data straight!)
	 *
	 * @param mapobject
	 * @param options
	 * @param map
	 */
	parse_map: function(mapobject, options, map){
        var name = "name_" + options.i18n.lang,
            eventdata = (typeof GW2Info.data.event_details[map.id] !== "undefined") ? GW2Info.data.event_details[map.id] : false,
			pois = {task: [], event: [], landmark: [], skill: [], vista: [], waypoint: [], sector: []},
			sort = {task: [], event: [], landmark: [], skill: [], vista: [], waypoint: [], sector: []},
			recalc_event_coords = function(cr, mr, p){
				// don't look at it. really! it will melt your brain and make your eyes bleed!
				return [Math.round(cr[0][0]+(cr[1][0]-cr[0][0])*(p[0]-mr[0][0])/(mr[1][0]-mr[0][0])),Math.round(cr[0][1]+(cr[1][1]-cr[0][1])*(1-(p[1]-mr [0][1])/(mr[1][1]-mr[0][1])))]
			};
		
		// pois
		map.points_of_interest.each(function(p){
			if(p.type == "waypoint"){
                sort.waypoint.push(p[name]);
				pois.waypoint.push({
					id: p.poi_id,
					type: p.type,
					coords: p.coord,
                    title: p[name],
                    text: p[name],
                    popup: p[name]+"<br />id: "+p.poi_id+"<br />"+p.coord.toString()
				});
			}
			if(p.type == "landmark"){
                sort.landmark.push(p[name]);
				pois.landmark.push({
					id: p.poi_id,
					type: p.type,
					coords: p.coord,
                    title: p[name],
                    text: p[name],
                    popup: '<a href="'+options.i18n.wiki+encodeURIComponent(p[name])+'" target="_blank">'+p[name]+"</a><br />id: "+p.poi_id
				});
			}
			if(p.type == "vista"){
				sort.vista.push(p.poi_id);
				pois.vista.push({
					type: p.type,
					coords:p.coord,
					title: "id:"+p.poi_id,
                    text: p[name]+' '+p.poi_id,
                    popup: "id: "+p.poi_id
				});
			}
		});
		// tasks (hearts)
		map.tasks.each(function(p){
			sort.task.push(p.level);
			pois.task.push({
				id: p.task_id,
				type: "task",
				coords: p.coord,
                title: p[name]+" ("+p.level+")",
                text: "("+p.level+") " + p[name],
                popup: '<a href="'+options.i18n.wiki+encodeURIComponent(p[name].replace(/\.$/, ""))+'" target="_blank">'+p[name]+"</a> ("+p.level+")<br />id:"+p.task_id
			});
		});
		// skill challenges
		map.skill_challenges.each(function(p){
			sort.skill.push(p.coord.toString());
			pois.skill.push({
				id: null,
				type: "skill",
				coords: p.coord,
				title: p.coord.toString(),
                text: p[name]+' '+p.coord.toString(),
                popup: p[name]+' '+p.coord.toString()
			});
		});
		// sector names
		map.sectors.each(function(p){
            sort.sector.push(p[name]);
			pois.sector.push({
				id: p.sector_id,
				type: "sector",
				coords:p.coord,
                title: p[name]+", id:"+p.sector_id,
                icon_text: p[name],
				icon_text_class: "sector_text",
                text: p[name],
				popup: false
			});
		});
		// eventdata
		if(eventdata){
			console.log("Parsing Events");
			$H(eventdata).each(function(p){
				console.log("Name: " + p[1][name]);
				sort.event.push(p[1].level);
				pois.event.push({
					id: p[0],
					type: "event",
					coords: recalc_event_coords(map.continent_rect,map.map_rect,p[1].location.center),
					title: p[1][name]+" ("+p[1].level+")",
					text: "("+p[1].level+") "+p[1][name],
					popup: '<a href="'+options.i18n.wiki+encodeURIComponent(p[1][name].replace(/\.$/, ""))+'" target="_blank">'+p[1][name]+"</a> ("+p[1].level+")<br />id:"+p[0]
				});
			});
        };

		// loop out the map points
        mapobject.linkbox.insert(new Element("div", {"class":"header"}).update(map[name]));
		$H(pois).each(function(points){
			if(points[1].length > 0){
				// phpJS... <3
				phpjs.array_multisort(sort[points[0]], "SORT_ASC", points[1]);
				mapobject.linkbox.insert(new Element("div", {"class":"header sub"}).update(options.i18n[points[0]]));
				points[1].each(function(p){
					GW2Maps.parse_point(mapobject, options, p);
				});
			}
		});
	},

    parse_wvwobjectives: function(mapobject,match_details)
    {
        //console.log(JSON.stringify(match_details.maps));
        if (!(GW2Info &&GW2Info.data && GW2Info.data.wvw_mapTypeId && GW2Info.data.wvw_objectives && mapobject.baselayer.options.continent_id === 2))
        {
            console.log("Returning early. mapobject continent:" + mapobject.baselayer.options.continent_id);
            return;
        }
        //if (!mapobject.wvw_objective_markers) mapobject.wvw_objective_markers = {};
        var options = mapobject.options;

        if (!isNeedFullMatchDetails)
            match_details = match_details._diff;

        $H(match_details.maps).each(function(e){
            //console.log("Parsing WvW Map: " + e[0]);
            //var map_id = GW2Info.data.wvw_mapTypeId[e[1].type];
            $H(e[1].objectives).each(function(f){   //objectives are unique
                //console.log("Parsing WvW Objective: " + f[0] + " " + typeof f[1].id);
                var objData = GW2Info.data.wvw_objectives[e[0]][f[0]];
                var objType = objData.type;
                var col = f[1].owner.toLowerCase();
                if (col === "neutral") col = "white"; //ffs

                var icon;
                if (objType === "ruin")
                {
                    if (GW2Info.data.files)
                    {
                        var basekey = objData.fileApiBase;
                        if (basekey)
                        {
                            //console.log("RUIN: " + basekey + col);

                            var fileObj = GW2Info.data.files[basekey + col];
                            var url = "https://render.guildwars2.com/file/" + fileObj.signature + "/" + fileObj.file_id + ".png";
                            //console.log(url);
                            icon = {iconUrl: url, iconSize: [24,24]};  //128*128
                        }
                        else
                            console.log("Could not get basekey");
                    }
                    else
                        console.log("Need access to GW2Info.data.files");
                }
                else
                {
                    //console.log(objType+'_'+col);
                    icon = GW2Maps.icons[objType+'_'+col];
                    if (!isNeedFullMatchDetails)
                    {
                        //console.log("Starting timer");
                        // create timer obj
                        var timer = new PeriodicalExecuter(function(pe) {
                            var remaining = this._end_time - (new Date());
                            if (remaining < 0) {
                                pe.stop();
                                this._marker.unbindLabel();
                            }
                            else
                            {
                                remaining = remaining / 1000; // seconds
                                var minutes = Math.floor(remaining / 60);
                                var seconds = Math.floor(remaining % 60);
                                this._marker.updateLabelContent(minutes + ":" + ((seconds < 10)?'0':'') + seconds);
                                //console.log(this._wvw_obj_id + " remaining: " + minutes + ":" + ((seconds < 10)?'0':'') + seconds);
                            }
                        }, 1);
                        var endTime = new Date();
                        endTime.setTime(endTime.getTime() + 300000); // 1000*60*5
                        timer._wvw_obj_id = f[0];
                        timer._marker = mapobject.wvw_objective_markers[f[0]];
                        timer._end_time = endTime;
                        if (timer._marker)
                        {
                            timer._marker.bindLabel('05:00', { noHide: true });
                            timer._marker.showLabel();
                            //console.log("Bound label");
                        }
                    }
                };

                if(icon)
                {
                    //console.log("Created icon data.");
                    if (!icon.popupAnchor) icon.popupAnchor = [0, -icon.iconSize[1]/2];
                    icon = L.icon(icon);

                    var obj_name = objData.name[options.i18n.lang]; //TODO: Unify this data sometime
                    //console.log(obj_name);

                    var popup = '<a href="'+options.i18n.wiki+encodeURIComponent(obj_name.replace(/\.$/, ""))+'" target="_blank">'+obj_name+"</a> <br />id:"+f[0];
                    //"owner": "Blue","owner_guild": "4427D889-82B6-4BC3-9763-7B1129BCB0E7"
                    if (f[1].owner_guild)
                        popup = popup + '<br />' + f[1].owner_guild;

                    //console.log(popup);

                    var marker = mapobject.wvw_objective_markers[f[0]];
                    //console.log("Have marker: " + (marker ? "true":"false"));
                    if(!marker)
                    {

                        marker = L.marker(mapobject.map.unproject(objData.coords, mapobject.map.getMaxZoom()), {title: obj_name, icon: icon});
                        marker.bindPopup(popup);
                        mapobject.layers["WvW"].addLayer(marker);
                        mapobject.wvw_objective_markers[f[0]] = marker;
                    }
                    else
                    {
                        marker.setPopupContent(popup,{closeButton:false,closeOnClick:true,keepInView:true});
                        marker.setIcon(icon);
                    };
                }
                else
                {
                    console.log("Could not create icon.");
                }
            });
        });

        if (isNeedFullMatchDetails)
            isNeedFullMatchDetails = false;
    },

	/**
	 * @param mapobject
	 * @param options
	 * @param point
	 */
	parse_point: function(mapobject, options, point){
        //var i = options.i18n["icon_"+point.type],
        var i = GW2Maps.icons[point.type],
			icon,
			pan = function(p,text){
                var ll = mapobject.map.unproject(p, mapobject.map.getMaxZoom());
				mapobject.map.panTo(ll);
				if(text){
					L.popup({offset:new L.Point(0,-5)}).setLatLng(ll).setContent(text).openOn(mapobject.map);
				}
			};

		if(point.type === "sector"){
			icon = L.divIcon({className: point.icon_text_class, html: point.icon_text});
        }
		else{
            //icon = L.icon({iconUrl: i.link, iconSize: i.size, popupAnchor:[0, -i.size[1]/2]});
            //console.log("type:"+point.type);
            icon = L.icon({iconUrl: i.iconUrl, iconSize: i.iconSize, popupAnchor:[0, -i.iconSize[1]/2]});
            //console.log("i.iconUrl:"+i.iconUrl + " i.iconSize: " + i.iconSize[0] + "," + i.iconSize[1]);
		}

        var marker = L.marker(mapobject.map.unproject(point.coords, mapobject.map.getMaxZoom()), {title: point.title, icon: icon});
		if(point.popup){
			marker.bindPopup(point.popup);
		}
		mapobject.layers[options.i18n[point.type]].addLayer(marker);
        mapobject.linkbox.insert(new Element("div").insert(i ? new Element("img", {"src":i.iconUrl}).setStyle({"width":"16px", "height":"16px"}) : '').insert(' '+point.text).observe("click", function(){
			pan(point.coords,point.popup);
		}));

		// we have also a poi in the options? lets find and display it...
		if(options.poi_id && point.id === options.poi_id && options.poi_type && point.type === options.poi_type){
			pan(point.coords,point.popup);
            mapobject.map.setZoom(mapobject.map.getMaxZoom());
		}
	},

	/**
	 * @param mapobject
	 * @param options
	 */
	parse_polylines: function(mapobject, options){
		var lines = options.polyline.split(";");
		lines.each(function(l){
			var coords = l.split(" "), line = [], opts = {};
			coords.each(function(c){
				if(c.match(/\d{1,5},\d{1,5}/)){
					var point = c.split(",");
                    line.push(mapobject.map.unproject(point, mapobject.map.getMaxZoom()));
				}
				if(c.match(/(color|width|opacity|style|type)=(([0-9a-f]{3}){1,2}|\d{1,3}|(arrow|marker|dash))/i)){
					var opt = c.toLowerCase().split("=");
					opts[opt[0]] = opt[1];
				}
			});
			var color = typeof opts.color !== "undefined" ? "#"+opts.color : "#ffe500",
				width = typeof opts.width !== "undefined" ? phpjs.intval(opts.width) : 3,
				opacity = typeof opts.opacity !== "undefined" ? phpjs.intval(opts.opacity)/100 : 0.8,
				dash = opts.style === "dash"  ? "30,15,10,15" : "";

			line = L.polyline(line, {color: color, weight: width, opacity: opacity, dashArray: dash});
			mapobject.layers[options.i18n.polyline].addLayer(line);

			if(typeof opts.type !== "undefined"){
				var patterns = [];
				if(opts.type === "arrow"){
					patterns.push({offset: 50, repeat: "150px", symbol: new L.Symbol.ArrowHead({pixelSize: 15, polygon: false, pathOptions: {stroke: true, color: color, weight: width, opacity: opacity}})});
				}
				if(opts.type === "marker"){
					patterns.push({offset: 0, repeat: "100%", symbol: new L.Symbol.Marker()});
				}
				mapobject.layers[options.i18n.polyline].addLayer(L.polylineDecorator(line, {patterns: patterns}));
			}
		});
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
			//max_zoom: continent_id == 1 ? 7 : 6,
			continent_id: continent_id,
			floor_id: typeof dataset.floor_id === "number" ? dataset.floor_id : -1,
			region_id: typeof dataset.region_id === "number" && dataset.region_id > 0 ? dataset.region_id : false,
			map_id: typeof dataset.map_id === "number" && dataset.map_id > 0 ? dataset.map_id : false,
			poi_id: typeof dataset.poi_id === "number" && dataset.poi_id > 0 ? dataset.poi_id : false,
			poi_type: typeof dataset.poi_type === "number" && dataset.poi_type > 0 && dataset.poi_type <= 6 ? poi_types[dataset.poi_type] : false,
			width: typeof dataset.width === "number" && dataset.width > 0 ? dataset.width+(dataset.w_percent == true ? "%" : "px") : "800px",
			height: typeof dataset.height === "number" && dataset.height > 0 ? dataset.height+(dataset.h_percent == true ? "%" : "px") : "450px",
			map_controls: dataset.disable_controls != true,
			map_controls_continent: dataset.enable_control_continent == true,
			linkbox: typeof dataset.linkbox === "number" && dataset.linkbox >= 100 ? dataset.linkbox+"px" : false,
			polyline: dataset.polyline && dataset.polyline.length > 7 ? dataset.polyline : false,
			i18n: typeof dataset.language === "number" && dataset.language >=1 && dataset.language <= 4 ? GW2Maps.i18n[lang[dataset.language]] : GW2Maps.i18n[lang[0]]
		};
	},

    icons: {
        // wvw objectives
        camp_red:       {iconUrl: "../img/wvw/camp_r.png", iconSize: [24, 24]}, //[32, 32]
        camp_green:     {iconUrl: "../img/wvw/camp_g.png", iconSize: [24, 24]},
        camp_blue:      {iconUrl: "../img/wvw/camp_b.png", iconSize: [24, 24]},
        tower_red:      {iconUrl: "../img/wvw/tower_r.png", iconSize: [24, 24]},
        tower_green:    {iconUrl: "../img/wvw/tower_g.png", iconSize: [24, 24]},
        tower_blue:     {iconUrl: "../img/wvw/tower_b.png", iconSize: [24, 24]},
        keep_red:       {iconUrl: "../img/wvw/keep_r.png", iconSize: [24, 24]},
        keep_green:     {iconUrl: "../img/wvw/keep_g.png", iconSize: [24, 24]},
        keep_blue:      {iconUrl: "../img/wvw/keep_b.png", iconSize: [24, 24]},
        castle_red:     {iconUrl: "../img/wvw/castle_r.png", iconSize: [24, 24]},
        castle_green:   {iconUrl: "../img/wvw/castle_g.png", iconSize: [24, 24]},
        castle_blue:    {iconUrl: "../img/wvw/castle_b.png", iconSize: [24, 24]},

        // player icons
        commander:      {iconUrl: "../img/Commander_Icon.png", iconSize: [20,21]},//[181,190]},
        commander_guild:{iconUrl: "../img/Commander_Icon_orange.png", iconSize: [30,31]},//[181,190]},
        commander_my:   {iconUrl: "../img/Commander_Icon_green.png", iconSize: [30,31]},//[181,190]},

        professions:{
            0: {iconUrl: "../img/professions/generic_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/c/ce/Allgemein_Icon.png"},   // grey circle
            1: {iconUrl: "../img/professions/guardian_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/f/f4/Wächter_Icon.png"},
            2: {iconUrl: "../img/professions/warrior_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/9/90/Krieger_Icon.png"},
            3: {iconUrl: "../img/professions/engineer_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/c/c9/Ingenieur_Icon.png"},
            4: {iconUrl: "../img/professions/ranger_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/5/5e/Waldläufer_Icon.png"},
            5: {iconUrl: "../img/professions/thief_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/0/07/Dieb_Icon.png"},
            6: {iconUrl: "../img/professions/elementalist_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/b/b4/Elementarmagier_Icon.png"},
            7: {iconUrl: "../img/professions/mesmer_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/0/05/Mesmer_Icon.png"},
            8: {iconUrl: "../img/professions/necromancer_icon.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/2/23/Nekromant_Icon.png"}
        },

        event: {iconUrl: "../img/event_attack.png", fallbackUrl: "http://wiki-de.guildwars2.com/images/7/7a/Event_Angriff_Icon.png", iconSize: [24,24]},
        landmark: {iconUrl: "../img/poi.png", fallbackUrl: "http://wiki.guildwars2.com/images/f/fb/Point_of_interest.png", iconSize: [20,20]},
        skill: {iconUrl: "../img/skill_point.png", fallbackUrl: "http://wiki.guildwars2.com/images/8/84/Skill_point.png", iconSize: [20,20]},
        task: {iconUrl: "../img/heart.png", fallbackUrl: "http://wiki.guildwars2.com/images/f/f8/Complete_heart_(map_icon).png", iconSize: [20,20]},
        vista: {iconUrl: "../img/vista.png", fallbackUrl: "http://wiki.guildwars2.com/images/d/d9/Vista.png", iconSize: [20,20]},
        waypoint: {iconUrl: "../img/waypoint.png", fallbackUrl: "http://wiki.guildwars2.com/images/d/d2/Waypoint_(map_icon).png", iconSize: [20,20]},
    },

	/**
	 *
	 */
	i18n: {
		de: {
			lang: "de",
			wiki: "http://wiki-de.guildwars2.com/wiki/",
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "Events",
			landmark: "Sehenswürdigkeiten",
			players: "Spieler",
			commanders: "Kommandeure",
			polyline: "Polylinien",
			sector: "Zonen",
			skill: "Fertigkeitspunkte",
			task: "Aufgaben",
			vista: "Aussichtspunkte",
			waypoint: "Wegpunkte",
			attribution: "Kartendaten und -bilder",
            avatar: "Avatar",
            myCommander: "Fokus",
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
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "Events",
			landmark: "Points of Interest",
			players: "Players",
			commanders: "Commanders",
			polyline: "Polylines",
			sector: "Sector Names",
			skill: "Skill Challenges",
			task: "Tasks",
			vista: "Vistas",
			waypoint: "Waypoints",
			attribution: "Map data and imagery",
            avatar: "Avatar",
            myCommander: "Focus",
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
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "event-es",
			landmark: "poi-es",
			players: "players-es",
			commanders: "Commanders",
			polyline: "polyline-es",
			sector: "sector-es",
			skill: "skill-es",
			task: "task-es",
			vista: "vista-es",
			waypoint: "waypoint-es",
			attribution: "attribution-es",
            avatar: "Avatar",
            myCommander: "Focus",
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
			errortile: "http://wiki-de.guildwars2.com/images/6/6f/Kartenhintergrund.png",
			event: "event-fr",
			landmark: "Sites remarquables",
			players: "players-fr",
			commanders: "Commanders",
			polyline: "polyline-fr",
			sector: "Secteurs",
			skill: "Défis de compétences",
			task: "Cœurs",
			vista: "Panoramas",
			waypoint: "Points de passage",
			attribution: "attribution-fr",
            avatar: "Avatar",
            myCommander: "Focus",
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
 *  (minified) excerpts from phpJS
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
	array_multisort: function(arr){
		var flags = {
				'SORT_REGULAR': 16,
				'SORT_NUMERIC': 17,
				'SORT_STRING': 18,
				'SORT_ASC': 32,
				'SORT_DESC': 40
			},
		//argl = arguments.length,
		//args = arguments,
			sortArrsLength = 0,
			sortArrs = [[]],
			sortKeys = [[]],
			sortFlag = [0],
			g = 0,
			i = 0,
			j,// = 0
			k = '',
			l = 0,
			thingsToSort = [],
			vkey = 0,
			zlast = null,
			nLastSort = [],
			lastSort = [],
			lastSorts = [],
			tmpArray = [],
			elIndex = 0,
			sortDuplicator = function(){//a, b
				return nLastSort.shift();
			},
			sortFunctions = [
				[
					function(a, b){
						lastSort.push(a > b ? 1 : (a < b ? -1 : 0));
						return a > b ? 1 : (a < b ? -1 : 0);
					},
					function(a, b){
						lastSort.push(b > a ? 1 : (b < a ? -1 : 0));
						return b > a ? 1 : (b < a ? -1 : 0);
					}
				],
				[
					function(a, b){
						lastSort.push(a-b);
						return a-b;
					},
					function(a, b){
						lastSort.push(b-a);
						return b-a;
					}
				],
				[
					function(a, b){
						lastSort.push((a+'') > (b+'') ? 1 : ((a+'') < (b+'') ? -1 : 0));
						return (a+'') > (b+'') ? 1 : ((a+'') < (b+'') ? -1 : 0);
					},
					function(a, b){
						lastSort.push((b+'') > (a+'') ? 1 : ((b+'') < (a+'') ? -1 : 0));
						return (b+'') > (a+'') ? 1 : ((b+'') < (a+'') ? -1 : 0);
					}
				]
			];

		if(Object.prototype.toString.call(arr) === '[object Array]'){
			sortArrs[0] = arr;
		}
		else if(arr && typeof arr === 'object'){
			for(i in arr){
				if(arr.hasOwnProperty(i)){
					sortKeys[0].push(i);
					sortArrs[0].push(arr[i]);
				}
			}
		}
		else{
			return false;
		}

		var arrMainLength = sortArrs[0].length, sortComponents = [0, arrMainLength];

		for(j = 1; j < arguments.length; j++){
			if(Object.prototype.toString.call(arguments[j]) === '[object Array]'){
				sortArrs[j] = arguments[j];
				sortFlag[j] = 0;
				if(arguments[j].length !== arrMainLength){
					return false;
				}
			}
			else if(arguments[j] && typeof arguments[j] === 'object'){
				sortKeys[j] = [];
				sortArrs[j] = [];
				sortFlag[j] = 0;
				for(i in arguments[j]){
					if(arguments[j].hasOwnProperty(i)){
						sortKeys[j].push(i);
						sortArrs[j].push(arguments[j][i]);
					}
				}
				if(sortArrs[j].length !== arrMainLength){
					return false;
				}
			}
			else if(typeof arguments[j] === 'string'){
				var lFlag = sortFlag.pop();
				if(typeof flags[arguments[j]] === 'undefined' || ((((flags[arguments[j]]) >>> 4)&(lFlag >>> 4)) > 0)){
					return false;
				}
				sortFlag.push(lFlag+flags[arguments[j]]);
			}
			else{
				return false;
			}
		}

		for(i = 0; i !== arrMainLength; i++){
			thingsToSort.push(true);
		}

		for(i in sortArrs){
			if(sortArrs.hasOwnProperty(i)){
				lastSorts = [];
				tmpArray = [];
				elIndex = 0;
				nLastSort = [];
				lastSort = [];

				if(sortComponents.length === 0){
					if(Object.prototype.toString.call(arguments[i]) === '[object Array]'){
						arguments[i] = sortArrs[i]; // args -> arguments
					}
					else{
						for(k in arguments[i]){
							if(arguments[i].hasOwnProperty(k)){
								delete arguments[i][k];
							}
						}
						sortArrsLength = sortArrs[i].length;
						for(j = 0, vkey = 0; j < sortArrsLength; j++){
							vkey = sortKeys[i][j];
							arguments[i][vkey] = sortArrs[i][j]; // args -> arguments
						}
					}
					delete sortArrs[i];
					delete sortKeys[i];
					continue;
				}

				var sFunction = sortFunctions[(sortFlag[i]&3)][((sortFlag[i]&8) > 0) ? 1 : 0];

				for(l = 0; l !== sortComponents.length; l += 2){
					tmpArray = sortArrs[i].slice(sortComponents[l], sortComponents[l+1]+1);
					tmpArray.sort(sFunction);
					lastSorts[l] = [].concat(lastSort); // Is there a better way to copy an array in Javascript?
					elIndex = sortComponents[l];
					for(g in tmpArray){
						if(tmpArray.hasOwnProperty(g)){
							sortArrs[i][elIndex] = tmpArray[g];
							elIndex++;
						}
					}
				}

				sFunction = sortDuplicator;
				for(j in sortArrs){
					if(sortArrs.hasOwnProperty(j)){
						if(sortArrs[j] === sortArrs[i]){
							continue;
						}
						for(l = 0; l !== sortComponents.length; l += 2){
							tmpArray = sortArrs[j].slice(sortComponents[l], sortComponents[l+1]+1);
							nLastSort = [].concat(lastSorts[l]); // alert(l + ':' + nLastSort);
							tmpArray.sort(sFunction);
							elIndex = sortComponents[l];
							for(g in tmpArray){
								if(tmpArray.hasOwnProperty(g)){
									sortArrs[j][elIndex] = tmpArray[g];
									elIndex++;
								}
							}
						}
					}
				}

				for(j in sortKeys){
					if(sortKeys.hasOwnProperty(j)){
						for(l = 0; l !== sortComponents.length; l += 2){
							tmpArray = sortKeys[j].slice(sortComponents[l], sortComponents[l+1]+1);
							nLastSort = [].concat(lastSorts[l]);
							tmpArray.sort(sFunction);
							elIndex = sortComponents[l];
							for(g in tmpArray){
								if(tmpArray.hasOwnProperty(g)){
									sortKeys[j][elIndex] = tmpArray[g];
									elIndex++;
								}
							}
						}
					}
				}

				zlast = null;
				sortComponents = [];
				for(j in sortArrs[i]){
					if(sortArrs[i].hasOwnProperty(j)){
						if(!thingsToSort[j]){
							if((sortComponents.length&1)){
								sortComponents.push(j-1);
							}
							zlast = null;
							continue;
						}
						if(!(sortComponents.length&1)){
							if(zlast !== null){
								if(sortArrs[i][j] === zlast){
									sortComponents.push(j-1);
								}
								else{
									thingsToSort[j] = false;
								}
							}
							zlast = sortArrs[i][j];
						}
						else{
							if(sortArrs[i][j] !== zlast){
								sortComponents.push(j-1);
								zlast = sortArrs[i][j];
							}
						}
					}
				}

				if(sortComponents.length&1){
					sortComponents.push(j);
				}
				if(Object.prototype.toString.call(arguments[i]) === '[object Array]'){
					arguments[i] = sortArrs[i]; // args -> arguments
				}
				else{
					for(j in arguments[i]){
						if(arguments[i].hasOwnProperty(j)){
							delete arguments[i][j];
						}
					}

					sortArrsLength = sortArrs[i].length;
					for(j = 0, vkey = 0; j < sortArrsLength; j++){
						vkey = sortKeys[i][j];
						arguments[i][vkey] = sortArrs[i][j]; // args -> arguments
					}

				}
				delete sortArrs[i];
				delete sortKeys[i];
			}
		}
		return true;
	}
};
