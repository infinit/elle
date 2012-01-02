/**
 * @fileOverview This is String prototype augmentation
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */

/**
 * The Element prototype
 * @name Element
 * @class
 */


// For IE
if (typeof Element === "undefined") {
    Element = function(){};
    py._importModule('py.prototype.browsers.ie');
}

/**
 * @property {String} __name__ The object class
 * @private
 */
Element.prototype.__name__ = "Element";
Element.prototype.__class__ = Element;

/**
 * Check in an element is child of another node
 * @param {Element|String} n a node or id
 * @returns {Boolean}
 */
Element.prototype.isIn = function isIn(/*Node|String*/ n) {
    /*<debug*/py.raiseNone(n);/*debug>*/
    if (py.isinstance(n, String)) {
        n = document.getElementById(n);
    }
    if (n === null) {return false;}
    var child = this;
    while (child) {
        if (child === n) {return true;}
        child = child.parentNode;
    }
    return false;
};

/**
 * Check if an element is parent of another node
 * @param {Element|String} n node or id
 * @returns {Boolean}
 */
Element.prototype.contains = function contains(/*Node|String*/ n) {
    if (py.isinstance(n, String)) {
        n = document.getElementById(n);
    }
    if (n === null) {return false;}
    return n.isIn(this);
};

/**
 * Get node attribute
 * @param {String} k attribute name
 * @returns attribute value
 * @private
 */
Element.prototype.__getitem__ = function(k) {
    var attr_map = py.browser.getAttributesMap(),
        getter = function(k) {
            //<debug
            if (py.isNone(k) || !py.isinstance(k, String)) {
                throw new TypeError('Node attribute must be a string');
            }
            //debug>
            k = attr_map[k] || k;
            if (py.isinstance(k, Function)) {
                return k.call(this);
            }
            return py.notNone(this[k]) ? this[k] : this.getAttribute(k);
    };
    setTimeout(function(){
        Element.prototype.__getitem__ = getter;
    }, 0);
    return getter.call(this, k);
};

/**
 * Set node attribute
 * @param {String} k attribute name
 * @param v attribute value
 * @private
 */
Element.prototype.__setitem__ = function(k, v) {
    var attr_map = py.browser.getAttributesMap(),
        setter = function(k, v) {
            //<debug
            if (py.isNone(k) || !py.isinstance(k, String)) {
                throw new TypeError('Node attribute must be a string');
            }
            //debug>
            k = attr_map[k.toLowerCase()] || k;
            if (py.isinstance(k, Function)) {
                return k.call(this, v);
            }
            this[k] = v;
        };
    setTimeout(function(){
        Element.prototype.__setitem__ = setter;
    }, 0);
    return setter.call(this, k, v);
};

/**
 * Get or set node attribute
 * @param {String} attribute attribute name
 * @param [value] attribute value
 * @returns attribute value or undefined
 */
Element.prototype.attr = function(/*String*/ attribute, /*String?*/ value) {
    if (py.isNone(value)) {return this.__getitem__(attribute);}
    else {this.__setitem__(attribute, value);}
};


// TODO add to ie
/**
 * Return true if the element has the class
 * @param {String} str Class name to test
 * @returns {Boolean}
 */
Element.prototype.hasClass = function(str) {
    var tab = this.className.toString().split(' ');
    return tab.contains(str);
};

/**
 * Add a class to an element
 * @param {String} str Class name to add
 */
Element.prototype.addClass = function(str) {
    if (!this.hasClass(str)) {
        this.className += ' ' + str;
    }
};

Element.prototype.removeClass = function(str) {
    var old = this.className.toString().split(' '),
        res = old.filter(function(cls) { return (str !== cls); });
    this.className = ' '.join(res);
};

/**
 * Select an array of nodes with CSS2 selector inside the element
 * @param {String} selectors CSS2 selectors (separated by comma)
 * @returns {Element[]} Array of nodes (possibly empty)
 */
Element.prototype.query = function query(selectors) {
    return py.dom.query(selectors, this);
};


/**
 * Fire an arbitrary event
 * @param {String} name The event name, NOT starting with 'on'
 * @param {Object} [properties] Properties of the event
 */
Element.prototype.fire = function fire(name, properties) {
    var evt;
    if (document.createEvent) {
        evt = document.createEvent("HTMLEvents");
        evt.initEvent(name, true, true);
    } else {
        evt = document.createEventObject();
        evt.eventType = "on" + name;
    }
    evt.eventName = name;
    if (properties) {
        properties.iteritems(function (k, v) {
            evt[k] = v;
        });
    }

    if (document.createEvent) {
        this.dispatchEvent(evt);
    } else {
        this.fireEvent(evt.eventType, evt);
    }
};

/**
 * connect a function to an element's event
 * @param {String} event_name The event name, NOT starting with 'on'
 * @param {Object|Function} scope Scope object or function to execute
 * @param {Function|String} [func] the function to execute
 * @returns {py.event.Handler} Handler, needed to disconnect
 */
Element.prototype.connect = function connect(str, scope, func) {
    //<debug
    py.raiseNone(str);
    if (!py.isinstance(str, String)) {
        throw new TypeError("The event must be a String");
    }
    if (str.startswith('on')) {
        throw new ValueError("The event must NOT starts with 'on'");
    }
    //debug>
    if (py.notNone(func)) {
        if (py.isinstance(func, String)) {
            //<debug
            if (py.isNone(scope)) {
                throw new ValueError("scope cannot be None when function is a String");
            }
            //debug>
            func = scope[func];
        }
        func = func.bind(scope);
    } else {
        func = scope;
        scope = null;
    }
    //<debug
    if (!py.isinstance(func, Function)) {
        throw new ValueError("given callback is not a function");
    }
    //debug>
    var self = this;
    function buildFirerer(str, hdlrs) {
        return (function (e) {
            var evt = e || window.event;
            var ret_val = true;
            evt.cancel = function() {
                ret_val = false;
                try {
                    this.returnValue = false;
                    if (py.notNone(this.preventDefault)) {
                        this.preventDefault();
                    }
                    if (py.notNone(this.cancelBubble)) {
                        this.cancelBubble = true;
                    }
                }catch(e) {
                    warn('cannot cancel event');
                }
                var self = this;
                setTimeout(function() {self.cancel = py.nothing;}, 0);
            };
            hdlrs.iter(function (hdlr) {
                /*<debug*/try {/*debug>*/
                    hdlr.fire([evt]);
                //<debug
                } catch(err) {
                    warn("Handler", hdlr, "of", self, ": '", str, "' failed:", err);
                }
                //debug>
            });
            return (ret_val);
        });
    }

    if (py.isNone(this._hdlrs)) {
        this._hdlrs = {};
        this._fire_hdlrs = {};
    }
    if (py.isNone(this._hdlrs[str])) {
        this._hdlrs[str] = [];
        this._fire_hdlrs[str] = buildFirerer(str, this._hdlrs[str]);
        if (this.addEventListener) {
            this.addEventListener(str, this._fire_hdlrs[str], false);
        } else if (this.attachEvent) {
            this.attachEvent('on' + str, this._fire_hdlrs[str]);
        }/*<debug*/ else {
            throw new Error("Cannot attach event !");
        }/*debug>*/
    }
    var hdlr = new py.event.Handler(str, self, func);
    this._hdlrs[str].append(hdlr);
    return (hdlr);
};

Element.prototype.disconnect = function disconnect(hdlr) {
    //<debug
    if (py.isNone(hdlr) || !py.isinstance(hdlr, py.event.Handler)) {
        throw new TypeError("Handler must be the result of a previous connect");
    }
    if (py.isNone(this._hdlrs) || py.isNone(this._hdlrs[hdlr._event_name])) {
        throw new Error("Internal Error: Handler refer to an empty connector");
    }
    //debug>
    var idx = -1,
        str = hdlr._event_name,
        hdlrs = this._hdlrs[str];
    hdlrs.iter(function (h, i) {
        if (h._id === hdlr._id) {
            idx = i;
            throw new StopIteration();
        }
    });
    if (idx >= 0) {
        delete hdlrs[idx];
        hdlrs.splice(idx, 1);
    } /*<debug*/ else {
        warn('This handler does not exists anymore !');
        return ;
    } /*debug>*/
    if (py.len(hdlrs) === 0) {
        delete this._hdlrs[str];
        if (this.removeEventListener) {
            this.removeEventListener(str, this._fire_hdlrs[str], false);
        } else if (this.detachEvent) {
            this.detachEvent('on' + str, this._fire_hdlrs[str]);
        } /*<debug*/ else {
            throw new Error("Cannot detach event !");
        } /*debug>*/
        delete this._fire_hdlrs[str];
    }
};

/**
 * Set multiples styles
 * @param {Object} styles Styles to apply
 * @see Element.setStyle
 */
Element.prototype.setStyles = function setStyles(styles) {
    /*<debug*/
    if (py.isNone(styles) || !py.isinstance(styles, Object)) {
        throw new TypeError("styles must be an Object");
    }
    /*debug>*/

    // TODO understand why this one does not work with IE
    //styles.iteritems(this.setStyle.bind(this));

    styles.iteritems(function(k, v) {this.setStyle(k, v);}, this);
};

/** Get multiples styles
 * @param {String[]} styles styles to get
 * @see Element.getStyle
 */
Element.prototype.getStyles = function getStyles(styles) {
    /*<debug*/
    if (py.isNone(styles) || !py.isinstance(styles, Array)) {
        throw new TypeError("styles must be an Array");
    }
    /*debug>*/
    var res = {};
    styles.iter(function(s) { res[s] = this.getStyle(s); }, this);
    return res;
};


(function() {

    function camelize(str) {
        return str.split('-').map(function(s, idx) {
            if (idx === 0) { return s; }
            else { return s.charAt(0).toUpperCase() + s.slice(1); }
        }).join('');
    }
    function raiseCamelCase(s) {
        if (s !== s.toLowerCase()) {
            throw new ValueError('The String "'+s+'" must not be camel cased');
        }
    }

    /**
     * Set style
     * @param {String} property Style property to alter
     * @param {String|Number} value Property value
     */
    Element.prototype.setStyle = function setStyle(k, v) {
         var map = py.browser.getStylesMap(), mapping = null,
            setter = function(k, v) {
                //<debug
                if (py.isNone(k) || !py.isinstance(k, String)) {
                    throw new TypeError('Node Style must be a string');
                }
                if (py.isNone(v) || !py.isinstance(v, String, Number)) {
                    throw new TypeError('Style property must be a String or a Number');
                }
                raiseCamelCase(k);
                //debug>
                k = camelize(k);
                mapping = map[k] || k;
                if (py.notNone(mapping) && py.isinstance(mapping, Function)) {
                    return mapping.call(this, k, v);
                }
                this.style[mapping] = v;
            };
        setTimeout(function(){
            Element.prototype.setStyle = setter;
        }, 0);
        return setter.call(this, k, v);

    };

    if (typeof(Element.prototype.getComputedStyle) === "undefined") {
        /**
         * Get computed styles
         * @returns {Object} Computed styles
         */
        Element.prototype.getComputedStyle = function getComputedStyle() {
            var getter;
            if (this.currentStyle) {
                getter = function () {
                    return (this.currentStyle);
                };
            } else {
                var gs = py.doc.defaultView.getComputedStyle;
                getter = function () {
                    return (gs(this, null));
                };
            }
            setTimeout(function () {
                Element.prototype.getComputedStyle = getter;
            }, 0);
            return getter.call(this);
        };
    }

    /**
     * Get Computed style
     * @param {String} property to get (no camel case)
     * @returns {String} Css property
     */
    Element.prototype.getStyle = function getStyle(k) {
        var map = py.browser.getStylesMap(),
            mapping = null, //temp var
            getter /*<debug*/,
            checkKey = function checkKey(k) {
                if (py.isNone(k) || !py.isinstance(k, String)) {
                    throw new TypeError('Css property must be a string');
                }
                raiseCamelCase(k);
            }/*debug>*/;
        if (py.browser.name == "Explorer") {
            getter = function(k) { // IE use currentStyle
                /*<debug*/checkKey(k);/*debug>*/
                k = camelize(k);
                mapping = map[k];
                if (py.isNone(mapping)) {
                    return this.currentStyle[k];
                } else if (py.isinstance(mapping, Function)) {
                    return mapping.call(this, k);
                } else {
                    return this.currentStyle[mapping];
                }
            };
        } else {
            var gs = py.doc.defaultView.getComputedStyle;
            getter = function (k) { // others use document.defaultView.getComputedStyle
                /*<debug*/checkKey(k);/*debug>*/
                mapping = map[camelize(k)];
                if (py.isNone(mapping)) {
                    return gs(this, null).getPropertyValue(k);
                } else if (py.isinstance(mapping, Function)) {
                    return mapping.call(this, k);
                } else {
                    return gs(this, null).getPropertyValue(k);
                }
            };
        }
        setTimeout(function(){
            Element.prototype.getStyle = getter;
        }, 0);
        return getter.call(this, k);
    };
})();



