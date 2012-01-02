/**
 * @fileOverview Provide a simple way to put all package dependencies in one File
 * @author <a href="mailto:raphael.londeix@gmail.com">Raphaël Londeix</a>
 * @version 0.1
 */

// TODO: modularize choice of packer method
py.importModule('py.packer.jsmin');

/**
 * Packer utitities
 * @name py.packer
 * @namespace
 */


py.extendNamespace('py.packer', {

    /** @lends py.packer */

    RE_IMPORT: /py\._?importModule\(["'][\w\.\-_]+["']\)\s*;/g,
    RE_MODULE: /.*['"]([\w\.\-_]+)["'].*/,

    params: {
        lvl: 2,
        remove: ['debug'],
        render: true,
        with_lines: false
    },

    /**
     * Prepare recursively in one string all dependencies
     * @param {String|String[]} modules modules to pack (order is important)
     * @param {Object} [args] options of packaging :
     * @param {Number} [args.lvl=2] Compression level : no compression (0) to agressive compression (3)
     * @param {String[]} [args.remove=['debug']] remove parts of source code
     * @param {Boolean} [args.render=true] render the result in the actual page (replace body content)
     */
    pack: function(modules, args) {
        if (py.isNone(modules) || !py.isinstance(modules, Array, String)) {
            throw TypeError('Modules must be an Array or a String');
        }
        if (py.isinstance(modules, String)) {
            modules = [modules];
        }

        this.params.update(args || {});
        this._loaded_modules = [];
        this.input = 0;
        this.line = 0;
        var data = modules.map(this._packModule, this).join('\n');

        if (this.params.with_lines) {
            var self = this;
            data = '0 '+ data.replace(/\n/gm, function() {return '\n'+(++self.line)+' ';});
        }
        if (this.params.render) {
            this._render(data);
        }
        this.output = data.length;
        return {src: data, ratio: this.output / this.input};
    },

    _applyParams: function(src) {
        this.params.remove.iter(function(flag) {
            var re = new RegExp(
                '(/\\*<'+flag+'\\*/|//<'+flag+')'+
                '[\\s\\S]*?'+
                '(/\\*'+flag+'>\\*/|//'+flag+'>)',
                'g'
            );
            log('Remove flag : '+flag);
            src = src.replace(re, '');
        });
        if (this.params.lvl > 0) {
            log('Compress file with level '+ this.params.lvl);
            src = py.packer.compressor.jsmin('', src, this.params.lvl);
        }

        return src;
    },

    _packModule: function(name) {
        if (name.isIn(this._loaded_modules)) {
            return '';
        } else {
            this._loaded_modules.append(name);
        }
        var src = py.loadJs(py.getModuleUrl(name));
        this.input += src.length;
        src = this._applyParams(src);

        var matches = src.match(this.RE_IMPORT);
        if (!(py.isNone(matches) || py.len(matches) === 0)) {
            matches.iter(function(import_cmd) {
                var module = import_cmd.replace(this.RE_MODULE, '$1'),
                    sub_src = this._packModule(module);
                src = src.replace(import_cmd, sub_src.replace(/\$/g, '$$$$')); //escape $ with two $$
            }, this);
        }
        return (
            '\n\n///////////////////// LOADING MODULE "' + name + '"\n' +
            'py._loaded_modules.push("' + name + '");\n\n' +
            src
        );
    },

    _render: function(data) {
        py.doc.body.innerHTML = '';
        var area = py.doc.createElement('textarea');
        var node = py.doc.createTextNode(data);
        area.attr('cols', 80);
        area.attr('rows', 30);
        area.appendChild(node);
        py.doc.body.appendChild(area);
    }
});
