

Number.prototype.__name__ = 'Number';
Number.prototype.__class__ = Number;

Number.prototype.equals = function(n) {
    return py.notNone(n) && py.isinstance(n, Number) && (this + 0 === n + 0);
};

Number.prototype.__repr__ = function() {
    if (this.isIn([NaN, Infinity, -Infinity])) {
        return null;
    }
    return this;
};
