/**
 * @fileOverview Parser functionnality
 * @author <a href="mailto:raphael.londeix@gmail.com>Raphaël Londeix</a>
 * @version 0.1
 */

/**
 * parser utilities namespace
 * @namespace
 * @name py.parser
 */



/**
 * Handlers for parsers base class
 * @name py.parser.Handler
 * @class
 */
py.declare('py.parser.Handler', null, {
    /** @lends py.parser.Handler */

    match: function match(element, elements) {
        return false;
    },

    onMatch: function onMatch(element, elements) {

    }
});

/**
 * Generic parser class
 * @name py.parser.Parser
 * @class
 */
py.declare('py.parser.Parser', null, {
    /** @lends py.parser.Parser */
    _handlers: null,

    __init__: function() {
        this._handlers = [];
    },

    /**
     * Add an handler to the parser
     * @param {py.parser.Handler} hdlr Handler to add
     */
    addHandler: function addHandler(hdlr) {
        this._handlers.append(hdlr);
    },

    /**
     * Return next element to parse, must be overrided
     * @param element last parsed element (null for first)
     * @param elements all elements
     * @param {Int} idx the element index, starting with 0
     * @returns {Object|None} next element or null when parsing is done
     */
    getNext: function(element, elements, idx) {},

    /**
     * Entry point, should not be overrided
     * @params elements elements to parse
     */
    parse: function(elements) {
        var element = null,
            i = 0;
        if (py.len(this._handlers) === 0) {
            throw Error("Cannot parse anything no handlers are defined");
        }
        element = this.getNext(null, elements, 0);
        if (py.isNone(element)) { warn("stupid parsing ?"); }
        var check_hdlrs = function(hdlr) {
            if (hdlr.match(element, elements)) {
                hdlr.onMatch(element, elements);
            }
        };
        while (py.notNone(element)) {
            this._handlers.iter(check_hdlrs);
            i += 1;
            element = this.getNext(element, elements, i);
        }
    }
});
