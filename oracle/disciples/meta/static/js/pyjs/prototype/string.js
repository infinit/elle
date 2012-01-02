/**
 * @fileOverview This is String prototype augmentation
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */


/**
 * The String prototype
 * @name String
 * @class
 */


/**
 * @property {String} __name__ The object class
 * @private
 */
String.prototype.__name__ = 'String';
String.prototype.__class__ = String;

/**
 * Returns string representation of the object
 * @private
 * @returns {String}
 */
String.prototype.__str__ = function() {
    return this.toString();
};

/**
 * Returns Json representaion of the object
 * @private
 * @returns {String}
 */
String.prototype.__repr__ = function() {
    return '"' + this.replace('\\', '\\\\', 'g').replace('"', '\\"', 'g') +  '"';
};


/**
 * Specific implementation for strings
 * @param {String} s String to compare with
 * @returns {Boolean}
 */
String.prototype.equals = function(s) {
    return s !== null &&
           s.__name__ === 'String' &&
           this.toString() === s.toString();
};

/**
 * Join a array
 * @param {Iterable} an Iterable to join
 * @returns {String}
 */
String.prototype.join = function(a) {
    /*<debug*/py.raiseNone(a);/*debug>*/
    if (!py.isinstance(a, String)) {
        return a.join(this);
    }
    var res = '';
    a.iter(function (el, idx) {
        res += (idx > 0 ? this : '') + el.toString();
    }, this);
    return res;
};

/**
 * Return lower cased string
 * @returns {String}
 */
String.prototype.lower = function () {
    return this.toLowerCase();
};

/**
 * Return upper cased string
 * @returns {String}
 */
String.prototype.upper = function () {
    return this.toUpperCase();
};

/**
 * Returns the number of contained objects
 * @private
 * @returns {Number}
 */
String.prototype.__len__ = function() {
    return this.length;
};


(function(){
    function StringIterator(s) {
        this._str = s;
        this._len = s.length;
        this._index = 0;
        this.__name__ = 'StringIterator';
    }
    StringIterator.prototype.next = function next() {
        if (this._index < this._len) {
            var c = this._str.charAt(this._index);
            this._index += 1;
            return c;
        }
        throw new StopIteration();
    };
    /**
     * Specific implementation for String. It returns an `Iterable` Object
     * @private
     * @returns {Array}
     * @see <a href="http://docs.python.org/library/stdtypes.html#iterator-types">Python Iterators</a>
     */
    String.prototype.__iter__ = function() {
        return new StringIterator(this);

        // TODO: figure out why this is not working on Chrome
        // to reproduce :
        //      a = 'pif';
        //      a.foo = 'bar';
        //      a.foo === 'bar'; //false
        //this._index = 0;
        //this._len = this.__len__();
        //return this;
    };

})();

/**
 * Return the next character or raise StopIteration
 * @returns {String} the next char
 * @private
 * @see <a href="http://docs.python.org/library/stdtypes.html#iterator-types">Python Iterators</a>
 * @ignore
 */
String.prototype.next = function() {
    // WARNING: Not used, see __iter__
    if (this._index < this._len) {
        var val = this.charAt(this._index);
        this._index += 1;
        return val;
    }
    throw new StopIteration();
};

/**
 * Strip whitespace around a string
 * @returns {String} striped string
 * @function
 * @see <a href="http://blog.stevenlevithan.com/archives/faster-trim-javascript"> Faster Javascript Trim</a>
 */
if (!String.prototype.trim) {
    String.prototype.trim = function trim() {
        var str = this.replace(/^\s+/, ''), i;
        for (i = str.length - 1; i >= 0; i--) {
            if (/\S/.test(str.charAt(i))) {
                str = str.substring(0, i + 1);
                break;
            }
        }
        return str;
    };
}

/**
 * Strip whitespace (default) or chars around a string
 * @param {String} [chars] characters to be striped
 * @returns {String} striped string
 * @function
 * @see <a href="http://blog.stevenlevithan.com/archives/faster-trim-javascript"> Faster Javascript Trim</a>
 */
String.prototype.strip =  function strip(chars) {
    if (py.isNone(chars)) {
        return this.trim();
    }
    var re_begin = new RegExp('^['+chars+']+'),
        re_char = new RegExp('[^'+chars+']'),
        str = this.replace(re_begin, ''),
        i;
    for (i = str.length - 1; i >= 0; i--) {
        if (re_char.test(str.charAt(i))) {
            str = str.substring(0, i + 1);
            break;
        }
    }
    return str;
};

/**
 * Strip leading whitespaces (default) or chars
 * @param {String} [chars] characters to strip out
 * @returns {String}
 */
String.prototype.lstrip = function lstrip(chars) {
    var re = py.notNone(chars) ? new RegExp('^['+chars+']+') : /^\s+/;
    return this.replace(re, '');
};

/**
 * Strip trailing whitespaces (default) or chars
 * @param {String} [chars] characters to strip out
 * @returns {String}
 */
String.prototype.rstrip = function rstrip(chars) {
    var str = this,
        re = py.notNone(chars) ? new RegExp('[^'+chars+']') : /\S/,
        i;
    for (i = str.length - 1; i >= 0; i--) {
        if (re.test(str.charAt(i))) {
            str = str.substring(0, i + 1);
            break;
        }
    }
    return str;
};

/**
 * Return true if string starts with given string
 * @param {String} str the sub string
 * @returns {Boolean}
 */
String.prototype.startswith = function startswith(str) {
    /*<debug*/
    if (py.isNone(str) || !py.isinstance(str, String)) {
        throw new TypeError('str must be a string');
    }
    /*debug>*/
    var len = str.length;
    if (len > this.length) {return false;}
    for (; len > 0; len--) {
        if (this.charAt(len-1) !== str.charAt(len-1)) {
            return false;
        }
    }
    return true;
};

/**
 * Return true if the string ends with given string
 * @param {String} str The sub string
 * @returns {Boolean}
 */
String.prototype.endswith = function endswith(str) {
    /*<debug*/
    if (py.isNone(str) || !py.isinstance(str, String)) {
        throw new TypeError('str must be a string');
    }
    /*debug>*/
    var len = str.length,
        tlen = this.length;
    if (len > tlen) { return false;}
    while (len > 0) {
        len--; tlen--;
        if (this.charAt(tlen) !== str.charAt(len)) {
            return false;
        }
    }
    return true;
};
String.prototype.regEscape = function() {
    return this.
           replace(/\\/g, '\\\\').
           replace(/\./g, '\\.').
           replace(/\$/g, '\\$$').
           replace(/\[/g, '\\[').
           replace(/\]/g, '\\]').
           replace(/\(/g, '\\(').
           replace(/\)/g, '\\)').
           replace(/\{/g, '\\{').
           replace(/\}/g, '\\}').
           replace(/\^/g, '\\^').
           replace(/\*/g, '\\*').
           replace(/\+/g, '\\+').
           replace(/\-/g, '\\-');
};

String.prototype.contains = function contains(s) {
    //<debug
    if (py.isNone(s) || !py.isinstance(s, String)) {
        throw new TypeError('Argument s must be a String');
    }
    //debug>

    return this.indexOf(s) !== -1;
};

String.prototype.isIn = function isIn(o) {
    /*<debug*/py.raiseNone(o);/*debug>*/
    if (py.isinstance(o, String)) {
        return o.indexOf(this) !== -1;
    } else {
        return Object.prototype.isIn.call(this, o);
    }
};

String.prototype.filter = function filter(f) {
    var i, l, c, res = '';
    for (i=0, l=this.length; i < l; i++) {
        c = this.charAt(i);
        if (f(c, i)) {
            res += c;
        }
    }
    return res;
}
