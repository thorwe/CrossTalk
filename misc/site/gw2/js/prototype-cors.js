/**
 * Enable CORS for prototype.js and get rid of the message "Refused to get unsafe header X-JSON"
 * play with it: http://jsfiddle.net/Ajx2k/11/
 *
 * Created by Smiley <@codemasher> on 11.01.14.
 */

// Enable CORS for Prototype: http://kourge.net/node/131
// With this, the console will still tell you: Refused to get unsafe header "X-JSON"
Ajax.Responders.register({
	onCreate: function(response){
		var t = response.transport;
		//for PHPStorm's sake
		//noinspection FunctionWithInconsistentReturnsJS
		t.setRequestHeader = t.setRequestHeader.wrap(function(original, k, v){
			if(/^(accept|accept-language|content-language)$/i.test(k) ||
				(/^content-type$/i.test(k) && /^(application\/x-www-form-urlencoded|multipart\/form-data|text\/plain)(;.+)?$/i.test(v))){
				return original(k, v);
			}
			//for PHPStorm's sake
			//noinspection UnnecessaryReturnStatementJS
			return;
		});
	}
});

// So, then lets get rid of this nasty message
// it is caused by _getHeaderJSON() which tries to receive this header

// the long way if you want to keep the original methods

// so let's override the original method in Ajax.Response
// see: http://prototypejs.org/learn/class-inheritance
Ajax.JSONResponse = Class.create(Ajax.Response, {
	_getHeaderJSON: Prototype.emptyFunction
});

// next, we need to replace all occurences of Ajax.Response with our new class Ajax.JSONResponse
// this is kinda lazy but works fine - more elegant anyone?
Ajax.JSON = Class.create(Ajax.Request, {
	request: function(url) {
		this.url = url;
		this.method = this.options.method;
		var params = Object.isString(this.options.parameters) ?
			this.options.parameters :
			Object.toQueryString(this.options.parameters);

		if (!['get', 'post'].include(this.method)) {
			params += (params ? '&' : '') + "_method=" + this.method;
			this.method = 'post';
		}

		if (params && this.method === 'get') {
			this.url += (this.url.include('?') ? '&' : '?') + params;
		}

		this.parameters = params.toQueryParams();

		try {
			var response = new Ajax.JSONResponse(this);
			if (this.options.onCreate) this.options.onCreate(response);
			Ajax.Responders.dispatch('onCreate', this, response);

			this.transport.open(this.method.toUpperCase(), this.url,
				this.options.asynchronous);

			if (this.options.asynchronous) this.respondToReadyState.bind(this).defer(1);

			this.transport.onreadystatechange = this.onStateChange.bind(this);
			this.setRequestHeaders();

			this.body = this.method == 'post' ? (this.options.postBody || params) : null;
			this.transport.send(this.body);

			/* Force Firefox to handle ready state 4 for synchronous requests */
			if (!this.options.asynchronous && this.transport.overrideMimeType)
				this.onStateChange();

		}
		catch (e) {
			this.dispatchException(e);
		}
	},

	respondToReadyState: function(readyState) {
		var state = Ajax.Request.Events[readyState], response = new Ajax.JSONResponse(this);

		if (state == 'Complete') {
			try {
				this._complete = true;
				(this.options['on' + response.status]
					|| this.options['on' + (this.success() ? 'Success' : 'Failure')]
					|| Prototype.emptyFunction)(response, response.headerJSON);
			} catch (e) {
				this.dispatchException(e);
			}

			var contentType = response.getHeader('Content-type');
			if (this.options.evalJS == 'force'
				|| (this.options.evalJS && this.isSameOrigin() && contentType
				&& contentType.match(/^\s*(text|application)\/(x-)?(java|ecma)script(;.*)?\s*$/i)))
				this.evalResponse();
		}

		try {
			(this.options['on' + state] || Prototype.emptyFunction)(response, response.headerJSON);
			Ajax.Responders.dispatch('on' + state, this, response, response.headerJSON);
		} catch (e) {
			this.dispatchException(e);
		}

		if (state == 'Complete') {
			this.transport.onreadystatechange = Prototype.emptyFunction;
		}
	}
});


// TL;DR: the short and nasty way:
Ajax.Response.prototype._getHeaderJSON = Prototype.emptyFunction;


// ok, lets test it
/*
// native Ajax.Request
var $AJAX = function(url, fn){
	new Ajax.Request(url, {
		method: 'get',
		onSuccess: function(r){
			fn(r.responseJSON);
		}
	});
};

// our new Ajax.JSON
var $JSON = function(url, fn){
	new Ajax.JSON(url, {
		method: 'get',
		onSuccess: function(r){
			fn(r.responseJSON);
		}
	});
};

var url = "https://api.guildwars2.com/v1/world_names.json";
var fn = function(json){
	console.log(json)
};

$AJAX(url, fn);

$JSON(url, fn);*/
