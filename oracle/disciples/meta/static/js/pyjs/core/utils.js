/**
 * @fileOverview Utilities functions
 */

py.update({
    /** @lends py */

    /**
     * Extend a namespace (created if needed) with an object
     * @param {String} name The namespace to extend
     * @param {Object} obj method and properties
     */
    extendNamespace: function extendNamespace(name, obj) {
        /*<debug*/
        py.raiseNone(name);
        py.raiseNone(obj);
        if (!py.isinstance(name, String)) {
            throw new TypeError("name must be a String");
        }
        if (!py.isinstance(obj, Object)) {
            throw new TypeError("obj must be an Object");
        }
        /*debug>*/
        var parts = name.split('.'),
            first = parts.shift();
        if (!window[first]) {
            window[first] = {};
        }
        var ref = window[first];
        parts.iter(function(part) {
            if (!ref[part]) {
                ref[part] = {};
            }
            //<debug
            if (!py.isinstance(ref[part], Object)) {
                throw new TypeError("The part "+part+" in the namespace "+name+
                                " is not an object.");
            }
            //debug>
            ref = ref[part];
        });
        ref.update(obj);
    },

    /**
     * Build url with parameters
     * @param {String} url
     * @param {Object} params
     * @returns {String}
     */
    buildUrl: function buildUrl(url, params) {
        var parts = [];
        if (py.notNone(params) && py.isinstance(params, Object)) {
            params.iteritems(function(k, v) {
                parts.append(
                    encodeURIComponent(k) + '=' + encodeURIComponent(v)
                );
            });
            return url + (url.indexOf('?') !== -1 ? '&' : '?') + '&'.join(parts);
        } else {
            return url;
        }
    }
});
