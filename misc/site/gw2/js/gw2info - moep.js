// This needs prototypejs


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

	var GW2Info = {
		isInit: false,
		init: function(callback,options)
		{
            localStorage.clear();
            //deleteDatabase();
            console.log("GW2Info: init");
            if (typeof options === 'undefined') options = {};
            GW2Info.options = options;

            // data
            var iDb;
            db.open( {
                        server: 'gw2info',
                        version: 1,
                        schema: {
                            /*people: {
                                key: { keyPath: 'id' , autoIncrement: true },
                                // Optionally add indexes
                                indexes: {
                                    firstName: { },
                                    answer: { unique: true }
                                }
                            },*/
                            build_ids: {
                                key: {},
                            },
                            files: {
                                key: {},
                            },
                            wvw_matches: {
                                key: { keyPath: "wvw_match_id"},
                            },
                            continents: {
                                key: {},
                            },
                            world_names: {
                                key: { keyPath: "id"},
                                indexes: {
                                    id: {},
                                    name_en: {},
                                    name_de: {},
                                    name_es: {},
                                    name_fr: {}
                                }
                            },
                            maps: {
                                key: {},
                            },
                            event_details: {
                                key: {},
                            }
                        }
                    } ).done( function ( s ) {
                        iDb = s;
                        onDbInit();
                    } );

            /*var db;
            var dbrequest = indexedDB.open('gw2info', 1);
            // Create/Change-Event
            dbrequest.onupgradeneeded = function(){
                console.log('dbrequest.onupgradeneeded');
                var db = this.result;
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

            };
            // Open-Event (fires after upgradeneeded)
            dbrequest.onsuccess = function(){
                console.log('db opened successfully');
                db = this.result;
                onDbInit();
            };
            dbrequest.error = function(e){
                console.log("Database error: " + e.target.errorCode);
            };
            var bulkAddToDb = function(store_name,obj,language)
            {
                var trans = db.transaction([store_name,'build_ids'], 'readwrite');
                var store = trans.objectStore(store_name);
                var request;

                if (store_name === 'files')
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
            var getStoreForRead = function(store_name)
            {
                var trans = db.transaction([store_name], 'readonly');
                return trans.objectStore(store_name);
            }
            var getStore = function(store_name, access)
            {
                var trans = db.transaction([store_name], access);
                return trans.objectStore(store_name);
            }

            var requestFromDbByKey = function(store_name, key, callback, args)
            {
                //Todo: type checks
                var trans = db.transaction([store_name], 'readonly');
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
            };*/

			var loadFromStorage = function()
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
			if (!data) data = {};
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
					&& (status.files != states.updating)
					&& (status.wvw_matches != states.updating)
					&& (status.continents != states.updating)
                    //&& (status.world_names != states.updating)
                    && (status.maps != states.updating)
                    && (status.event_details != states.updating))
				{
					GW2Info.isInit = true;
					saveToStorage();
					console.log("GW2Info: init done");
					console.log("Status: build_id:"+status.build_id+" files:"+status.files+" wvw_matches:"+status.wvw_matches+" continents:"+status.continents+" world_names:"+status.world_names+" maps:"+status.maps);
					callback();
				}
			};
			
            var onDbInit = function()
            {
                /********************************************************************************/
                // Files, Continents, World Names
                // Here we're using the build id to determine if cache needs update
                /********************************************************************************/

                // Todo: Another mechanism (on demand)? Data outweighs the sum of the rest by a lot
                /*var updateEventDetails = function()
                {
                    var url_base = 'https://api.guildwars2.com/v1/event_details.json';
                    var count_success = 0;
                    var count_fail = 0;
                    var need_response = options.disable_languages ? 1 : (1 + addLanguages.length);
                    var obj;

                    var cleanup = function()
                    {
                        if (typeof obj === 'object')
                        {
                            var eventdata = {};
                            $H(obj).each(function(e){
                                if ((e[0]) && (e[1]))
                                {
                                    if (e[1].name)
                                        delete e[1].name;

                                    // sort by map_id
                                    if(typeof eventdata[e[1].map_id] === 'undefined'){
                                        eventdata[e[1].map_id] = {};
                                    }
                                    eventdata[e[1].map_id][e[0]] = e[1];
                                }
                            });
                            data.event_details = eventdata;
                        };
                        checkFireInit();
                    };

                    var get = function(language)
                    {
                        if (!language) language = 'en';
                        var url = language ? (url_base + '?lang=' + language) : url_base;
                        new Ajax.Request(url, {
                                method: "get",
                                requestHeaders: {Accept: 'application/json'},
                                onSuccess: function(request){
                                    count_success++;
                                    console.log('GW2Info: Received ' + url);

                                    if ((!language) || language === 'en')
                                    {
                                        obj = request.responseJSON.events;
                                        if (!options.disable_languages)
                                        {
                                            var length = addLanguages.length;
                                            for (var i = 0; i < length; i++) {
                                                get(addLanguages[i]);
                                            }
                                        }
                                    };

                                    $H(request.responseJSON.events).each(function(e){
                                            if ((e[0]) && (e[1]) && (e[1].name))
                                                obj[e[0]]['name_'+language] = e[1].name;
                                        });

                                    if (count_success + count_fail == need_response)
                                    {
                                        data.event_details = obj;
                                        status.event_details = ((count_fail == 0) ? states.updated : states.valid);
                                        cleanup();
                                    }

                                },
                                onFailure: function(err){
                                    count_fail++;
                                    console.log('GW2Info: ajax requesting gw2 api failure (' + url + ')');
                                    if ((!language) || language === 'en')
                                    {
                                        // Fail on en already? Abort further tries.
                                        status.event_details = (data.event_details) ? states.valid : states.invalid;
                                        cleanup();
                                    }
                                    else
                                    {
                                        // fall back to an old translation in good hopes
                                        $H(data.event_details).each(function(e){
                                                if (e[0] && e[1] && e[1]['name_'+language])
                                                    obj[e[0]]['name_'+language] = e[1]['name_'+language];
                                            });

                                        if (count_response == need_response)
                                        {
                                            data.event_details = obj;
                                            status.event_details = states.valid;
                                            cleanup();
                                        }
                                    };
                                }
                            });
                    };
                    get('en');
                };*/

                /*var updateMaps = function()
                {
                    // for each language
                    var url_base = 'https://api.guildwars2.com/v1/maps.json';
                    var count_success = 0;
                    var count_fail = 0;
                    var need_response = options.disable_languages ? 1 : (1 + addLanguages.length);
                    var obj;
                    var cleanup = function()
                    {
                        if (typeof obj === 'object')
                        {
                            $H(obj).each(function(e){
                                if ((e[0]) && (e[1]))
                                {
                                    if (e[1].map_name)
                                        delete e[1].map_name;
                                    if (e[1].region_name)
                                        delete e[1].region_name;
                                    if (e[1].continent_name)
                                        delete e[1].continent_name;
                                }
                            });
                        };
                    };

                    var get = function(language)
                    {
                        if (!language) language = 'en';
                        var url = language ? (url_base + '?lang=' + language) : url_base;
                        new Ajax.Request(url, {
                            method:'get',
                            requestHeaders: {Accept: 'application/json'},
                            onSuccess: function(request) {
                                count_success++;
                                console.log('GW2Info: Received ' + url);

                                if ((!language) || language === 'en')
                                {
                                    obj = {};
                                    $H(request.responseJSON.maps).each(function(e){
                                        var keyInt = parseInt(e[0]);
                                        obj[keyInt] = e[1];
                                    });

                                    //obj = request.responseJSON.maps;
                                    if (!options.disable_languages)
                                    {
                                        var length = addLanguages.length;
                                        for (var i = 0; i < length; i++) {
                                            get(addLanguages[i]);
                                        }
                                    }
                                };

                                $H(request.responseJSON.maps).each(function(e){
                                        if ((e[0]) && (e[1]) && (e[1].map_name))
                                            obj[e[0]]['name_'+language] = e[1].map_name;
                                    });

                                if (count_success + count_fail === need_response)
                                {
                                    data.maps = obj;
                                    status.maps = ((count_fail == 0) ? states.updated : states.valid);
                                    cleanup();
                                    checkFireInit();
                                }
                            },
                            onFailure: function(response) {
                                count_fail++;
                                console.log('GW2Info: ajax requesting gw2 api failure (' + url + ')');
                                if ((!language) || language === 'en')
                                {
                                    // Fail on en already? Abort further tries.
                                    status.maps = (data.maps) ? states.valid : states.invalid;
                                    cleanup();
                                    checkFireInit();
                                }
                                else
                                {
                                    // fall back to an old translation in good hopes
                                    $H(data.maps).each(function(e){
                                            if (e[0] && e[1] && e[1]['name_'+language])
                                                maps[e[0]]['name_'+language] = e[1]['name_'+language];
                                        });

                                    if (count_response === need_response)
                                    {
                                        data.maps = obj;
                                        status.maps = states.valid;
                                        cleanup();
                                        checkFireInit();
                                    }
                                };
                            }
                        });
                    };
                    get('en');
                };*/


                /*var updateWorldNames = function()
                {
                    // for each language
                    var url_base = 'https://api.guildwars2.com/v1/world_names.json';
                    var count_success = 0;
                    var count_fail = 0;
                    var need_response = options.disable_languages ? 1 : (1 + addLanguages.length);
                    var world_names = {};
                    var get = function(language)
                    {
                        if (!language) language = 'en';
                        var url = language ? (url_base + '?lang=' + language) : url_base;
                        new Ajax.Request(url, {
                            method:'get',
                            requestHeaders: {Accept: 'application/json'},
                            onSuccess: function(request) {
                                count_success++;
                                console.log('GW2Info: Received ' + url);

                                var length = request.responseJSON.length;
                                var response = request.responseJSON;
                                for (var i = 0; i < length; i++) {
                                    var id = response[i].id;
                                    if (!world_names[id]) world_names[id] = {};
                                    if ((!language) || language === 'en')
                                        world_names[id].name = response[i].name;
                                    else
                                        world_names[id]['name_'+language] = response[i].name;
                                };

                                if (count_success + count_fail === need_response)
                                {
                                    data.world_names = world_names;
                                    status.world_names = ((count_fail == 0) ? states.updated : states.valid);
                                    checkFireInit();
                                }
                                else if ((!language) || language === 'en')
                                {
                                    if (!options.disable_languages)
                                    {
                                        var length = addLanguages.length;
                                        for (var i = 0; i < length; i++) {
                                            get(addLanguages[i]);
                                        };
                                    };
                                };
                            },
                            onFailure: function(response) {
                                count_fail++;
                                console.log('GW2Info: ajax requesting gw2 api failure (' + url + ')');

                                if ((!language) || language === 'en')
                                {
                                    // Fail on en already? Abort further tries.
                                    status.world_names = (data.world_names) ? states.valid : states.invalid;
                                    checkFireInit();
                                }
                                else
                                {
                                    // fall back to an old translation in good hopes
                                    if (data.world_names)
                                    {
                                        $H(data.world_names).each(function(e){
                                            if (e[0] && e[1] && e[1]['name_'+language])
                                                world_names[e[0]]['name_'+language] = e[1]['name_'+language];
                                        });
                                    };

                                    if (count_response === need_response)
                                    {
                                        data.world_names = world_names;
                                        status.world_names = states.valid;
                                        checkFireInit();
                                    }
                                };
                            }
                        });
                    };
                    get('en');
                };*/
                var doRequestWorldNames = function(callback,language)
                {
                    var url_base = 'https://api.guildwars2.com/v1/world_names.json';
                    var url = language ? (url_base + '?lang=' + language) : url_base;
                    new Ajax.Request(url, {
                        method:'get',
                        requestHeaders: {Accept: 'application/json'},
                        onSuccess: function(request) {
                            console.log('GW2Info: Received ' + url);
                            callback(request.responseJSON,language);

                            /*var response = request.responseJSON;
                            var length = response.length;

                            var world_names = {};
                            for (var i = 0; i < length; i++) {
                                requestFromDbByKey('world_names', response[i].id, function(result,args){
                                    console.log("Inside " + args.id + "; " + args.name);
                                    if(!result)
                                    {
                                        result = {};
                                        result.id = args.id
                                    }
                                    result['name_'+language] = args.name;
                                    var store = getStore('world_names', 'readwrite');
                                    var dbputrequest = store.put(result);
                                    dbputrequest.onsuccess = function(evt){
                                        console.log('Entry ' + evt.target.result + ' saved.');
                                    };
                                    world_names[args.id] = result;
                                },response[i]);
                            };
                            var store = getStore('build_ids', 'readwrite')
                            var dbputrequest = store.put(data.build_id,'world_names_' + language);
                            dbputrequest.onsuccess = function(evt){
                                console.log('Entry ' + evt.target.result + ' saved.');
                            };
                            callback(result);*/
                        },
                        onFailure: function(response) {
                            console.log('GW2Info: ajax requesting gw2 api failure (' + url + ')');
                            callback(false, false);
                        }
                    });
                }

                GW2Info.requestWorldNames = function(callback,language)
                {
                    if (!language) language = 'en';
                    // try database
                    iDb.build_ids.get('world_names_' + language).then(
                                function(result){
                                    if (result && result === GW2Info.build_id)
                                    {
                                        console.log("World names up to date.");
                                        iDb.world_names.query().all().execute().done(
                                                    function(items){
                                                        if (items && items[0])
                                                            callback(items[0]);
                                                    });
                                    }
                                    else
                                    {
                                        if (!result)
                                        {
                                            var count = 0;
                                            console.log("No cached world_names for language");
                                            iDb.world_names.query().all().execute().then(function(){
                                                console.log("cached world then");
                                                console.log(count);
                                            });

                                            doRequestWorldNames(function(world_names,language){
                                                if(world_names && language)
                                                {
                                                    var name = 'name_' + language;
                                                    if (count > 0)
                                                    {
                                                        $A(world_names).each(function(e){
                                                            var modifyObj = {};
                                                            modifyObj[name] = e[1];
                                                            iDb.world_names.query('id', e[0]).modify(modifyObj).execute().done(function(results){
                                                                console.log("Modified world name entry");
                                                            });
                                                        });
                                                    }
                                                    else
                                                    {
                                                        for (var i = 0, len = world_names.length; i < len; ++i) {
                                                            world_names[i][name] = world_names[i].name;
                                                            delete world_names[i].name;
                                                            console.log("ID:" + world_names[i].id);

                                                            iDb.world_names.get(world_names[i].id).then(function(results){
                                                                if(!results)
                                                                {
                                                                    console.log("Have no entry");
                                                                    console.log("ID:" + world_names[i].id);
                                                                    iDb.world_names.update({item:world_names[i]}).then(function(updateResult){
                                                                        console.log("Added entry");
                                                                    });

                                                                }
                                                                else
                                                                    console.log("Modified world name entry");
                                                            });
                                                        };
                                                    }
                                                }
                                                else
                                                {
                                                }
                                            },language);
                                        }
                                        else
                                        {
                                            console.log("Files need update.");
                                        }
                                    }


                        /*if (result !== data.build_id)
                        {
                            console.log('world_names' + language + ' outdated or not present.');
                            doUpdateWorldNames(function(result){
                                if(result)
                                    callback(result);
                            },language);
                        }
                        else
                        {
                            console.log('world_names' + language + ' up to date.');
                            var store = getStore('world_names','readonly');
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var world_names = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.value.key;
                                    world_names[key] = result.value;
                                    world_names[key].name = world_names[key]["name_" + language];
                                    result.continue();
                                };
                            };
                            callback(world_names);
                        };*/
                    });
                };
                //GW2Info.requestWorldNames(function(result){console.log("Moep");},'en');

                /*var updateContinents = function()
                {
                    // for each language
                    var url_base = 'https://api.guildwars2.com/v1/continents.json';
                    var count_success = 0;
                    var count_fail = 0;
                    var need_response = options.disable_languages ? 1 : (1 + addLanguages.length);
                    var continents;
                    var get = function(language)
                    {
                        if (!language) language = 'en';
                        var url = language ? (url_base + '?lang=' + language) : url_base;
                        new Ajax.Request(url, {
                            method:'get',
                            requestHeaders: {Accept: 'application/json'},
                            onSuccess: function(request) {
                                count_success++;
                                console.log('GW2Info: Received ' + url);

                                if ((!language) || language === 'en')
                                {
                                    continents = request.responseJSON.continents;
                                    if (!options.disable_languages)
                                    {
                                        var length = addLanguages.length;
                                        for (var i = 0; i < length; i++) {
                                            get(addLanguages[i]);
                                        }
                                    };
                                }

                                $H(request.responseJSON.continents).each(function(e){
                                    if ((e[0]) && (e[1]) && (e[1].name))
                                        continents[e[0]]['name_'+language] = e[1].name;
                                });

                                if (count_success + count_fail == need_response)
                                {
                                    delete continents.name;
                                    $H(continents).each(function(e){
                                        continents[parseInt(e[0])] = e[1];
                                    });
                                    data.continents = continents;
                                    status.continents = ((count_fail == 0) ? states.updated : states.valid);
                                    checkFireInit();
                                }
                            },
                            onFailure: function(response) {
                                count_fail++;
                                console.log('GW2Info: ajax requesting gw2 api failure (' + url + ')');
                                if ((!language) || language === 'en')
                                {
                                    // Fail on en already? Abort further tries.
                                    status.continents = (data.continents) ? states.valid : states.invalid;
                                    checkFireInit();
                                }
                                else
                                {
                                    // fall back to an old translation in good hopes
                                    $H(data.continents).each(function(e){
                                            if (e[0] && e[1] && e[1]['name_'+language])
                                                continents[e[0]]['name_'+language] = e[1]['name_'+language];
                                        });

                                    if (count_response == need_response)
                                    {
                                        data.continents = continents;
                                        status.continents = states.valid;
                                        checkFireInit();
                                    }
                                };
                            }
                        });
                    };
                    get('en');
                };*/

                GW2Info.RequestFiles = function(callback, key)
                {
                    var doRequestFiles = function(doRequestCallback)
                    {
                        new Ajax.Request('https://api.guildwars2.com/v1/files.json', {
                                             method:'get',
                                             requestHeaders: {Accept: 'application/json'},
                                             onSuccess: function(request) {
                                                 console.log('GW2Info: Received files.json.');
                                                 doRequestCallback(request.responseJSON);
                                             },
                                             onFailure: function(response) {
                                                 console.log('GW2Info: ajax requesting gw2 api failure (files.json)');
                                                 doRequestCallback(false);
                                             }
                                         });
                    };

                    iDb.build_ids.get('files').then(
                                function(result){
                                    if (result && result === GW2Info.build_id)
                                    {
                                        console.log("Files up to date.");
                                        if (key)
                                        {
                                            iDb.files.get(key).then(
                                                        function(items){
                                                            if (items && items[0])
                                                                callback(items[0][key]);
                                                        },
                                                        function(error){    console.log("Error"); callback(false);   },
                                                        function(progress){ console.log("progress");}
                                                        );
                                        }
                                        else
                                        {
                                            // Get All
                                            iDb.files.query().all().execute().then(
                                                        function(items){
                                                            if (items && items[0])
                                                                callback(items[0]);
                                                        },
                                                        function(error){    console.log("Error"); callback(false)   },
                                                        function(progress){ console.log("progress");}
                                                        );

                                        }
                                    }
                                    else
                                    {
                                        if (!result)
                                            console.log("No cached files");
                                        else
                                            console.log("Files need update.");

                                        doRequestFiles(function(response){
                                            if (response)
                                            {
                                                $H(response).each(function(e){
                                                    if (e[0] && e[1])
                                                    {
                                                        iDb.files.update( {
                                                                             key: e[0],
                                                                             item: e[1]
                                                                         } ).done( function ( item ) {
                                                                             // item stored
                                                                         } );
                                                    }
                                                });
                                                iDb.build_ids.update( {
                                                                         key: 'files',
                                                                         item: GW2Info.build_id
                                                                     } ).done( function ( items ) {
                                                                         // item stored
                                                                         console.log("Files updated to " + items[0].item);
                                                                         if (key)
                                                                             callback(response[key]);
                                                                         else
                                                                             callback(response);
                                                                     } );
                                            }
                                            else if (!result)
                                                callback(false);
                                            else
                                                callback(result);

                                        });
                                    }
                                },
                                function(error){    console.log("Error");   },
                                function(progress){ console.log("progress");}
                                );
                };

                //************

                var compareBuildIds = function()
                {
                    var requestBuildId = function(callback)
                    {
                        new Ajax.Request('https://api.guildwars2.com/v1/build.json', {
                                method:'get',
                                requestHeaders: {Accept: 'application/json'},
                                onSuccess: function(request) {
                                    console.log('GW2Info: Received build id: ' + request.responseJSON.build_id);
                                    callback(request.responseJSON.build_id);
                                },
                                onFailure: function(response) {
                                    console.log('GW2Info: ajax requesting gw2 api failure (build_id)');
                                    callback(false);
                                }
                            });
                    };

                    iDb.build_ids.get('_current').then(
                                function(result){
                                    if(!result)
                                    {
                                        console.log("No cached build id.");
                                        result = 0;
                                    }
                                    else
                                        console.log("Cached build id: " + result);

                                    GW2Info.build_id = result;
                                    requestBuildId(function(build_id){
                                        if (!GW2Info.isInit)
                                        {
                                            GW2Info.isInit = true;
                                            if ((typeof build_id !== 'number') && (result === 0))
                                                result--;

                                            callback(result);
                                        };

                                        if (typeof build_id !== 'number')
                                        {

                                        }
                                        else if (build_id !== result)
                                        {
                                            console.log("New Build available");
                                            iDb.build_ids.update( {
                                                                     key: '_current',
                                                                     item: build_id
                                                                 } ).done( function ( items ) {
                                                                     // item stored
                                                                     console.log("Build Id stored: " + items[0].item);
                                                                     GW2Info.build_id = items[0].item;
                                                                 } );
                                        }
                                        else
                                        {
                                            console.log("Build is still current");
                                        }
                                    });
                                },
                                function(error){console.log("Error");},
                                function(progress){console.log("progress");}
                                );

                };
                compareBuildIds();

                /********************************************************************************/
                // WvW Matches
                // Here we're using the end date to determine if cache needs update
                /********************************************************************************/
                /*GW2Info.RequestMatches = function()
                {
                    //Do we need to update matches?
                    var store_name = 'wvw_matches';
                    var store = getStoreForRead(store_name);
                    store.get("1-1").onsuccess = function(event) {
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
                                checkFireInit();
                            }
                            else
                                console.log('Match too old, ends '+ endTime.toDateString() + "; current time: " + d.toDateString() + ". Updating...");
                        }
                        else
                            console.log('Key not found: '+ key + " in " + store_name + ". Updating...");

                        if (needUpdate)
                        {
                            new Ajax.Request('https://api.guildwars2.com/v1/wvw/matches.json', {
                                method:'get',
                                requestHeaders: {Accept: 'application/json'},
                                onSuccess: function(request) {
                                    //data.wvw_matches = request.responseJSON.wvw_matches;
                                    status.wvw_matches = states.updated;
                                    console.log('GW2Info: Received WvW Matches.');
                                    bulkAddToDb('wvw_matches', request.responseJSON.wvw_matches);
                                    checkFireInit();
                                },
                                onFailure: function(response) {
                                    delete data.wvw_matches;
                                    status.wvw_matches = states.invalid;
                                    console.log('GW2Info: ajax requesting gw2 api failure (matches)');
                                    checkFireInit();
                                }
                            });
                        };
                    };
                };*/
            };
		},
		
		// will be cached, but only after first request and until build_id changes
		// localized names in here too that are available no where else.
		// The way localization is handled in the API is sort of the opposite of what I like -.-
		RequestMapFloor : function(continent_id, floor_id, language, callback)
		{
			if (!GW2Info.isInit) return false;
			if (!language) language = 'en';

            if (data.map_floor && data.map_floor[continent_id] && data.map_floor[continent_id][floor_id] && data.map_floor[continent_id][floor_id].languages[language])
				callback(data.map_floor[continent_id][floor_id]);		//return data.map_floor[continent_id][floor_id];	// Fire event!
			else
			{
				if (!data.map_floor) data.map_floor = {};
				if (!data.map_floor[continent_id]) data.map_floor[continent_id] = {};
				if (!data.map_floor[continent_id][floor_id]) data.map_floor[continent_id][floor_id] = {};
				
				var url = 'https://api.guildwars2.com/v1/map_floor.json?continent_id=' + continent_id + '&floor=' + floor_id + '&lang=' + language;
				new Ajax.Request(url, {
					method:'get',
					requestHeaders: {Accept: 'application/json'},
					onSuccess: function(request) {
						// we don't need map "name", "min_level", "max_level", "default_floor", "map_rect", "continent_rect" a gazillion times, it's in our maps.json -.-
                        //ToDo: Shrink stuff in dB to one for all languages
						var response = request.responseJSON;
                        response.continent_id = continent_id;
                        response.floor_id = floor_id;

                        // get rid of stringified ints
                        var regions = {};
                        $H(response.regions).each(function(e){
                            if (e[0])
                                regions[parseInt(e[0])] = e[1];
                            var maps = {};
                            $H(e[1].maps).each(function(e){
                                if (e[0])
                                    maps[parseInt(e[0])] = e[1];
                            });
                            regions.maps = maps;
                        });
                        response.regions = regions;

                        // localize
                        var localize = function(obj)
                        {
                            if (obj.name)
                            {
                                obj['name_'+language] = obj.name;
                                delete obj.name;
                            }
                        }

                        $H(response.regions).each(function(e){
                            if ((e[0]) && (e[1])) {
                                localize(e[1]);
                                $H(e[1].maps).each(function(f){
                                    if (f[1]) {
                                        localize(f[1]);
                                        $A(["points_of_interest", "tasks", "skill_challenges","sectors"]).each(function(g){
                                            if (f[1][g]) {
                                                $A(f[1][g]).each(function(h){
                                                    localize(h);
                                                });
                                            };
                                        });
                                    };
                                });
                            };
                        });


						data.map_floor[continent_id][floor_id] = response;
						
						// Fire event
                        callback(data.map_floor[continent_id][floor_id], true);	// 2nd argument determines if language is available
						// save to local cache
					},
					onFailure: function(response) {
						console.log('GW2Info: ajax requesting gw2 api failure ('+url+')');
						if (data.map_floor[continent_id][floor_id])
							callback(data.map_floor[continent_id][floor_id], false);
						else
							callback();
					}
				});
			};
		},
		
		RegisterMatchDetails : function(callback, match_id)
		{
			var isRunning = false;
			var periodicalCallback = function(pe) {
				if (!confirm('Want me to annoy you again later?'))
					pe.stop();
				/*if (!isRunning)
					pe.stop();*/
				else
				{
					var url = 'https://api.guildwars2.com/v1/wvw/match_details.json?match_id=' + match_id;
					new Ajax.Request(url, {
						method:'get',
						requestHeaders: {Accept: 'application/json'},
						onSuccess: function(request) {
							var response = request.responseJSON;
							// Fire event
							callback(response);
						},
						onFailure: function(response) {
							console.log('GW2Info: ajax requesting gw2 api failure ('+url+')');
						}
					});
				};
			};
			var start = function() { if (!isRunning) new PeriodicalExecuter(periodicalCallback, 5);};
			var stop = function() {isRunning = false};
		}
	};
	global.GW2Info = GW2Info;
})();
