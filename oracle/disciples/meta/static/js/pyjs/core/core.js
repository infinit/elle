/**
 * @fileOverview Core PyJS file, the first to be loaded
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */
/*jslint sloppy: true, eqeq: true, white: true, maxerr: 50, indent: 4, nomen: true, plusplus: true */
/*global clearInterval: false, clearTimeout: false, document: false, event: false,
         frames: false, history: false, Image: false, location: false, name: false,
         navigator: false, Option: false, parent: false, screen: false, setInterval: false,
         setTimeout: false, window: false, XMLHttpRequest: false, ActiveXObject: false,
         console: true, log: true, warn: true, pyConfig: true,
         KeyError: true, ValueError: true, TypeError: true, PyError: true,
         StopIteration: true, Element: true
*/

if (typeof console != "undefined") {
    if (typeof console.log.apply != "undefined") {
        if (typeof log === "undefined") {
            log = function() {
                try {
                    console.log.apply(console, arguments);
                } catch (err) {}
            };
        }
        if (typeof warn === "undefined") {
            warn = function() {
                try {
                    console.warn.apply(console, arguments);
                } catch (err) {}
            };
        }
    } else {
        if (typeof log === "undefined") {
            log = console.log;
        }
        if (typeof warn === "undefined") {
            warn = console.warn;
        }
    }
} else {
    log = warn = function() {};
}

py = window.py = (function(){
    var dom_loaded = false,
        listenOnLoad = function () { // http://www.kryogenix.org/days/2007/09/26/shortloaded
            var i = function() {
                    dom_loaded = true;
                    log("Dom loaded ");
                    if (typeof(py) !== "undefined" && py._onLoad) {
                        py._onLoad();
                    }
                },
                u = navigator.userAgent,
                explorerTrick = function () {
                    var t = document.createElement('doc:rdy');
                    try {
                        t.doScroll('left');
                        i();
                        t = null;
                    } catch (err) {
                        setTimeout(explorerTrick, 1);
                    }
                },
                webkitTrick = function() {
                    var dr = document.readyState;
                    if (dr == "loaded" || dr == "complete") {
                        i();
                    } else {
                        setTimeout(webkitTrick, 10);
                    }
                },
                e = /*@cc_on!@*/false;
            if (/webkit/i.test(u)) {
                setTimeout(webkitTrick, 10);
            }
            else if ((/mozilla/i.test(u) && !/(compati)/.test(u)) || (/opera/i.test(u))) {
                document.addEventListener("DOMContentLoaded", i, false);
            } else if (e) {
                explorerTrick();
            } else {
                window.onload = i;
            }
        };

    /**
     * Global PyJS namespace
     * @namespace
     */
    var py = {

        config: {
            extremist: false,
            withGlobals: true,
            preventCache: true
        },

        // Contains directories path for each namespaces
        _modules_path: {},

        // Contains loaded modules
        _loaded_modules: [
            'py.core.core' // this is the name of this module !
        ],

        // modules to be loaded
        _pending_modules: [],

        /** alias of document */
        doc: null,

        /** alias of body */
        body: null,

        __init__: function() {
            log("Loading PyJS");
            this._findGlobals();
            this._initPyModulesPath();
            this._init_fired = true;
        },

        _findGlobals: function() {
            //TODO complete for browser compatibility
            py.doc = window.document;
            py.body = py.doc.body;
            if (!py.body) {
                py.addOnLoad(function() {
                    py.body = py.doc.body;
                });
            }
            if (typeof pyConfig !== "undefined") { // config from user
                var c;
                for (c in pyConfig) {
                    if (pyConfig.hasOwnProperty(c)) {
                        this.config[c] = pyConfig[c];
                    }
                }
            }
        },

        getHeader: function() {
            var h = document.getElementsByTagName('head')[0];
            if (!h) {
               h = document.documentElement.firstChild;
            }
            return (h);
        },

        _initPyModulesPath: function() {
            var header = this.getHeader(),
                scripts = [],
                py_src = null,
                src,
                i;
            for (i = 0; i < header.childNodes.length; i++) {
                if (header.childNodes[i].tagName &&
                    header.childNodes[i].tagName.toLowerCase() == 'script') {
                    scripts.push(header.childNodes[i]);
                }
            }
            for (i=scripts.length-1; i>-1; i--) {
                src = scripts[i].getAttribute('src');
                if (scripts[i].src.indexOf('pyjs/core/core.js') != -1) {
                    py_src = src;
                    break;
                }
            }
            if (py_src) {
                py._modules_path.py = py_src.replace('core/core.js', '');
            } /*<debug*/ else {
                warn("PyJS module URL not found !");
            } /*debug>*/
        },

        registerModulePath: function(prefix, path) {
            /*<debug*/
            py.raiseNone(prefix);
            py.raiseNone(path);
            if (!py.isinstance(prefix, String) || !py.isinstance(path, String)) {
                throw TypeError('prefix and path arguments must be String');
            }
            if (prefix.length === 0) {
                throw new ValueError('prefix must be a non empty String');
            }
            if (prefix.contains('.')) {
                throw new ValueError('prefix cannot contains the "." character');
            }
            if (path.length === 0) {
                throw new ValueError('path must be a non empty String');
            }
            if (prefix.isIn(this._modules_path)) {
                if (this._modules_path[prefix] !== path.rstrip('/') + '/') {
                    throw new Error("The prefix "+ prefix +" was already registered as " + this._modules_path[prefix]);
                }
                return;
            }
            /*debug>*/
            path = path.rstrip('/') + '/';
            this._modules_path[prefix] = path;
        },

        loadJsFromOtherDomain: function(/*String*/ url) {
            var head = py.doc.getElementsByTagName('head')[0],
            loader = py.dom.create('script');
            loader.src = url;
            head.appendChild(loader);
        },


        /** Load JavaScript from url
         * @param {String} url url to load
         * @returns {String} source code
         */
        loadJs: function(/*String*/ url) {
            if (py.config.preventCache === true) {
                var d = new Date();
                if (url.indexOf('?') === -1) {
                    url += '?';
                }
                else {
                    url += '&';
                }
                url += 'preventCache=' + d.getTime().toString() + '.' + d.getMilliseconds().toString();
            }
            var xhr = this.xhrObj();
            xhr.open('GET', url, false);
            xhr.send(null);
            if (!this.isXhrOk(xhr)) {
                throw "Cannot load URI " + url;
            } else {
                return xhr.responseText;
            }
        },

        /**
         * Get the URL of a Module name
         * @param {String} name dotted module name
         * @returns {String} Url of the module
         */
        getModuleUrl: function(name) {
            var parts = name.split('.'), folder, filename, i, l;
            if (parts.length == 1) {
                folder = py._modules_path.py.replace('pyjs/', '');
                filename = name+'.js';
            } else {
                if (this._modules_path[parts[0]]) {
                    folder = this._modules_path[parts[0]];
                    parts.shift();
                } else {
                    folder = py._modules_path.py.replace('pyjs/', '');
                }
                for (i = 0, l = parts.length; i < l - 1; i++) {
                    folder += parts[i] + '/';
                }
                filename = parts.pop() + '.js';
            }
            return folder + filename;
        },

        /**
         * Import module from dotted name
         * @param {String} name the module name
         * @private
         */
        _importModule: function(/*String*/ name) {
            var i, len;
            for (i = 0, len = this._loaded_modules.length; i < len; ++i)
            {
                if (this._loaded_modules[i] === name) {
                    return;
                }
            }
            log('import module: ', name);
            var src = this.loadJs(py.getModuleUrl(name));
            try {
                this.globalEval("(function(){"+src+"})()");
            } catch (err) {
                throw "Error while loading module " + name + ' : ' + err;
            }
            this._loaded_modules.push(name);
        },

        /**
         * Import module from a dotted name
         * @param {String} the module name
         */
        importModule: function(name) {
            if (this._can_import_modules) {
                this._importModule(name);
            } else {
                log('Add', name, 'to queue');
                this._pending_modules.push(name);
            }
        },


        _importPendingModules: function _importPendingModules() {
            var m;
            this._can_import_modules = true;
            while ((m = py._pending_modules.shift())) {
                py._importModule(m);
            }
            // Just in case...
            if (this._pending_modules.length > 0) {
                this._importPendingModules();
            }
        },

        /**
         * Cross browser XMLHttpRequest
         * @returns {XMLHttpRequest} XmlHTTPRequest Object
         */
        xhrObj: function() {
            var methods = {
                    f1: function() {
                        return new ActiveXObject('Msxml2.XMLHTTP');
                    },
                    f2: function() {
                        return new ActiveXObject('Microsoft.XMLHTTP');
                    },
                    f3: function() {
                        return new ActiveXObject('Msxml2.XMLHTTP.4.0');
                    },
                    f4: function() {
                        return new XMLHttpRequest();
                    }
                },
                xhr_obj = null,
                working_method,
                setWorkingMethod = function() { py.xhrObj = working_method; },
                i;
            // According to Dojo, we try ActiveXObject before others
            // beceause in IE7, when local loading, XmlHttpRequest() fails
            for (i = 1; i < 5; i++) {
                try {
                    working_method = methods['f' + i.toString()];
                    xhr_obj = working_method();
                } catch (e) {
                    xhr_obj = null;
                }
                if (xhr_obj !== null) {
                    setTimeout(setWorkingMethod, 1);
                    return xhr_obj;
                }
            }
            throw "XmlHttpRequest is not available";
        },

        /**
         * Verify if an XHR status is OK
         * @params {XMLHttpRequest} xhr
         * @returns {Boolean} true if XHR has no error
         */
        isXhrOk: function(xhr) {    // From Dojo
            var stat = xhr.status || 0;
            return (
                stat >= 200 && stat < 300) ||       // allow any 2XX response code
                stat == 304 ||
                stat == 1223 ||                     // get it out of the cache
                (!stat && (location.protocol=="file:" || location.protocol=="chrome:")
            );
        },

        /**
         * Eval anything in the global scope
         * @param {String} text Source code to eval
         */
        globalEval: function(text) {
            if (py.global.execScript) {
                py.global.execScript(text, "javascript");
            } else if (py.global.eval) {
                py.global.eval(text);
            } else {
                eval(text);
            }
        },

        // true when all base modules and pending modules are loaded
        _can_fire_onload: false,

        _onload_fired: false,
        _onload_callbacks: [],

        _onLoad: function _onLoad() {
            if (this._can_fire_onload && dom_loaded) {
                var f;
                while (f = py._onload_callbacks.shift()) {
                    try {
                        f();
                    } catch (err) {
                        warn('Error with callback ' + f.toString() + ' : ', err);
                    }
                }
                this._onload_fired = true;
            }
        },

        /**
         * Add a function to be fired when dom is ready
         * @param {Function} f function to fire
         */
        addOnLoad: function addOnLoad(f) {
            // It is py.browser._onLoad that use _onload_callbacks
            // and set _onload_fired to true
            /*<debug*/
            if (typeof f != "function") {
                throw TypeError('f must be a function');
            }
            /*debug>*/
            if (this._onload_fired) {
                f();
            } else {
                this._onload_callbacks.push(f);
            }
        }

    };


    py.global = this;
    listenOnLoad();
    var __init_pyjs_interval__ = null,
        __init_pyjs__ = function() {
        if (!py.getHeader()) {
            return (false);
        }
        if (__init_pyjs_interval__) {
            clearInterval(__init_pyjs_interval__);
            __init_pyjs_interval__ = null;
        }
        py.__init__();
        py._importPendingModules();
        py._can_fire_onload = true;
        py._onLoad();
        return (true);
    };
    if (!__init_pyjs__()) {
        __init_pyjs_interval__ = setInterval(__init_pyjs__, 10);
    }
    return py;

})();

py.importModule('py.prototype.__init__');
py.importModule('py.core.class');
py.importModule('py.core.error');
py.importModule('py.core.utils');
py.importModule('py.core.browser');
py.importModule('py.core.dom');
if (py.config.withGlobals === true) {
    py.importModule('py.core.globals');
}
py.importModule('py.core.event');
py.importModule('py.core.browser_event');
//<debug
py.importModule('py.packer.packer');
py.packMe = function packMe(args) {
    if (!args) {
        args = {'lvl': 0};
    }
    var modules = py._loaded_modules.filter(function(mod) {
        return !mod.startswith("py.packer");
    });
    py.packer.pack(modules, args);
}
//debug>
