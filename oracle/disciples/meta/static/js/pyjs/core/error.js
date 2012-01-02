py.declareFromBuiltin('PyError', Error, {
    message: 'High level error',
    __init__: function(msg) {
        this.message += ' : ' + py.str(msg);
    }
});
py.declareFromBuiltin('StopIteration', PyError, {__init__: py.nothing});
py.declareFromBuiltin('AttributeError', PyError, {message: 'Attribute not found'});
py.declareFromBuiltin('KeyError', PyError, {message: 'Key not found'});
py.declareFromBuiltin('ValueError', PyError, {message: 'Value not allowed'});
