/**
 * @fileOverview This is Object prototype augmentation, it both implements
 *               general functions working for all object and specifics function
 *               only for Object prototype
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */

/**
 * The Object prototype
 * @name Object
 * @class
 */


/**
 * @property {String} __name__ The object class
 * @default "Object"
 * @private
 * @lends Object.prototype
 */
Object.prototype.__name__ = 'Object';
Object.prototype.__class__ = Object;

/**
 * Returns string representation of the object
 * @private
 * @returns {String}
 */
Object.prototype.__str__ = function() {
    if (py.notNone(this.__name__)) {
        return '<Class ' + this.__name__+' ('+this.__repr__()+')>';
    } else {
        return '<Class `Unknown` ('+this.__repr__()+')>';
    }
};

/**
 * Returns Json representaion of the object
 * @private
 * @returns {String}
 */
Object.prototype.__repr__ = function() {
    var pairs = [];
    this.iteritems(function(k, v, idx) {
        pairs[idx] = py.repr(k) + ': ' + py.repr(v);
    });
    return "{" + (', ').join(pairs) + "}";
};


/**
 * Specific implementation for Object. It returns an `Iterable` Object
 * For Object, it returns an Arrays of keys
 * @private
 * @returns {Array}
 * @see <a href="http://docs.python.org/library/stdtypes.html#iterator-types">Python Iterators</a>
 */
Object.prototype.__iter__ = function() {
    var keys = [], k;
    for (k in this) {
        if (this.hasOwnProperty(k)) {
            keys.append(k);
        }
    }
    return keys.__iter__();
};


/**
 * Returns the number of contained objects
 * To be available, class must have an __iter__ member
 * @private
 * @returns {Number}
 */
Object.prototype.__len__ = function() {
    return this.__iter__().__len__();
};


/**
 * Returns the value for given key
 * @param {String} k Key
 * @private
 * @returns The value contained
 */
Object.prototype.__getitem__ = function(k) {
    var v = this[k];
    if (v === undefined) {
        throw new KeyError(k);
    }
    return v;
};


/**
 * Set the value for given key
 * @param {String} k Key
 * @param v Value
 * @private
 */
Object.prototype.__setitem__ = function(k, v) {
    //<debug
    if (k === undefined) {
        throw new KeyError("key is undefined");
    }
    if (v === undefined) {
        throw new ValueError("value is undefined, use delete instead");
    }
    //debug>
    this[k] = v;
};


/**
 * Iter trough the object and call a function in a given scope
 * Arguments passed are the key and the index
 * if the callback raise StopIteration error, it will stop the iteration ...
 * @function
 * @param {Function} f The callback
 * @param {Object} [scope] The scope
 * @public
 */
Object.prototype.iter = function iter(f, scope) {
    var it = this.__iter__(),
        idx = 0;
    if (py.isNone(f)) {
        return it;
    }
    //<debug
    if (!py.isinstance(f, Function)) {
        throw new TypeError("iter first argument must be a function");
    }
    //debug>
    while (true) {
        try {
            f.call(scope, it.next(), idx);
            idx += 1;
        } catch (err) {
            if (py.isinstance(err, StopIteration)) {
                break;
            }
            throw err;
        }
    }
};


/**
 * Specific to Object, same as Object.iter, but give to the callback
 * the key, the value and the index
 * @param {Function} f The callback
 * @param {Object} [scope] The scope
 * @see Object#iter
 */
Object.prototype.iteritems = function iteritems(f, scope) {
    if (py.isNone(f)) {
        return this.items();
    }
    //<debug
    if (!py.isinstance(f, Function)) {
        throw new TypeError('iteritems argument must be function !');
    }
    //debug>
    this.iter(function(k, idx) {
        f.call(scope, k, this[k], idx);
    }, this);
};

/**
 * Specific to Object, same as Object.iter, but give to the callback
 * the value and the index
 * @param {Function} f The callback
 * @param {Object} [scope] The scope
 * @see Object#iter
 */
Object.prototype.itervalues = function itervalues(f, scope) {
    if (py.isNone(f)) {
        return this.values();
    }
    //<debug
    if (!py.isinstance(f, Function)) {
        throw new Error('itervalues argument must be function !');
    }
    //debug>
    this.iter(function(k, idx) {
        f.call(scope, this[k], idx);
    }, this);
};


/**
 * Generic function to get keys as an Array
 * @returns {Array} keys
 */
Object.prototype.keys = function keys() {
    return this.iter();
};

/**
 * Object specific, get all keys and return them as an Array
 * @returns {Array} values
 */
Object.prototype.values = function values() {
    return this.iter().map(function(k) {return this[k];}, this);
};


/**
 * Object specific, returns keys and values as pairs in an Array
 * @returns {Arrays} pairs
 */
Object.prototype.items = function items() {
    return this.iter().map(function(k) {return [k, this[k]];}, this);
};

/**
 * Specific function to compare any objects
 * @param {Object} obj Object to compare with
 * @returns {Boolean}
 */
Object.prototype.equals = function equals(obj) {
    if (py.isNone(obj) || !py.isinstance(obj, Object)) {
        return false;
    }
    var same = true;
    this.iteritems(function(k, v) {
        if (typeof obj[k] === "undefined") {
            same = false;
        } else {
            same = v.equals(obj[k]);
        }
        if (same === false) {
            throw new StopIteration();
        }
    });
    return same;
};

/**
 * General function to check if an object is contained
 * @param {Object} obj The container
 * @returns {Boolean}
 */
Object.prototype.isIn = function isIn(obj) {
    //<debug
    py.raiseNone(obj);
    //debug>
    var found = false;
    obj.iter(function(el) {
        if (this.equals(el)) {
            found = true;
            throw new StopIteration();
        }
    }, this);
    return found;
};


/**
 * Generic function to check if an object contains another
 * @param {Object} obj Contained object
 * @returns {Boolean}
 */
Object.prototype.contains = function contains(obj) {
    var found = false;
    this.iter(function(el) {
        found = py.equal(el, obj);
        if (found) {
            throw new StopIteration();
        }
    }, this);
    return found;
};

/**
 * Generic function to check if any of contained object verify a condition
 * The given callback must returns a Boolean. It takes same arguments as
 * iter callback
 * @param {Function} fn callback that return Boolean
 * @param {Object} [scope] Scope to execute callback
 * @returns {Boolean}
 */
Object.prototype.any = function any(fn, scope) {
    //<debug
    if (!py.isinstance(fn, Function)) {
        throw new TypeError('argument must be function !');
    }
    //debug>
    var res = false;
    this.iter(function(el, idx) {
        res = !!fn.call(scope, el, idx);
        if (res === true) {
            throw new StopIteration();
        }
    });
    return res;
};

/**
 * Generic function to check if all contained objects verify a condition
 * The given callback must returns a Boolean. It takes same arguments as
 * iter callback.
 * @param {Function} fn callback that return Boolean
 * @param {Object} [scope] Scope to execute callback
 * @returns {Boolean}
 */
Object.prototype.all = function all(fn, scope) {
    //<debug
    if (!py.isinstance(fn, Function)) {
        throw new TypeError('argument must be function !');
    }
   //debug>
    var res = true;
    this.iter(function(el, idx) {
        res = !!fn.call(scope, el, idx);
        if (res === false) {
            throw new StopIteration();
        }
    });
    return res;
};

/**
 * Object specific. It augments the object.
 * @param {Object} obj the source object
 */
Object.prototype.update = function update(/*Object*/ obj) {
    //<debug
    if (!py.isinstance(obj, Object)) {
        throw new TypeError('obj must be an Object');
    }
    //debug>
    obj.iteritems(function(k, v) {
        this[k] = v;
    }, this);
};

