/**
 * @fileOverview Function object prototyping
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */


Function.prototype.__name__ = 'Function';
Function.prototype.__class__ = Function;

(function(){

    var _slice_args = Array.prototype.slice;

    /**
     * bind a function to any scope
     * @param {Object} scope Scope of the function
     * @param {Array} args fixed arguments given first
     * @returns {Function} binded function
     */
    Function.prototype.bind = function bind(scope, args) {
        //<debug
        if (py.notNone(args) && !py.isinstance(args, Array)) {
            throw new TypeError("args must be an Array");
        }
        //debug>
        var method = this;
        if (py.isNone(args)) {
            return function() {
                return method.apply(scope, arguments);
            };
        } else {
            return function() {
                return method.apply(
                    scope,
                    args.concat(_slice_args.call(arguments, 0))
                );
            };
        }
    };


})();
