/**
 * @fileOverview Dom namespace defined here
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
*/

/**
 * Dom utilities
 * @namespace
 * @name py.dom
 */

py.extendNamespace('py.dom', {
    /** @lends py.dom */

    /**
     * Get element by ID
     * @param {String|Element} id The node id
     * @return {Element} node found or Null
     */
    byId: function byId(id) {
        /*<debug*/py.raiseNone(id);/*debug>*/
        return document.getElementById(id) || null;
    },

    /**
     * Create an Element
     * @param {String} tag Tag name
     * @param {Object|String} [obj] attributes of the node, or a string for content of the node
     * @returns {Element} created element
     */
    create: function create(tag, attrs) {
        /*<debug*/
        py.raiseNone(tag);
        if (!py.isinstance(tag, String)) {
            throw TypeError("tag must be a string");
        }
        /*debug>*/
        var el = document.createElement(tag);
        if (attrs) {
            if (py.isinstance(attrs, String)) {
                attrs = {content: attrs};
            }
            //<debug
            if (!py.isinstance(attrs, Object)) {
                throw TypeError("attributes must be given in an Object");
            }
            //debug>
            attrs.iteritems(el.attr.bind(el));
        }
        return el;
    },

    /**
     * Destroy a node (remove it completely from the DOM)
     * @param {Element|String} node The node or its ID
     */
    destroyElement: function destroyElement(node) {
        /*<debug*/py.raiseNone(node);/*debug>*/
        node = py.isinstance(node, String) ? py.dom.byId(node) : node;
        //<debug
        if (py.isNone(node) || !py.isinstance(node, Element)) {
            throw TypeError('node must be a node or a node ID');
        }
        //debug>
        node.parentNode.removeChild(node);
    },

    insertAfter: function (node, ref) {
        ref = py.isinstance(ref, String) ? py.dom.byId(ref) : ref;
        //<debug
        py.raiseNone(node);
        py.raiseNone(ref);
        //debug>

        var parent = ref.parentNode;
        if (parent.lastChild == ref) {
            parent.appendChild(node);
        } else {
            parent.insertBefore(node, ref.nextSibling);
        }
    },

    insertBefore: function (node, ref) {
        ref = py.isinstance(ref, String) ? py.dom.byId(ref) : ref;
        //<debug
        py.raiseNone(node);
        py.raiseNone(ref);
        //debug>
        ref.parentNode.insertBefore(node, ref);
    },

    /**
     * Set or get attribute(s) of an elements
     * @param {String|Element} el the node or node id
     * @param {Object|Array|String} attr If it's an object, it will set all pairs to
     *                              the node. If it's an array, it will return an
     *                              object of attributes values. if it's a string,
     *                              it will return the value or set the new one if
     *                              last param val is specified
     * @param [val] used when set one attribute
     */
    attr: function attr(el, _attr, val) {
        //<debug
        py.raiseNone(el);
        py.raiseNone(_attr);
        //debug>

        var node = py.isinstance(el, String) ? py.dom.byId(el) : el;
        //<debug
        if (py.isinstance(el, String) && py.isNone(node)) {
            throw new ValueError('The ID '+el+' is not found');
        } else if (!py.isinstance(el, Element, String) ) {
            throw new ValueError('el must be a string or an Element');
        }
        //debug>

        if (py.isinstance(_attr, String)) {
            if (py.isNone(val)) { // get simple attribute
                return node.__getitem__(_attr);
            } else {
                return node.__setitem__(_attr, val);
            }
        } else if (py.isinstance(_attr, Array)) { // get multiple attributes
            var res = {};
            _attr.iter(function(k) {
                res[k] = node.__getitem__(k);
            });
            return res;
        } else if (py.isinstance(_attr, Object)) { // set multiple attributes
            _attr.iteritems(function(k,v) {
                node.__setitem__(k, v);
            });
        } /*<debug*/ else {
            throw TypeError('_attr must a String, an Array or an Object');
        } /*debug>*/
    },


    /**
     * Set or get style(s) from an element
     * @param {String|Element} el The node
     * @param {Object|Array|String} if it's an object, it will set all pairs to
     *                              the node style. If it's an array, it will return an
     *                              object of styles values. if it's a string,
     *                              it will return the value or set the new one if
     *                              last param val is specified
     * @param [val] used when set one style
     */
    style: function style(el, _style, val) {
        //<debug
        py.raiseNone(el);
        py.raiseNone(_style);
        //debug>

        var node = py.isinstance(el, String) ? document.getElementById(el) : el;

        //<debug
        if (py.isinstance(el, String) && py.isNone(node)) {
            throw new ValueError('The ID '+el+' is not found');
        } else if (!py.isinstance(el, Element, String) ) {
            throw new ValueError('el must be a String or an Element');
        }
        //debug>

        if (py.isinstance(_style, String)) {
            if (py.isNone(val)) { // get simple attribute
                return node.getStyle(_style);
            } else {
                return node.setStyle(_style, val);
            }
        } else if (py.isinstance(_style, Array)) { // get multiple attributes
            return node.getStyles(_style);
        } else if (py.isinstance(_style, Object)) { // set multiple attributes
            return node.setStyles(_style);
        } else {
            throw TypeError('_style must be a String, an Array or an Object');
        }
    },

    /**
     * Get or set size of a node
     * @param {String|Element} el The node
     * @param {String} [method='outer'] The type of size, can be 'inner' or 'outer'
     * @param {Object} [set_values] Use this method as a setter, give sizes as an object
     */
    layout: function layout(el, method, set_values) {
        /*<debug*/py.raiseNone(el);/*debug>*/
        var node = py.isinstance(el, String) ? document.getElementById(el) : el,
            size_method;
        if (py.notNone(method) && py.isinstance(method, String)) {
            //<debug
            if (method !== 'inner' && method !== 'outer') {
                throw new ValueError("Method must be either 'inner' or 'outer'");
            }
            //debug>
            size_method = method;
        } else if (py.isNone(method)) {
            size_method = 'outer';
        } else if (py.isinstance(method, Object) && py.isNone(set_values)) {
            size_method = 'outer';
            set_values = method;
        } /*<debug*/ else if (py.notNone(method)) {
            throw new TypeError("Wrong type for method");
        } /*debug>*/
        size_method = size_method[0].toUpperCase() + size_method.slice(1);
        //<debug
        if (py.notNone(set_values) && !py.isinstance(set_values, Object)) {
            throw new TypeError("When use as a setter, give values as an Object");
        }
        if (py.isinstance(el, String) && py.isNone(node)) {
            throw new ValueError('The ID '+el+' was not found');
        } else if (!py.isinstance(el, Element, String)) {
            throw new ValueError('el must be a String or an Element');
        }
        //debug>
        var computed_style = node.getComputedStyle();
        if (py.isNone(set_values)) {
            return (py.dom['get' + size_method + 'Layout'](node, computed_style));
        } else {
            py.dom['set' + size_method + 'Layout'](node, set_values, computed_style);
        }
    },


    /**
     * return real position on the screen
     * @param {Element} el
     * @private
     * @see http://www.quirksmode.org/js/findpos.html
     * @returns {Int[]} left and top offset
     */
    _getOffsets: function _getOffsets(el) {
        var l = 0,
            t = 0;
        if (el.offsetParent) {
            do {
                l += el.offsetLeft;
                t += el.offsetTop;
            } while (el = el.offsetParent);
        }
        return ([l, t]);
    },

    _getPadding: function _getPadding(el, cs) {
        cs = py.isNone(cs) ? el.getComputedStyle() : cs;
        return ({
            l: cs.paddingLeft ? parseInt(cs.paddingLeft, 10) : 0,
            r: cs.paddingRight ? parseInt(cs.paddingRight, 10) : 0,
            t: cs.paddingTop ? parseInt(cs.paddingTop, 10) : 0,
            b: cs.paddingBottom ? parseInt(cs.paddingBottom, 10) : 0
        });
    },

    _getMargin: function _getMargin(el, cs) {
        cs = py.isNone(cs) ? el.getComputedStyle() : cs;
        return ({
            l: cs.marginLeft ? parseInt(cs.marginLeft, 10) : 0,
            r: cs.marginRight ? parseInt(cs.marginRight, 10) : 0,
            t: cs.marginTop ? parseInt(cs.marginTop, 10) : 0,
            b: cs.marginBottom ? parseInt(cs.marginBottom, 10) : 0
        });
    },

    _getBorder: function _getBorder(el, cs) {
        cs = py.isNone(cs) ? el.getComputedStyle() : cs;
        return ({
            l: cs.borderLeftWidth ? parseInt(cs.borderLeftWidth, 10) : 0,
            r: cs.borderRightWidth ? parseInt(cs.borderRightWidth, 10) : 0,
            t: cs.borderTopWidth ? parseInt(cs.borderTopWidth, 10) : 0,
            b: cs.borderBottomWidth ? parseInt(cs.borderBottomWidth, 10) : 0
        });
    },

    /**
     * Get box layout including padding, borders, and margins
     * @param {Element} element
     * @param {Object} [cs] computed style
     */
    getOuterLayout: function getOuterLayout(el, cs) {
        //<debug
        if (py.isNone(el) || !py.isinstance(el, Element)) {
            throw TypeError("el must be an Element");
        }
        //debug>
        if (py.isNone(cs)) {
            cs = el.getComputedStyle();
        }
        var w = el.offsetWidth,
            h = el.offsetHeight,
            offsets = py.dom._getOffsets(el),
            l = offsets[0],
            t = offsets[1],
            margin = this._getMargin(el, cs);

        return ({
            w: w + margin.l + margin.r,
            h: h + margin.t + margin.b,
            l: l,
            t: t
        });
    },

    /**
     * Get box layout (inner size without padding and borders)
     * @param {Element} element
     * @param {Object} [cs] computed style
     */
    getInnerLayout: function getInnerLayout(el, cs) {
        //<debug
        if (py.isNone(el) || !py.isinstance(el, Element)) {
            throw TypeError("el must be an Element");
        }
        //debug>
        if (py.isNone(cs)) {
            cs = el.getComputedStyle();
        }
        var w = el.clientWidth,
            h = el.clientHeight,
            offsets = py.dom._getOffsets(el),
            l = offsets[0],
            t = offsets[1],
            padding = this._getPadding(el, cs),
            margin = this._getMargin(el, cs),
            border = this._getBorder(el, cs);

        return ({
            w: w - padding.l - padding.r,
            h: h - padding.t - padding.b,
            l: l + padding.l + margin.l + border.l,
            t: t + padding.t + margin.t + border.t
        });
    },


    /**
     * Set box layout including padding, borders and margin
     * @param {Element} element
     * @param {Object} values
     * @param {Integer} [values.w] width
     * @param {Integer} [values.h] height
     * @param {Integer} [values.l] left position relative to screen
     * @param {Integer} [values.t] top position relative to screen
     * @param {Object} [cs] Computed styles
     */
    setOuterLayout: function setOuterLayout(el, values, cs) {
        //<debug
        if (py.isNone(el) || !py.isinstance(el, Element)) {
            throw TypeError("el must be an Element");
        }
        if (py.isNone(values) || !py.isinstance(values, Object)) {
            throw TypeError("values must be an Object");
        }
        //debug>
        el.style.position = '';
        el.style.position = 'absolute'; // really ? (bug with margin calculation on chrome)
        var margin = this._getMargin(el, cs),
            border = this._getBorder(el, cs),
            padding = this._getPadding(el, cs);
        if (py.notNone(values.w)) {
            var w = values.w - margin.l - margin.r - padding.l - padding.r - border.l - border.r; //TODO IE box model ?
            el.style.width = (w > 0 ? w : 0) + 'px';
        }
        if (py.notNone(values.h)) {
            var h = values.h - margin.t - margin.b - padding.t - padding.b - border.t - border.b; //TODO IE box model ?
            el.style.height = (h > 0 ? h : 0) + 'px';
        }

        var offsets = null;
        if (py.isNone(values.l)) {
            offsets = this._getOffsets(el);
            values.l = offsets[0];
        }
        if (py.isNone(values.t)) {
            if (py.isNone(offsets))
                offsets = this._getOffsets(el);
            values.t = offsets[1];
        }
        el.style['left'] = values.l + 'px';
        el.style['top'] = values.t + 'px';

        offsets = this._getOffsets(el);
        if (values.l - offsets[0] != 0)
        {
            el.style['left'] = (values.l - (offsets[0] - values.l)) + 'px';
        }
        if (values.t - offsets[1] != 0)
        {
            el.style['top'] = (values.t - (offsets[1] - values.t)) + 'px';
        }
    },

    /**
     * Set box layout excluding padding, borders and margin
     * @param {Element} element
     * @param {Object} values
     * @param {Integer} [values.w] width
     * @param {Integer} [values.h] height
     * @param {Integer} [values.l] left position relative to screen
     * @param {Integer} [values.t] top position relative to screen
     * @param {Object} [cs] Computed styles
     */
    setInnerLayout: function setInnerLayout(el, values, cs) {
        //<debug
        if (py.isNone(el) || !py.isinstance(el, Element)) {
            throw TypeError("el must be an Element");
        }
        if (py.isNone(values) || !py.isinstance(values, Object)) {
            throw TypeError("values must be an Object");
        }
        //debug>
        var margin = this._getMargin(el, cs),
            border = this._getBorder(el, cs),
            padding = this._getPadding(el, cs);
        if (py.notNone(values.w)) {
            //TODO IE box model ?
            el.style.width = (values.w > 0 ? values.w : 0) + 'px';
        }
        if (py.notNone(values.h)) {
            //TODO IE box model ?
            el.style.height = (values.h > 0 ? values.h : 0) + 'px';
        }
        if (py.notNone(values.t) || py.notNone(values.l)) {
            var offsets = this._getOffsets(el);
            el.style.position = 'absolute'; // really ?
            var l, t;
            if (py.notNone(values.l)) {
                l = values.l - margin.l - border.l - padding.l;
            } else {
                l = offsets[0];
            }
            if (py.notNone(values.t)) {
                t = values.t - margin.t - border.t - padding.t;
            } else {
                t = offsets[1];
            }
            el.style['left'] = l + 'px';
            el.style['top'] = t + 'px';
        }
    }



});

py._importModule('py.core.dom-query');
py._importModule('py.core.document');
