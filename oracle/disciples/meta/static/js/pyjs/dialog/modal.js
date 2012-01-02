/**
 * @fileOverview base class for dialogs
 * @author <a href="mailto:raphael.londeix@gmail.com"> Raphaël Londeix </a>
 * @version 0.1
 */


py.importModule('py.dialog.dialog');
py.importModule('py.defer.xhr');

py.declare('py.dialog.FormMixin', null, {
    _getFormValues: function _getFormValues() {
        var forms = this._node.query('form'),
            res = {},
            pos;
        if (py.len(forms) === 0) {
          forms.append(this._content_node ? this._content_node : this._node);
        }
        forms.iter(function (form) {
            var form_name = form.attr('name');
            if (py.isNone(form_name) || py.len(form_name) === 0) {
                pos = res;
            } else {
                res[form_name] = {};
                pos = res[form_name];
            }
            form.query('input,select').iter(function(input) {
                log('parse ', input);
                var name = input.attr('name'),
                    type = input.attr('type'),
                    value = input.attr('value');
                if (type == 'radio') {
                    if (input.checked) {
                        pos[name] = value;
                    } else if (py.isNone(pos[name])) {
                        pos[name] = null;
                    }
                } else {
                    pos[name] = value;
                }
            });
        });
        if (this._args.send_forms === true) {
            if (this.isValid(res)) {
                return (this._sendForms(forms, res));
            }
        } else {
            return (res);
        }
    },

    _sendForms: function(forms, values) {
        //<debug
        log('trying to send', forms, values);
        //debug>
        var res = null;
        forms.iter(function (form) {
            var form_name = form.attr('name') || '';
            var form_href = form.attr('action') || '';
            if ((py.len(form_href) === 0) || (py.len(form_name) === 0)) {
                //<debug
                warn('Form in a dialog need both "action" and "name" attributes to be sent', form);
                //debug>
                return ;
            }
            if (py.isNone(values[form_name])) {
                //<debug
                warn('No values in the form', form);
                //debug>
                return ;
            }
            var d = py.defer.xhrPost({
                query: values[form_name],
                url: form_href
            });
            if (py.notNone(res)) {
                res.chainDeferred(d);
            } else {
                res = d;
            }
        });
        return (res);
    },

    isValid: function(res) {
        return (true);
    }
});

/** @class */
py.declare('py.dialog.Modal', [py.dialog.Dialog, py.dialog.FormMixin], {
    /** @lends py.dialog.Modal.prototype */

    _cancel_str: "Cancel",
    _validate_str: "OK",

    _content_node: null,
    _toolbar_node: null,
    _validate_button: null,
    _cancel_button: null,

    /** @constructs */
    __init__: function __init__(args) {
        //<debug
        py.raiseNone(args);
        if (py.isNone(args.onValidate) || !py.isinstance(args.onValidate, Function)) {
            throw new TypeError("you must give a function onValidate");
        }
        //debug>
        var i=0;
        this.onValidate = args.onValidate;
        if (py.notNone(args.isValid)) {
            this.isValid = args.isValid;
        }
        this.$super(arguments);
        this._toolbar_node = $c('div', {"class": "pyDialogToolbar"});
        this._cancel_button = $c('button', {
            'class': 'pyDialogButton pyDialogCancelButton',
            'content': this._cancel_str
        });
        this._validate_button = $c('button', {
            'class': 'pyDialogButton pyDialogValidateButton',
            'content': this._validate_str
        });
        this._toolbar_node.appendChild(this._cancel_button);
        this._toolbar_node.appendChild(this._validate_button);
        this._node.appendChild(this._toolbar_node);
        this._cancel_hdlr = this._cancel_button.connect('click', this._onCancel.bind(this));
        this._validate_hdlr = this._validate_button.connect('click', this._onValidate.bind(this));
    },

    _onValidate: function() {
        var res = this.onValidate(this._getFormValues());
        if (py.notNone(res) && (res === false)) {
            return ;
        }
        this.hide();
    },

    onValidate: function () {
        warn('onValidate should be overrided');
    },

    _onCancel: function () {
        this.hide();
        this.onCancel();
    },

    onCancel: function () {},

    _refresh: function _refresh() {
        if (this._is_shown === true) {
            this._content_node.setStyle('height', '');
            this.$super("_refresh");
            var container = py.dom.layout(this._node, 'inner'),
                toolbar = py.dom.layout(this._toolbar_node, 'outer'),
                title = py.dom.layout(this._titlebar_node, 'outer');
            py.dom.layout(this._content_node, 'outer', {h: container.h - toolbar.h - title.h});
            //this.$super("_refresh"); // needed ?
        }
    },

    destroy: function destroy() {
        log('1');
        this._cancel_button.disconnect(this._cancel_hdlr);
        log('2');
        this._validate_button.disconnect(this._validate_hdlr);
        log('3');
        this.$super(arguments);
    }

});
