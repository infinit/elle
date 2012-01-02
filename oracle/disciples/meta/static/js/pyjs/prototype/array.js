/**
 * @fileOverview This is Array prototype augmentation, it implements specifics
 *               function for Array prototype
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */

/**
 * The Array prototype
 * @name Array
 * @class
 */

/**
 * @property {String} __name__ The object class
 * @private
 */
Array.prototype.__name__ = 'Array';
Array.prototype.__class__ = Array;

/**
 * Returns the number of contained objects
 * @p(rivate
 * @returns {Number}
 */
Array.prototype.__len__ = function() {
    return this.length;
};

/**
 * Returns Json representaion of the object
 * @private
 * @returns {String}
 */
Array.prototype.__repr__ = function() {
    return '[' + (', ').join(this.map(function(k){return py.repr(k);})) + ']';
};

/**
 * Append value at the end of the Array
 * @param {Object} value The value to append
 * @returns {Number} Array length
 * @function
 */
Array.prototype.append = function() {
    var _push = Array.prototype.push;
    if (py.config.extremist) {
        Array.prototype.push = null;
    }

    return _push ?
    function append() {
        return _push.apply(this, arguments);
    } :
    function append() {
        var args = arguments, l = args.length, j = this.length, i;
        for (i = 0; i < l; i++) {
            this[j] = args[i];
            j++;
        }
        return j;
    };
}();


Array.prototype.first = function() {
    return this[0];
};

Array.prototype.last = function() {
    return this.slice(-1)[0];
};

/**
 * Specific implementation for Array. It returns an `Iterable` Object
 * @private
 * @returns {Array}
 * @see <a href="http://docs.python.org/library/stdtypes.html#iterator-types">Python Iterators</a>
 */
Array.prototype.__iter__ = function() {
    this._index = 0;
    this._len = this.__len__();
    return this;
};

/**
 * Return the next value or raise StopIteration
 * @returns {Object} the next value
 * @private
 * @see <a href="http://docs.python.org/library/stdtypes.html#iterator-types">Python Iterators</a>
 */
Array.prototype.next = function next() {
    if (this._index < this._len) {
        var val = this[this._index];
        this._index += 1;
        return val;
    }
    throw new StopIteration();
};

/**
 * Specific implementation for arrays
 * @returns {Boolean}
 * @see Object#equals
 */
Array.prototype.equals = function equals(a) {
    if (py.isNone(a) || a.__name__ !== 'Array' || this.__len__() !== a.__len__()) {
        return false;
    }
    var same = true;
    this.iter(function(i, idx) {
        same = py.equal(i, a[idx]);
        if (!same) {
            throw new StopIteration();
        }
    });
    return same;
};

/**
 * Append an array at the end
 * @param {Array} a array to add
 * @returns {Number} new array length
 */
Array.prototype.extend = function extend(a) {
    return this.append.apply(this, a); //convert Array to Arguments
};

Array.prototype.all = function() {
    var _every = Array.prototype.every;
    if (py.config.extremist) {
        Array.prototype.every = null;
    }
    return _every ? function all(fn, scope) {
        /*<debug*/py.raiseNone(fn);/*debug>*/
        return _every.call(this, fn, scope);
    } : Object.prototype.all;
}();

Array.prototype.any = function() {
    var _some = Array.prototype.some;
    if (py.config.extremist) {
        Array.prototype.some = null;
    }
    return _some ? function any(fn, scope) {
        /*<debug*/py.raiseNone(fn);/*debug>*/
        return _some.call(this, fn, scope);
    } : Object.prototype.any;
}();


// WARNING: builtin map function does not handle StopIteration
if (!Array.prototype.map) {
    /**
     * Apply a function to all items and returns an array of results
     * @param {Function} fn callback
     * @param {Object} [scope] scope of the callback
     * @returns {Array} array of results
     */
    Array.prototype.map = function map(fn, scope) {
        /*<debug*/py.raiseNone(fn);/*debug>*/
        var res = new Array(this.length);
        this.iter(function(item, idx) {
            res[idx] = fn.call(scope, item, idx);
        });
        return res;
    };
}

// WARNING: builtin filter function does not handle StopIteration
if (!Array.prototype.filter) {
    /**
     * Apply a function to each item in the array and return a new one with
     * items that pass the test
     * @param {Function} fn callback
     * @param {Object} [scope] scope of the callback
     * @returns {Array} array of validated items
     */
    Array.prototype.filter = function filter(fn, scope) {
        /*<debug*/py.raiseNone(fn);/*debug>*/
        var res = [];
        this.iter(function(item, idx) {
            if (fn.call(scope, item, idx)) {
                res.push(item);
            }
        });
        return res;
    };
}


/**
 * Sort and return a new array
 * @param {Function} [cmp] Comparator function (default is >)
 * @returns {Array} sorted array
 */
Array.prototype.sort = function sort(cmp) {
    return py.sorted(this, cmp);
};

