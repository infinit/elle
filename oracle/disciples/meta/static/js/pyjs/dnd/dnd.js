/**
 * @fileOverview utils function for drag & drop support
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */

/*global Element, clearTimeout, setTimeout*/


py.declare('py.dnd.DndNode', null, {
    _node: null,
    _dnd_start_hdlr: null,
    _dnd_release_hdlr: null,
    _dnd_move_hdlr: null,
    _args: null,
    _delay: 0,

    /** Initial mouse x position */
    mouse_startx: 0,

    /** Initial mouse y position */
    mouse_starty: 0,

    /** Initial node x position */
    node_startx: 0,

    /** Initial node y position */
    node_starty: 0,

    /** Current x delta */
    deltax: 0,

    /** Current y delta */
    deltay: 0,


    /**
     * Base drag&drop class
     * @param {Element|String} node Node or node id to drag
     * @param {Object} args Constructs arguments
     * @param {Function} [args.onStart] Callback for starting drag
     * @param {Function} [args.onMove] Callback for moving the node
     * @param {Function} [args.onStop] Callback for dropping
     * @param {Boolean} [args.dont_start=false] Do not start d&d when creation
     * @param {Integer} [args.delay] Let a delay occur before dnd (eg. a click is also waited)
     * @constructs
     */
    __init__: function __init__(node, args) {
        //<debug
        if (py.isNone(node) || !py.isinstance(node, Element, String)) {
            throw new TypeError("node element must be either an Element or a String");
        }
        //debug>
        if (py.isinstance(node, String)) {
            node = $(node);
        }
        //<debug
        py.raiseNone(node);
        //debug>
        this._node = node;
        this._args = args || {};
        if (!this._args.dont_start) {
            this.start();
        }
        if (py.notNone(this._args.delay)) {
            this._delay = this._args.delay;
        }
    },

    start: function start() {
        if (py.notNone(this._dnd_start_hdlr)) {
            //<debug
            warn('Starting Dnd more than once for', this._node);
            //debug>
            this.stop();
        }
        this._dnd_start_hdlr = this._node.connect("mousedown", this, "_startDnd");
    },

    _startDnd: function _startDnd(evt) {
        var startx = evt.clientX,
            starty = evt.clientY;
        var startDnd = (function() {
            evt.cancel();
            this._releaseDnd(); // Just in case

            this.mouse_startx = startx;
            this.mouse_starty = starty;
            this.node_startx = this._node.offsetLeft;
            this.node_starty = this._node.offsetTop;
            this.deltax = 0;
            this.deltay = 0;
            this._dnd_move_hdlr = py.body.connect('mousemove', this, "_moveDnd");
            if (py.isNone(this._dnd_release_hdlr)) {
                this._dnd_release_hdlr = py.body.connect('mouseup', this, "_releaseDnd");
            }
            if (py.notNone(this._args.onStart)) {
                this._args.onStart(this);
            }
        }).bind(this);
        if (this._delay > 0) {
            var delayed_start = null;
            var ignore_dnd_hdlr = py.body.connect('mouseup', function() {
                clearTimeout(delayed_start);
                ignore_dnd_hdlr.disconnect();
            });
            delayed_start = setTimeout(function() {
                ignore_dnd_hdlr.disconnect();
                startDnd();
            }, this._delay);
        } else {
            startDnd();
        }
    },

    _moveDnd: function _moveDnd(evt) {
        evt.cancel();
        this.deltax = evt.clientX - this.mouse_startx;
        this.deltay = evt.clientY - this.mouse_starty;
//        this._node.setStyles({
//            'left': this._node_initial_x + dx + 'px',
//            'top': this._node_initial_y + dy + 'px'
//        });
        if (py.notNone(this._args.onMove)) {
            this._args.onMove(this);
        }
    },

    _releaseDnd: function() {
        if (py.notNone(this._dnd_move_hdlr)) {
            this._dnd_move_hdlr.disconnect();
            if (py.notNone(this._args.onStop)) {
                this._args.onStop(this);
            }
        }
        this._dnd_move_hdlr = null;
    },

    /**
     * Disconnect all handlers
     */
    stop: function() {
        (['_dnd_start_hdlr', '_dnd_move_hdlr', '_dnd_release_hdlr']).iter(
            function (hdlr) {
                if (py.notNone(this[hdlr])) {
                    this[hdlr].disconnect();
                    this[hdlr] = null;
                }
            },
            this
        );
    }
});
