/**
 * fileOverview Globals functions, generaly shortcuts
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */
/** scope _global_ */

/**
 * Get element by id
 * @param {String} id The element id
 * @returns {Element|null} First element found, if exists or null
 */
$ = function $(id) {return document.getElementById(id);};

/**
 * Query elements with CSS2 syntax
 * @param {String} selectors CSS2 selectors
 * @param {String|String[]|Element|Element[]} [ref] reference(s) node(s) to search in
 * @returns {Element[]} Array of results
 * @see py.dom.query
 */
$$ = function $$(selectors, ref) {return py.dom.query(selectors, ref);};

/**
 * Create an Element
 * @param {String} tag Tag name
 * @param {Object} [obj] attributes of the node
 * @returns {Element} created element
 * @see py.dom.create
 */
$c = py.dom.create;

/**
 * Create a text node
 * @param {String} str Text
 * @returns {Element} text node
 */
$t = function $t(str){return document.createTextNode(str);};


/**
 * Set or get attribute(s) from a node
 * @see py.dom.attr
 */
$attr = py.dom.attr;

/**
 * Set or get style(s) from a node
 * @see py.dom.style
 */
$style = py.dom.style;
