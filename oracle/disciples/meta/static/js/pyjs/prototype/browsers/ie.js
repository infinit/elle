

/****************************************************************************
 * Element prototype fixes :
 *      - include HTC file with CSS rule
 *      - redefine document.createElement
 ****************************************************************************/

py.addOnLoad(function() {
    // Creation of a <style> tag
    var head = py.doc.getElementsByTagName('head')[0],
        css_node = py.doc.createElement('style');
    css_node.type = 'text/css';
    css_node.rel = 'stylesheet';
    css_node.media = 'screen';
    css_node.title = 'ie_fix';
    head.appendChild(css_node);

    // We search now the stylesheet we just create
    var style_sheet, sheets = py.doc.styleSheets, i, l;
    for (i = 0, l = sheets.length; i < l; i++) {
        if (sheets[i].title === "ie_fix") {
            style_sheet = sheets[i];
            break;
        } else { continue; }
    }
    if (!style_sheet) {
        warn('Cannot add IE StyleSheet !!');
        return;
    }

    var path = py._modules_path.py + 'prototype/browsers/ie_fix.htc';

    // We finally have our style sheet !
    style_sheet.addRule('*', '{ behavior: url('+path+')}');

    document.nativeCreateElement = document.createElement;

    document.createElement = function(tag) {
        var el = document.nativeCreateElement(tag);
        Element.prototype.iteritems(function(k, v) {
            el[k] = v;
        });
        return el;
    };

    //TODO getElementsByName
});
