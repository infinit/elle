/**
 * @fileOverview Separate query function for modularity. Use of Sizzle css selector
 */

/**
 * Querying dom for nodes with CSS1-2-3 selectors.
 * @param {String} selectors (separated by comma)
 * @param {Element|Element[]|String|String[]} [ref] reference(s) node(s)
 * @returns {Array} arrays of selected nodes (possibly empty)
 * @methodOf py.dom
 */

py.importModule('py.core.sizzle');

py.extendNamespace('py.dom', {
    query: function (selectors, context) {
        return Sizzle(selectors, context);
    }
});
