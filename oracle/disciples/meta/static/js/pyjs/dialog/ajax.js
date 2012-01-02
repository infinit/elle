/**
 * @fileOverview base class for dialogs
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */

py.importModule('py.dialog.dialog');
py.importModule('py.dialog.modal');
py.importModule('py.defer.xhr');


/**
 * Mixin for ajax dialog
 * @name py.dialog.AjaxMixin
 * @class
 */
py.declare('py.dialog.AjaxMixin', null, {
    /** @lends py.dialog.AjaxMixin.prototype */

    /** Loading message */
    _loading_str: "Loading...",

    /** Error message */
    _error_str: "Loading error",

    /** Current deferred */
    _defer: null,

    /** fetch data from given url */
    _fetchData: function () {
        if (py.notNone(this._defer)) {
            return ;
        }
        //<debug
        if (py.isNone(this._args.url)) {
            throw new ValueError("Cannot find url in args");
        }
        if (!py.isinstance(this._args.url, String)) {
            throw new TypeError("url must be a String");
        }
        //debug>
        this._content_loaded = false;
        var self = this;
        this._setContent(this._loading_str);
        this._defer = py.defer.xhrGet({
            url: self._args.url,
            prevent_cache: self._args.prevent_cache,
            onLoad: self._onLoad.bind(self),
            onError: self._onError.bind(self)
        });
    },

    /** callback when data is available */
    _onLoad: function _onLoad(res) {
        this._args.content = res;
        this._setContent();
        this._defer = null;
    },

    /** error callback */
    _onError: function _onError() {
        this._args.content = this._error_str;
        this._setContent();
        this._defer = null;
    }

});

/** @class */
py.declare('py.dialog.AjaxDialog', [py.dialog.Dialog, py.dialog.AjaxMixin], {
    /** @lends py.dialog.AjaxDialog.prototype */

    /**
     * Dialog that fetch content from an url
     * @constructs
     * @augments py.dialog.Dialog
     * @augments py.dialog.AjaxMixin
     * @param {Object} args arguments for the dialog
     * @param {String} args.url Url to use
     * @param {Boolean} [args.refresh_on_show] refresh every time show() is called
     * @param {Boolean} [args.prevent_cache] prevent browser caching
     */
    __init__: function __init__(args) {
        this.$super(arguments);
    },

    show: function() {
        if (this._content_loaded === false || this._args.refresh_on_show === true) {
            this._fetchData();
        }
        this.$super(arguments);
    }
});


/** @class */
py.declare('py.dialog.AjaxModal', [py.dialog.Modal, py.dialog.AjaxMixin], {
    /** @lends py.dialog.AjaxModalDialog.prototype */

    /**
     * Dialog that fetch content from an url
     * @constructs
     * @augments py.dialog.Dialog
     * @param {Object} args arguments for the dialog
     * @param {String} args.url Url to use
     * @param {Boolean} [args.refresh_on_show] refresh every time show() is called
     */
    __init__: function __init__(args) {
        this.$super(arguments);
    },

    show: function() {
        log('content loaded =',this._content_loaded);
        if (this._content_loaded === false || this._args.refresh_on_show === true) {
            this._fetchData();
        }
        this.$super(arguments);
    }
});

