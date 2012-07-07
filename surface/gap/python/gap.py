# -*- encoding: utf-8 -*-

import _gap

"""gap library binding module

>>> state = State
>>> state.meta_status
True
>>> state.register("Testing Program", "test@infinit.io", "MEGABIET", "dev name")
>>>

"""

class State:
    """State is the interface to gap library functions
    """

    def __init__(self):
        self._state = _gap.new()
        self.email = ''

    def __del__(self):
        _gap.free(self._state)

    @property
    def meta_status(self):
        try:
            self._call('meta_status')
        except:
            return False
        return True

    def _call(self, method, *args):
        res = getattr(_gap, method)(self._state, *args)
        if isinstance(res, _gap.Status) and res != _gap.gap_ok:
            raise Exception(
                "Error while calling %s: %s " % (method, str(res))
            )
        return res

    def login(self, email, password):
        self.email = email
        pw_hash = self._call('hash_password', email, password)
        try:
            self._call('login', email, pw_hash)
        finally:
            _gap.hash_free(pw_hash)

    def register(self, fullname, email, password, dev_name):
        self.email = email
        pw_hash = self._call('hash_password', email, password)
        try:
            self._call('register', fullname, email, pw_hash, dev_name)
        finally:
            _gap.hash_free(pw_hash)

    def create_network(self, name):
        self._call('create_network', name)

    def networks(self):
        return _gap.networks(self._state)

    def launch_watchdog(self, binary):
        return _gap.launch_watchdog(self._state, binary);

if __name__ == "__main__":
    import doctest
    doctest.testmod()
