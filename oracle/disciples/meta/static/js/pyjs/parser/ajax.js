/**
 * @fileOverview HTML Parser
 * @author <a href="mailto:raphael.londeix@gmail.com>Raphaël Londeix</a>
 * @version 0.1
 */

py.importModule('py.parser.html');

py.declare('py.parser.AjaxHandler', [py.parser.HTMLElementHandler], {
    onMatch: function onMatch(element, elements) {
        var href = element.attr('href');
        element.attr('href', '#' + href);

    }
});

