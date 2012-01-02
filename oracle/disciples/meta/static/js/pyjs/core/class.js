/**
 * @fileOverview Class declaration and inheritance
 * @author <a href="mailto:raphael.londeix@gmail.com>Raphaël Londeix</a>
 * @version 0.1
 */

/*global ValueError, StopIteration, AttributeError*/

py._reserved_names = [
    '__name__',
    '__class__',
    '__bases__',
    '__inherited_functions__',
    '$super',
    'toString'
];

py.declareFromBuiltin = function(name, _parent, obj) {
    var _class = function() {
        this.__init__.apply(this, arguments);
    };
    if (_parent) {
        _class.prototype = new _parent();
    } else {
        _class.prototype = {};
        _parent = Object;
    }
    _class.prototype.constructor = _class;
    _class.prototype.__name__ = name;
    _class.__name__ = name;
    _class.prototype.__class__ = _class;
    _class.prototype.__bases__ = [_parent.prototype];


    var k;
    for (k in obj) {
        if (obj.hasOwnProperty(k) && k !== "constructor") {
            _class.prototype[k] = obj[k];
        }
    }
    window[name] = _class;
    return _class;
};

py._getClassSuper = function(_class) {
    var inherited_fn = _class.prototype.__inherited_functions__;
    return function(/*arguments|String*/ args, /*Array?*/ real_args) {
        /*<debug*/py.raiseNone(args);/*debug>*/
        var fname, f;
        if (py.isinstance(args, String)) {
            fname = args;
            real_args = real_args || [];
        } else {
            /*<debug*/try {/*debug>*/
            fname = args.callee.__name__;
            /*<debug*/ } catch (err) {
                 throw new TypeError("$super first argument must be the JavaScript vari"+
                 "able 'arguments' or a String:"+ err);
             }/*debug>*/
        }
        if (fname.isIn(inherited_fn)) {
            f = py.$super(inherited_fn[fname], this, fname);
        } else {
            f = py.$super(_class, this, fname);
        }
        if (py.notNone(real_args)) {
            return f.apply(null, real_args);
        } else {
            return f.apply(null, args);
        }
    };
};


/**
 * Declare a new class
 * @param {String} name The class name, possibly a dotted namespace
 * @param {Array|Class} [parents] Bases class
 * @param {Object} obj The Class methods and members
 */
py.declare = function(name, parents, obj) {
    //<debug
    if (py.isNone(obj)) {
        throw ValueError("You must supply an object to declare a class (perhaps you just forgot parent argument)");
    }
    //debug>
    var _class = function() {
        if (py.notNone(_class.prototype.__init__)) {
            _class.prototype.__init__.apply(this, arguments);
        }
    };
    _class.prototype.__name__ = name;
    _class.__name__ = name;
    _class.prototype.__class__ = _class;
    _class.prototype.__inherited_functions__ = {};
    _class.prototype.__repr__ = function() {
        return '<Class '+this.__name__+'>';
    };
    _class.prototype.toString = function() {
        return py.repr(this);
    };

    var _parents = parents || [];
    if (!py.isinstance(_parents, Array)) {
        _parents = [parents];
    }
    _class.prototype.__bases__ = _parents;

    _parents.iter(function(pclass) {
        if (pclass && pclass.prototype && pclass.prototype.iteritems) {
            try {
                pclass.prototype.iteritems(function(key, val) {
                    if (key.isIn(py._reserved_names)) {return;}
                        _class.prototype[key] = val;
                    if (py.notNone(val) && py.isinstance(val, Function)) {
                        _class.prototype.__inherited_functions__[key] = pclass;
                        _class.prototype[key].__name__ = key;
                    }
                });
            } catch (err) {
                warn(err);
            }
        }
    });

    obj.iteritems(function(key, val) {
        if (key.isIn(py._reserved_names)) {return;}
        if (key === 'constructor') {return;}
        _class.prototype[key] = val;
        if (py.notNone(val) && py.isinstance(val, Function)) {
            _class.prototype.__inherited_functions__[key] = _class;
            _class.prototype[key].__name__ = key;
        }
    });
    _class.prototype.$super = py._getClassSuper(_class);
    var parts = name.split('.');
    if (parts.length > 1) {
        name = parts.pop();
        var container = {};
        container[name] = _class;
        py.extendNamespace('.'.join(parts), container);
    } else {
        window[name] = _class;
    }

    return _class;
};

py.superMixin = function(_class, obj) {
    throw "SuperMixin not implemented";
};

py.superFunction = function(_class, obj, f) {
    var _parents = _class.prototype.__bases__.slice().reverse(),
    it = _parents.__iter__();
    var super_func = function(parent) {
        var p = parent;
        return function() {
            var old_super = obj.$super;
            obj.$super = p.prototype.$super;
            //<debug
            try {
                //debug>
                var res = p.prototype[f].apply(obj, arguments);
                //<debug
            } catch(err) {
                warn('Error while executing "'+p.prototype.__name__+'.'+f+'" for', obj, ':', err);
            }
            //debug>
            obj.$super = old_super;
            return res;
        };
    };

    while (true) {
        try {
            var p = it.next();
            //<debug
            //log('$super for function "' + f + '" in', p.prototype.__name__, 'for', obj);
            //debug>
            if (py.notNone(p.prototype[f])) {
                if (py.isinstance(p.prototype[f], Function)) {
                    return super_func(p);
                } else {
                    throw new ValueError(Object.__name__ + '.' + f + ' is not callable');
                }
            }
        } catch (err) {
            if (py.isinstance(err, StopIteration)) {
                throw new AttributeError(obj.__name__ + ' has no parent with member '+f);
            } else {
                throw err;
            }
        }
    }
};

/**
 * Returns neither parent function of an object, or
 * an appropriate mixin of parents methods
 * @param {Class} _class The class from which search of parents
 * @param {Object} obj The instance
 * @param {String} [f] The function name to get from bases class. If not
 *                     specified, function returns a mixin object
 * @returns {Object|Function} A mixin object or the base function
 */
py.$super = function $super(_class, obj, f) {
    //<debug
    py.raiseNone(_class);
    py.raiseNone(obj);
    //debug>
    if (py.notNone(f)) {
        return py.superFunction(_class, obj, f);
    } else {
        return py.superMixin(_class, obj);
    }
};

