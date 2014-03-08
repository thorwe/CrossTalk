// This needs prototypejs


// Enable XSS... errr... CORS for Prototype: http://kourge.net/node/131
// the console will tell you: Refused to get unsafe header "X-JSON"
/*Ajax.Responders.register({
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
});*/

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
	
    // awesomium lagging behind
    var indexedDB = indexedDB || window.indexedDB || window.webkitIndexedDB || window.mozIndexedDB || window.oIndexedDB || window.msIndexedDB,
        IDBKeyRange = window.IDBKeyRange || window.webkitIDBKeyRange;

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
            //localStorage.clear();
            deleteDatabase();
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
            var db;
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
                if(!db.objectStoreNames.contains('map_floor')){
                    store = db.createObjectStore('map_floor', {
                                                     keyPath: "id"
                                                 });
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
            };

            var requestFromDbAll = function(store_name, callback, args)
            {
                //Todo: type checks
                var trans = db.transaction([store_name], 'readonly');
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
                        var key = result.value.key;
                        obj[key] = result.value;
                        result.continue();
                    }
                    else
                        callback(obj,args);
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
            var data = {};
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
                    var trans = db.transaction([store_name], 'readwrite');
                    var store = trans.objectStore(store_name);
                    var key;
                    if (ool_key)
                        key = ool_key;
                    else if (store.keyPath)
                        key = responseJSON[store.keyPath];
                    else
                    {
                        console.log('No access to key, aborting.');
                        callback(false);
                        return;
                    };

                    store.get(key).onsuccess = function(event) {
                        var result = event.target.result;
                        //console.log('Key ' + (result ? '' : 'not ') + 'found: ' + key + ' in ' + store_name);
                        if(!result)
                            result = {};

                        result = $H(result).merge(responseJSON).toObject();

                        var dbputrequest = ool_key ? (store.put(result, key)):(store.put(result));
                        dbputrequest.onsuccess = function(evt){
                            callback(result);
                        };
                    };
                };

                var updateDbEntries = function(callback, store_name, responseJSON, inlineKeyPath)
                {
                    var obj = {};

                    //Todo: type checks
                    var trans = db.transaction([store_name], 'readwrite');
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

                /********************************************************************************/
                // Files, Continents, World Names
                // Here we're using the build id to determine if cache needs update
                /********************************************************************************/


                GW2Info.requestEventDetails = function(callback,language)
                {
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
                                        var store = getStore('build_ids', 'readwrite')
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
                            var store = getStore(file,'readonly');
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var obj = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.value.key;
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
                                        var store = getStore('build_ids', 'readwrite')
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
                                        var store = getStore('build_ids', 'readwrite')
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
                            var store = getStore(file,'readonly');
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var obj = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.value.key;
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
                                        var store = getStore('build_ids', 'readwrite')
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
                            var store = getStore('files','readonly');
                            // Cursor for all entries, 0 to finish
                            var range = IDBKeyRange.lowerBound(0);
                            var cursorRequest = store.openCursor(range);

                            // Called once for every entry...and one time extra
                            var files = {};
                            cursorRequest.onsuccess = function(evt){
                                var result = evt.target.result;
                                if(result)
                                {
                                    var key = result.value.key;
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
                    var store = getStore('build_ids','readwrite');
                    data.build_id = build_id ? build_id : 0;
                    status.build_id = (data.build_id)?states.updated:states.invalid;
                    console.log("onNewBuild: " + data.build_id);

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

                /********************************************************************************/
                // WvW Matches
                // Here we're using the end date to determine if cache needs update
                /********************************************************************************/
                GW2Info.requestWvWMatches = function()
                {
                    //Do we need to update matches?
                    var store_name = 'wvw_matches';
                    var store = getStoreForRead(store_name);
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
                            console.log('Key not found: '+ testkey + " in " + store_name + ". Updating...");

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
                                },
                                onFailure: function(response) {
                                    delete data.wvw_matches;
                                    status.wvw_matches = states.invalid;
                                    console.log('GW2Info: ajax requesting gw2 api failure (matches)');
                                }
                            });
                        };
                    };
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
                                    responseJSON.id = '' + continent_id + '_' + floor_id;

                                    updateDbEntry(function(obj){
                                        var store = getStore('build_ids', 'readwrite')
                                        var dbputrequest = store.put(data.build_id, (file + '_' +language + '_' + continent_id + '_' + floor_id));
                                        dbputrequest.onsuccess = function(evt){
                                            console.log('Entry ' + evt.target.result + ' saved.');
                                            callback(obj);
                                        };
                                    }, file, responseJSON, false);

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
                            requestFromDbByKey(file, ('' + continent_id + '_' + floor_id), function(dBresult){
                                callback(dBresult);
                            });
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
                        objObj[f.id] = f;
                        //delete f.id;
                    });
                    e.objectives = objObj;
                    obj[e.type] = e;
                    //delete e.type;
                });
                responseJSON.maps = obj;
            };


            publisher.on(match_id,callback,context);
            if (Array.length(publisher.subscribers[match_id]) === 1)
            {
                new PeriodicalExecuter( function(pe) {
                    var url = 'https://api.guildwars2.com/v1/wvw/match_details.json?match_id=' + match_id;
                    new Ajax.Request(url, {
                        method:'get',
                        requestHeaders: {Accept: 'application/json'},
                        onSuccess: function(request) {
                            if (Array.length(publisher.subscribers[match_id]) === 0)
                                pe.stop();
                            else
                            {
                                var response = request.responseJSON;
                                cleanup(response);
                                publisher.fire(match_id,response);
                            }
                        },
                        onFailure: function(response) {
                            console.log('GW2Info: ajax requesting gw2 api failure ('+url+')');
                            if (Array.length(publisher.subscribers[match_id]) === 0)
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
                        if (Array.length(publisher.subscribers[url]) === 0)
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
            }, 5);
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
