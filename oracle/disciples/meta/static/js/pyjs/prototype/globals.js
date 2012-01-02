/**
 * @fileOverview Global functions
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */

/*global ValueError*/

(function() {

function insertionSort(a, cmp) {
    var i, tmp, len = a.length;
    a.iter(function(tmp, j) {
        if (j == a.length - 1) {return;}
        i = j;
        tmp = a[j+1];
        while ( i > -1 && cmp(a[i], tmp)) {
            a[i+1] = a[i];
            i = i - 1;
        }
        a[i+1] = tmp;
    });
    return a;
}

function quickSort(a, cmp) {
    var alength = a.length, tmp;
    function swap(a, pos1, pos2) { // still waiting for javascript 1.8
        tmp = a[pos2];
        a[pos2] = a[pos1];
        a[pos1] = tmp;
    }

    function partition(a, start, end) {
        while (start < end) {
            while (start < end) {
                if (cmp(a[start], a[end])) {
                    swap(a, start, end);
                    break;
                }
                end--;
            }
            while (start < end) {
                if (cmp(a[start], a[end])) {
                    swap(a, start, end);
                    break;
                }
                start++;
            }
        }
        return start;
    }

    function qsort(a, start, end) {
        if (start >= end) { return; }
        var idx = partition(a, start, end - 1);
        qsort(a, start, idx);
        qsort(a, idx+1, end);
    }

    qsort(a, 0, alength);
    return a;
}

var methods = {
    /** @lends py*/

    /**
     * Check if an obj is Not Undefined And Not Null
     * @param obj anything
     * @returns {Boolean}
     */
    notNone: function notNone(o) {
        return (typeof o !== "undefined" && o !== null);
    },

    /**
     * Check if an obj is Undefined Or Null
     * @param obj anything
     * @returns {Boolean}
     */
    isNone: function isNone(o) {
        return (typeof o === "undefined" || o === null);
    },

    /**
     * raise a TypeError is an object is undefined or null
     * @param anything
     */
    raiseNone: function raiseNone(o) {
        if (py.isNone(o)) {
            throw new TypeError("Object is Undefined Or Null");
        }
    },

    /**
     * Check is an object is an instance of a class. If you give
     * more than one class, it will return true if one of them
     * is good.
     * @params {Object} obj Object to check
     * @params {Object} _class A class, You can give more than one class
     * @returns {Boolean}
     */
    isinstance: function isinstance(obj, _class) {
        /*<debug*/
        if (py.isNone(obj)) {
            throw new ValueError("instance argument must not be null");
        }
        if (py.isNone(_class)) {
            throw new ValueError("class argument must not be null");
        }
        /*debug>*/
        var check_proto = function(obj_proto, proto) {
            if (obj_proto.__name__ === proto.__name__) {
                return true;
            }
            var b = obj_proto.__bases__, i = 0, len = (b ? b.length : 0);
            while (i < len) {
                if (check_proto(b[i].prototype, proto)) {
                    return true;
                }
                i += 1;
            }
            return false;
        };
        var l = arguments.length;
        if (l === 2) {
            return check_proto(obj.__class__.prototype, _class.prototype);
        } else {
            for (; l>1; l -=1) {
                if (check_proto(obj.__class__.prototype, arguments[l-1].prototype)) {
                    return true;
                }
            }
            return false;
        }
    },

    /**
     * Check if two object are equal
     * @param a
     * @param b
     * @returns {Boolean}
     */
    equal: function equal(a, b) {
        if (py.notNone(a) && py.notNone(b)) {
            return a.equals(b);
        } else if (py.isNone(a) && py.isNone(b)) {
            return a === b;
        } else {return false;}
    },

    /**
     * Returns length of an Iterable object
     * @param {Object} obj
     * @returns {Number} length
     */
    len: function len(obj) {
        /*<debug*/py.raiseNone(obj);/*debug>*/
        return obj.__len__();
    },

    /**
     * Returns an object iterator
     * @param {Object} obj
     * @returns {Object} Iterator
     */
    iter: function iter(obj) {
        /*<debug*/py.raiseNone(obj);/*debug>*/
        return obj.__iter__();
    },

    /**
     * Returns the string representation of anything
     * @param obj
     * @returns {String}
     */
    str: function str(obj) {return py.notNone(obj) ? obj.__str__() : obj+'';},

    /**
     * Returns the Json representation of an Object
     * @param {Object} obj
     * @returns {String|null}
     */
    repr: function repr(obj) {
        if (py.isNone(obj)) { return "null"; }
        return obj.__repr__();
    },

    /**
     * This function do nothing
     */
    nothing: function nothing(){},

    /**
     * Sort an Iterable
     * @param {Array} Iterable to sort
     * @param {Function} [cmp=>] comparator function
     * @returns {Array} Sorted values in an array
     */
    sorted: function(a, cmp) {
        var tmp;
        if (py.isinstance(a, Array)) {
            tmp = a.slice();
        } else {
            tmp = [];
            a.iter(function(i, idx) { tmp[idx] = i; });
        }
        if (py.isNone(cmp)) {
            cmp = function(a, b) { return a > b;};
        }
        if (py.len(tmp) > 15) {
            return quickSort(tmp, cmp);
        } else {
            return insertionSort(tmp, cmp);
        }
    }
};

//WARNING cannot use update method here, isinstance is used ...
var m;
for (m in methods) {
    if (methods.hasOwnProperty(m)) {
        py[m] = methods[m];
    }
}


})();

if (py.config.withGlobals === true) {
    window.notNone = py.notNone;
    window.isNone = py.isNone;
    window.raiseNone = py.raiseNone;
    window.isinstance = py.isinstance;
    window.equal = py.equal;
    window.len = py.len;
    window.iter = py.iter;
    window.str = py.str;
    window.repr = py.repr;
    window.sorted = py.sorted;
} else {
    window.iter = null;
}


