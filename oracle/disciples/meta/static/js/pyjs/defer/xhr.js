/**
 * @fileOverview XMLHTTPRequest manipulation
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.2
 */

/*global ValueError, setTimeout */

py.importModule('py.defer.deferred');

py.declare('py.defer.XmlHttpRequest', null, {

    /**
     * This object fire an XMLHTTPRequest
     * @param {Object} args Arguments for the xhr
     * @param {String} args.url Url
     * @param {String} [args.method] Method to use (GET, POST)
     * @param {Object} [args.query] Query objet to use (transformed to URL with GET method)
     * @param {Object} [args.content] Content to send (When POST)
     * @param {String} [args.username] User name when authentication is needed
     * @param {String} [args.password] Password when authentication is needed
     * @param {Array[]} [args.headers] Additionnal headers ([name, value])
     * @param {String} [args.mime_type] Mime type to use
     * @param {Boolean} [args.prevent_cache] set this to true to prevent browser caching
     * @param {Function} [args.onLoad] callback when data is available
     * @param {Function} [args.onError] callback when error happened
     */
    __init__: function __init__(args) {
        //<debug
        if (py.isNone(args) || !py.isinstance(args, Object)) {
            throw new ValueError("Argument must be an Object");
        }
        if (py.isNone(args.url)) {
            throw new ValueError("URL must be specified");
        }
        if (py.notNone(args.method) && !args.method.isIn(['GET', 'POST'])) {
            throw new ValueError("Method must neither GET or POST");
        }
        if (py.notNone(args.headers) && !py.isinstance(args.headers, Array)) {
            throw new ValueError("Headers must an Array of pairs");
        }
        //debug>
        this.options = {
            method: 'GET',
            content: null
        };
        this.options.update(args);
        this.xhr = py.xhrObj();
        this._fire();
    },

    _reset: function() {
        var req = this.xhr;
        setTimeout(function() {
            try {
                req.onreadystatechange = null;
            } catch(e) {
                warn('Cannot reset onreadystatechange !');
            }
        }, 0);
    },

    _onReadyStateChange: function() {
        if (this.xhr.readyState != 4) { return; }
        this._reset();
        if (py.isXhrOk(this.xhr)) {
            this.onLoad();
        } else {
            this.onError();
        }
    },

    _fire: function () {
        var o = this.options;
        if (o.prevent_cache) {
            var d = new Date();
            if (py.isNone(o.query)) {
                o.query = {};
            }
            o.query.prevent_cache = d.getTime()+'.'+d.getMilliseconds();
        }
        var url = py.buildUrl(o.url, o.query);
        if (py.notNone(o.username)) {
            this.xhr.open(o.method, url, true, o.username, o.password);
        } else {
            this.xhr.open(o.method, url, true);
        }
        this.xhr.setRequestHeader("X-Requested-With", "XMLHttpRequest");
        if (py.notNone(o.headers)) {
            o.headers.iter(function(pair) {
                this.xhr.setRequestHeader(pair[0], pair[1]);
            }, this);
        }
        if (py.isNone(o.content)) {
            o.content = "";
        }

        try {
            this.xhr.onreadystatechange = this._onReadyStateChange.bind(this);
            this.xhr.send(o.content);
        } catch(err) {
            this._reset();
            this.onError(err);
        }
    },

    onLoad: function onLoad() {
        if (this.options.onLoad) {
            this.options.onLoad(this.xhr.responseText, this.xhr);
        }
    },

    onError: function onError(err) {
        if (this.options.onError) {
            this.options.onError(err, this.xhr);
        }
    }

});


py.extendNamespace('py.defer', {

    _xhrMethod: function(params, method) {
        /*<debug*/
        py.raiseNone(params);
        if (!py.isinstance(params, String, Object)) {
            throw new ValueError("params must be either a String or an Object");
        }
        /*debug>*/
        if (py.isinstance(params, String)) {
            params = {url: params};
        }
        params.method = method;
        var d = new py.defer.Deferred(),
            _onload = params.onLoad,
            _onerror = params.onError;
        params.onLoad = d.callback.bind(d);
        params.onError = d.errback.bind(d);
        if (py.notNone(_onload)) {
            d.addCallbacks({callback: _onload, errback: _onerror});
        } else if (py.notNone(_onerror)) {
            d.addCallbacks({
                callback: function(res, xhr) { return res;},
                errback: _onerror
            });
        }
        new py.defer.XmlHttpRequest(params);
        return d;
    },

    xhrGet: function xhrGet(params) {
        return (this._xhrMethod(params, 'GET'));
    },

    xhrPost: function xhrPost(params) {
        /*<debug*/
        if (!py.isinstance(params, Object) || py.isNone(params.query)) {
            throw new ValueError("params must be an object and have a key 'query'");
        }
        /*debug>*/
        var parts = [];
        params.query.iteritems(function(k, v) {
            parts.append(
                encodeURIComponent(k) + '=' + encodeURIComponent(v)
            );
        });
        params.content = '&'.join(parts);
        if (py.isNone(params.headers)) {
            params.headers = [];
        }
        params.headers.append(["Content-Type", "application/x-www-form-urlencoded"]);
        //try {
        //    params.headers.append(["Content-Length", py.len(params.content)]);
        //} catch (err) {}
        delete params["query"];
        return (this._xhrMethod(params, 'POST'));
    }
});


