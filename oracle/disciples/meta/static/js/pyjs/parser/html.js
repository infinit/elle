/**
 * @fileOverview HTML Parser
 * @author <a href="mailto:raphael.londeix@gmail.com>Raphaël Londeix</a>
 * @version 0.1
 */

py.importModule('py.parser.parser');

/**
 * Handler to match a HTML Element based on its tag name and classes
 * @name py.parser.HTMLElementHandler
 * @class
 */
py.declare('py.parser.HTMLElementHandler', [py.parser.Handler], {
    /** @lends py.parser.HTMLElementHandler */

    /**
     * Initialise an html handler
     * @param {Object} args Define matching properties
     * @param {String} args.tag Array of tag name to match
     * @param {String[]} args.classes Array of class name to match
     * @param {Function} args.onMatch function to call when match an object
     */
    __init__: function(args) {
        this.args = args;
        if (args.contains('onMatch')) {
            this.onMatch = args.onMatch;
        }
    },

    match: function(element, elements) {
        if (py.notNone(this.args.tag) && py.notNone(element.tagName) &&
            (this.args.tag.toUpperCase() != element.tagName.toUpperCase())) {
            return ;
        }
        if (py.notNone(this.args.classes)) {
            var found_cls = this.args.classes.all(function(class_name) {
                return (py.notNone(element.hasClass) && element.hasClass(class_name));
            });
            if (!found_cls) { return; }
        }
        this.onMatch(element, elements);
    }
});


/**
 * Parser for HTML node
 * @name py.parser.HTMLParser
 * @class
 */
py.declare('py.parser.HTMLParser', [py.parser.Parser], {
    /** @lends py.parser.HTMLParser */

    getNext: function(element, root, idx) {
        if (py.isNone(element)) {
            return root.firstChild;
        }
        if (py.notNone(element.firstChild)) {
            return (element.firstChild);
        }
        while (py.isNone(element.nextSibling)) {
            element = element.parentNode;
            if (root === element) {
                return (null);
            }
        }
        return (element.nextSibling);
    }
});
