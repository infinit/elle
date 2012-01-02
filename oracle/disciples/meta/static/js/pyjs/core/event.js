/**
 * @fileOverview publish/subscribe methods
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */

/**
 * Events classes used for publish/subscribe methods
 * @namespace
 * @name py.event
 */


(function(){
    var cur_id = 0;
    var getHandlerId = function () {
        cur_id += 1;
        return (cur_id);
    };

    /** @class */
    py.declare('py.event.Handler', null, {
        /** @lends py.event.Handler.prototype */

        // unique id
        _id: null,

        // event_name
        _event_name: null,

        // function to call
        _func: null,

        /**
         * Generic handler
         * @param {String} evt_name the name of event
         * @param {Object} The scope for the callback
         * @param {Function} func callback
         * @param {Function} [disconnect] Specify a disconnector function if obj
         *                                have not any 'disconnect' method
         * @constructs
         */
        __init__: function __init__(evt_name, obj, func, _disconnect) {
            this._id = getHandlerId();
            this._event_name = evt_name;
            this._func = func.bind(obj);
            this._obj = obj;
            if (py.notNone(_disconnect)) {
                this.disconnect = _disconnect;
            }
        },

        disconnect: function() {
            //<debug
            if (
                    py.isNone(this._obj) ||
                    py.isNone(this._obj.disconnect) ||
                    !py.isinstance(this._obj.disconnect, Function)
                ) {
                throw new TypeError("Cannot find any disconnect function for "+this._event_name);
            }
            try {
                //debug>
                this._obj.disconnect(this);
                //<debug
            } catch (err) {
                warn("Error while disconnect", this, "from", this._obj);
            }
            //debug>
        },

        /**
         * fire function
         * @param {Array} args array of arguments to pass
         */
        fire: function fire(args) {
            this._func.apply(null, args);
        }
    });

    var hdlrs = {};

    py.extendNamespace('py', {
        /** @lends py */

        /**
         * Subscribe to an event
         * @param {String} evt_name The event name
         * @param {Object|Function} obj_or_func The 'this' object or the function
         * @param {Function} [func] Function to call
         * @returns {py.event.Handler} The handler
         */
        subscribe: function subscribe(evt_name, obj_or_func, func) {
            //<debug
            if (py.isNone(evt_name) || !py.isinstance(evt_name, String)) {
                throw new ValueError("evt_name must be a string");
            }
            if (py.isNone(obj_or_func)) {
                py.raiseNone(func);
            }
            if (py.notNone(obj_or_func) && py.notNone(func) && !py.isinstance(func, Function)) {
                throw new ValueError("func must be a Function !");
            } else if (py.isNone(func) && !py.isinstance(obj_or_func, Function)) {
                throw new ValueError("obj_or_func must be a Function !");
            }
            //debug>
            if (py.isNone(hdlrs[evt_name])) {
                hdlrs[evt_name] = [];
            }
            var obj = null;
            if (py.isNone(func)) {
                func = obj_or_func;
            } else {
                obj = obj_or_func;
            }
            var hdlr = new py.event.Handler(evt_name, obj, func);
            hdlr.disconnect = function disconnect() {
                setTimeout(function(){py.unsubscribe(hdlr);}, 0);
            };
            hdlrs[evt_name].append(hdlr);
            return (hdlr);
        },

        /**
         * Declare an event
         * @param {String} evt_name The event name
         */
        register: function register(evt_name) {
            //<debug
            if (py.isNone(evt_name) || !py.isinstance(evt_name, String)) {
                throw new ValueError("evt_name must be a string");
            }
            //debug>
            if (py.isNone(hdlrs[evt_name])) {
                hdlrs[evt_name] = [];
            } /*<debug*/ else {
                warn("The event '"+evt_name+"' already exists !");
            }/*debug>*/
        },

        /**
         * publish an event
         * @param {String} evt_name The event name
         * @param {Array} [args] Arguments to pass to handlers
         */
        publish: function publish(evt_name, args) {
            //<debug
            if (py.isNone(evt_name) || !py.isinstance(evt_name, String)) {
                throw new ValueError("evt_name must be a string");
            }
            if (py.isNone(hdlrs[evt_name])) {
                throw new ValueError("the event '"+evt_name+"' is not registered, use register or subscribe it first !");
            }
            if (py.notNone(args) && !py.isinstance(args, Array)) {
                throw new ValueError("args must be an array");
            }
            //debug>
            hdlrs[evt_name].iter(function(hdlr) {
                hdlr.fire(args);
            });
        },

        //<debug
        getHandlers: function() {
            return (hdlrs);
        },
        //debug>

        /**
         * detach an handler
         * @param {py.event.Handler} handler The handler to detach (returned by py.subscribe)
         */
        unsubscribe: function unsubscribe(handler) {
            //<debug
            if (py.isNone(handler) || !py.isinstance(handler, py.event.Handler)) {
                throw new TypeError("handler must be an instance of py.event.Handler");
            }
            //debug>
            var index = -1;
            var event_name = handler._event_name;
            hdlrs[event_name].iter(function(hdlr, idx) {
                if (hdlr._id === handler._id) {
                    index = idx;
                    throw new StopIteration();
                }
            });
            if (index >= 0) {
                delete hdlrs[event_name][index];
                hdlrs[event_name].splice(index, 1);
            } /*<debug*/ else {
                warn('This handler does not exists anymore !');
            } /*debug>*/
        }

    });

})();
