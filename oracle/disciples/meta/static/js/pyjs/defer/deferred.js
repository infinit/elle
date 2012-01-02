/**
 * @fileOverview Rewriting Deferred from twisted.internet.defer
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */

/**
 * defer namespace for Deferred class and derived ones
 * @namespace
 * @name py.defer
 */

/*global PyError, ValueError*/

py.declare('py.defer.AlreadyCalledError', PyError, {message: 'Deferred already called'});
py.declare('py.defer.DeferredFailure', PyError, {message: 'Last callback failure'});

(function(){
    var current_id = 0;
    function nextId() { return current_id++; }

    py.declare('py.defer.Deferred', null, {
        /** @lends py.defer.Deferred.prototype */

        callbacks: null,
        id: -1,
        called: false,
        paused: 0,
        results: null,
        canceller: null,
        silently_cancelled: false,

        __repr__: function() {
            return '<Deferred '+this.id+' : '+(
                this.paused ? 'paused' :
                (this.called ? 'called' : 'unfired')
            )+'>';
        },

        /**
         * Deferred class
         * @constructs
         * @param {Function} [canceller] canceller function
         */
        __init__: function (canceller) {
            this.callbacks = [];
            this.id = nextId();
            this.canceller = canceller;
        },

        /**
         * Add pair of callbacks (success and error)
         * @param {Object} callbacks Object containing callbacks
         * @param {Function} callbacks.callback Main callback
         * @param {Array} [callbacks.callback_args] Arguments to be passed to the callback
         * @param {Function} [callback.errback] Callback that will handle error
         * @param {Array} [callback.errback_args] Arguments to be passed to the errback
         */
        addCallbacks: function addCallbacks(callbacks) {
            //<debug
            py.raiseNone(callbacks);
            if (py.isNone(callbacks.callback) || !py.isinstance(callbacks.callback, Function)) {
                throw new ValueError("callbacks must contain a Function called 'callback'");
            }
            if (py.notNone(callbacks.errback) && !py.isinstance(callbacks.errback, Function)) {
                throw new ValueError('callbacks.errback must be a Function');
            }
            if (py.notNone(callbacks.callback_args) && !py.isinstance(callbacks.callback_args, Array)) {
                throw new ValueError('callbacks.callback_args must be an Array');
            }
            if (py.notNone(callbacks.errback_args) && !py.isinstance(callbacks.errback_args, Array)) {
                throw new ValueError('callbacks.errback_args must be an Array');
            }
            //debug>
            this.callbacks.append([
                [callbacks.callback, callbacks.callback_args || []],
                [callbacks.errback || py.nothing,  callbacks.errback_args || []]
            ]);
            if (this.called) {
                this._runCallbacks();
            }
            return this;
        },

        addCallback: function addCallback(callback, args) {
            return this.addCallbacks({
                callback: callback,
                callback_args: args,
                errback: py.nothing,
                errback_args: []
            });
        },

        addErrback: function addErrback(callback, args) {
            return this.addCallbacks({
                callback: py.nothing,
                callback_args: [],
                errback: callback,
                errback_args: args
            });
        },

        addBoth: function addErrback(callback, args) {
            return this.addCallbacks({
                callback: callback,
                callback_args: args,
                errback: callback,
                errback_args: args
            });
        },

        chainDeferred: function chainDeferred(d) {
            return this.addCallbacks({
                callback: d.callback.bind(d),
                errback: d.errback.bind(d)
            });
        },

        callback: function callback() {
            var args = Array.prototype.slice.call(arguments, 0);
            this._startRunCallbacks(args);
        },

        errback: function errback(fail) {
            if (py.isNone(fail) || !py.isinstance(fail, py.defer.DeferredFailure)) {
                fail = new py.defer.DeferredFailure(fail);
            }
            this._startRunCallbacks([fail]);
        },

        pause: function pause() {
            this.paused += 1;
        },

        unpause: function unpause() {
            this.paused -= 1;
            if (this.paused) {
                return;
            } else if (this.called) {
                this._runCallbacks();
            }
        },

        _startRunCallbacks: function(args) {
            if (this.called) {
                throw new py.defer.AlreadyCalledError();
            }
            this.called = true;
            this.last_result = args || [];
            this._runCallbacks();
        },

        _continue: function(res) {
            this.last_result = [res];
            this.unpause();
        },

        _runCallbacks: function() {
            if (this.paused === 0) {
                while (this.callbacks.length > 0) {
                    var item = this.callbacks.shift(),
                        callback, args, res;
                    if (py.isNone(this.last_result[0]) || !py.isinstance(this.last_result[0], py.defer.DeferredFailure)) {
                        callback = item[0][0];
                        args = item[0][1];
                    } else {
                        callback = item[1][0];
                        args = item[1][1];
                    }
                    args = this.last_result.concat(args);
                    try {
                        res = callback.apply(null, args);
                        if (py.notNone(res) && py.isinstance(res, py.defer.Deferred)) {
                            this.pause();
                            res.addBoth(this._continue.bind(this));
                            break;
                        }
                        this.last_result = [res];
                    } catch (err) {
                        this.last_result = [new py.defer.DeferredFailure()];
                    }
                }
            }
        }
    });
})();

