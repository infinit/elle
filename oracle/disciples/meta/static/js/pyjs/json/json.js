
py.extendNamespace('py.json', {
    fromJson: function fromJson(str) {
        return eval('(' + str + ')');
    },
    toJson: function toJson(o) {
        return py.repr(o);
    }
});
