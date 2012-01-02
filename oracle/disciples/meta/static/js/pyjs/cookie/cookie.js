

py.extendNamespace('py.cookie', {
    // from http://www.javascripter.net/faq/settinga.htmd
    /**
     * set a cookie
     * @param name name of the cookie
     * @param value value of the cookie
     * @param days number of days before expire
     */
    set: function set(name, value, days) {
        var today = new Date();
        var expire = new Date();
        if (py.isNone(days) || days === 0) {
            days = 1;
        }
        expire.setTime(today.getTime() + 3600000 * 24 * days);
        py.doc.cookie = name + "=" + escape(value) + ";expires=" + expire.toGMTString();
    },

    // from http://www.javascripter.net/faq/readinga.htm
    /**
     * Get a cookie by name
     * @param name name of the cookie
     * @returns {String} cookie value or null
     */
    get: function get(name) {
         var c = "" + py.doc.cookie;
         var ind = c.indexOf(name + '=');
         if (ind == -1 || name === "") { return null; }
         var ind1 = c.indexOf(';', ind);
         if (ind1 == -1) { ind1 = c.length; }
         return unescape(c.substring(ind + name.length + 1, ind1));
    }
});
