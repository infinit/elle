/**
 * @fileOverview base class for dialogs
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */

/**
 * Dialog classes namespaces
 * @namespace
 * @name py.dialog
 */

/** @class */
py.declare('py.dialog.Dialog', null, {
    /** @lends py.dialog.Dialog.prototype */

    /** main container node */
    _node: null,

    /** content node */
    _content_node: null,

    /** Title bar node, contains _title_node and _close_button */
    _titlebar_node: null,

    /** title  node */
    _title_node: null,

    /** close button node */
    _close_button: null,

    /** the background node (an iframe) */
    _background_node: null,

    /** usefull boolean !*/
    _content_loaded: false,

    /** max width ratio */
    _max_width: 0.95,

    /** max height ratio */
    _max_height: 0.85,

    /** another usefull boolean ! */
    _is_shown: false,

    /** store __init__ argument */
    _args: null,

    /** resize handler */
    _resize_hdlr: null,

    /** scroll handler */
    _scroll_hdlr: null,

    /**
     * Template string to be placed in _node
     */
    template: '<div class="pyDialogTitlebar">'+
                '<span class="pyDialogTitle"></span>'+
                '<span class="pyDialogCloseButton">X</span>'+
              '</div>'+
              '<div class="pyDialogContent" style="overflow: auto"></div>',

    /**
     * Base dialog class
     * @param {Object} args Constructs arguments
     * @param {String} [args.title] Popup title
     * @param {String} args.content Popup content
     * @param {Function} [args.onCancel] function to call when close popup
     * @param {Boolean} [args.draggable=true] enable or disable drag&drop
     * @constructs
     */
    __init__: function __init__(args) {
        this._node = $c('div', {"class": "pyDialog pyDialogContainer"});
        py.body.appendChild(this._node);
        this._node.innerHTML = this.template;
        this._content_node = this._node.query('div.pyDialogContent')[0];
        this._titlebar_node = this._node.query('div.pyDialogTitlebar')[0];
        this._title_node = this._node.query('span.pyDialogTitle')[0];
        this._close_button = this._node.query('span.pyDialogCloseButton')[0];
        this._args = args || {};
        this._initNode();
        this._createBackground();
        this._resize_hdlr = py.subscribe('py/window/resize', this, this._refresh);
        this._scroll_hdlr = py.subscribe('py/window/scroll', this, this._refresh);
        this._close_hdlr = this._close_button.connect('click', this._onCancel.bind(this));
        if (args.draggable !== false) {
            this._initDnd();
        }
    },

    /** put the content in _content_node */
    _setContent: function _setContent() {
        if (py.notNone(this._args.content)) {
            this._content_node.attr('content', this._args.content);
            this._content_loaded = true;
        }
        this._refresh();
    },

    /** prepare _node styles */
    _initNode: function _initNode() {
        this._node.setStyles({
            'z-index': '100',
            'position': 'absolute',
            'top': '-9999px',
            'left': '-9999px',
            'width': '0px',
            'height': '0px',
            'border': '1px solid black',
            'overflow': 'auto',
            'background-color': 'white'
        });
        this._setContent();
        if (py.notNone(this._args.title)) {
          this._title_node.attr('content', this._args.title);
        }
    },


    _start_dnd_hdlr: null,
    _dnd_move_hdlr: null,
    _dnd_bg_move_hdlr: null,
    _dnd_release_hdlr: null,
    _dnd_initial_x: 0,
    _dnd_initial_y: 0,
    _node_initial_x: 0,
    _node_initial_y: 0,

    _initDnd: function _initDnd() {
        this._start_dnd_hdlr = this._title_node.connect("mousedown", this, "_startDnd");
    },

    _startDnd: function _startDnd(evt) {
        evt.cancel();
        this._releaseDnd(); // Just in case
        this._dnd_initial_x = evt.clientX;
        this._dnd_initial_y = evt.clientY;
        this._node_initial_x = this._node.offsetLeft;
        this._node_initial_y = this._node.offsetTop;
        this._dnd_move_hdlr = py.body.connect('mousemove', this, "_moveDnd");
        py._initDocument(this._background_node.contentWindow.document); // TODO move this in $c ??
        this._dnd_bg_move_hdlr = this._background_node.contentWindow.document.connect('mousemove', this, '_moveDnd');
        if (py.isNone(this._dnd_release_hdlr)) {
            this._dnd_release_hdlr = py.body.connect('mouseup', this, "_releaseDnd");
        }
    },

    _moveDnd: function _moveDnd(evt) {
        evt.cancel();
        var dx = evt.clientX - this._dnd_initial_x;
        var dy = evt.clientY - this._dnd_initial_y;
        this._node.setStyles({
            'left': this._node_initial_x + dx + 'px',
            'top': this._node_initial_y + dy + 'px'
        });
    },

    _releaseDnd: function() {
        if (py.notNone(this._dnd_move_hdlr)) {
            this._dnd_move_hdlr.disconnect();
        }
        if (py.notNone(this._dnd_bg_move_hdlr)) {
            this._dnd_bg_move_hdlr.disconnect();
        }
        this._dnd_bg_move_hdlr = null;
        this._dnd_move_hdlr = null;
    },

    _createBackground: function() {
        this._background_node = $c('iframe');
        this._background_node.setStyles({
            'display': 'none',
            'border': 'none',
            'position': 'absolute',
            'z-index': '99',
            'width': '100%',
            'height': '100%',
            'top': '0px',
            'left': '0px',
            'background-color': 'black',
            'opacity': '0.5'
        });
        py.body.appendChild(this._background_node);
    },

    _refresh: function _refresh() {
        if (this._is_shown == false) {
            return ;
        }

        this._node.setStyles({
            'top': '-9999px',
            'left': '-9999px',
            'width': '',
            'height': ''
        });
        var vp = py.browser.getViewport(),
            max_width = vp.w * this._max_width,
            max_height = vp.h * this._max_height;
        if (this._node.offsetHeight > max_height) {
            this._node.setStyle('height', max_height + 'px');
            max_width += 18;
            this._node.setStyle('width', this._node.offsetWidth + 18 + 'px')
        }
        if (this._node.offsetWidth > max_width) {
            this._node.setStyle('width', max_width + 'px');
        }
        var l = vp.w / 2 - this._node.offsetWidth / 2,
            t = vp.h / 2 - this._node.offsetHeight / 2;
        this._node.setStyles({
            'top': vp.t + t + 'px',
            'left': vp.l + l + 'px'
        });
    },

    _onCancel: function _onCancel() {
        this.hide();
        if (py.notNone(this._args.onClose)) {
            this._args.onClose.call(this);
        }
    },

    /**
     * show the popup
     */
    show: function show() {
        if (this._is_shown === true) {
            return ;
        }
        this._is_shown = true;
        this._background_node.setStyle('display', '');
        this._refresh();
    },

    /**
     * hide the popup
     */
    hide: function hide() {
        if (this._is_shown === false) {
            return ;
        }
        this._is_shown = false;
        this._node.setStyles({
            'top': '-9999px',
            'left': '-99990px'
        });
        this._background_node.setStyle('display', 'none');
    },

    /**
     * destroy the popup
     */
    destroy: function destroy() {
        this.hide();
        var hdlrs = [
            '_close_hdlr',
            '_start_dnd_hdlr',
            '_dnd_move_hdlr',
            '_dnd_bg_move_hdlr',
            '_dnd_release_hdlr'
        ];
        hdlrs.iter(function (hdlr) {
            if (py.notNone(this[hdlr])) {
                this[hdlr].disconnect();
            }
        }, this);
        py.unsubscribe(this._resize_hdlr);
        py.unsubscribe(this._scroll_hdlr);
        py.dom.destroyElement(this._node);
        py.dom.destroyElement(this._background_node);
    }

});

