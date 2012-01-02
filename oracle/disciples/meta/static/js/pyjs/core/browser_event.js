/**
 * @fileOverview provides generals event from browser
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */

py.addOnLoad(function () {
    py.register("py/window/resize");
    py.register("py/window/scroll");
    var publish_resize = function () {
        py.publish('py/window/resize');
    };
    var publish_scroll = function () {
        py.publish('py/window/scroll');
    };
    if (window.addEventListener) {
        window.addEventListener('resize', publish_resize, false);
        window.addEventListener('scroll', publish_scroll, false);
    } else if (window.attachEvent) {
        window.attachEvent('onresize', publish_resize);
        window.attachEvent('onscroll', publish_scroll);
    } else {
        window.onresize = publish_resize;
        window.onscroll = publish_scroll;
    }
});
