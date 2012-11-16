# -*- encoding: utf-8 -*-

import _gap
from collections import namedtuple
import sys

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
    read_perm = _gap.gap_read
    write_perm = _gap.gap_write
    exec_perm = _gap.gap_exec

    def __init__(self):
        self._state = _gap.new()
        if self._state == None:
            raise Exception("Couldn't create state.")
        self.email = ''

        directly_exported_methods = [
            'create_network',
            'enable_debug',
            'refresh_networks',
            'networks',
            'launch_watchdog',
            'stop_watchdog',
            'set_permissions',
            'set_device_name',
            'network_add_user',
            'network_name',
            'network_mount_point',
            'logout',
            'connect',
            'poll',
            'OnBite',
            'OnTransaction',
            'OnTransactionStatus',
            'OnMessage',
            'send_files',
            'update_transaction',
            'invite_user',
            'scratch_db',
            'send_message',
            'get_notifications',
            'notifications_red',
        ]

        directly_exported_enums = [
            'Status',
            'TransactionStatus',
        ]

        def make_method(m):
            method = lambda *args: (
                self._call(m, *args)
            )
            method.__doc__ = getattr(_gap, m).__doc__
            return method

        for method in directly_exported_methods:
            setattr(self, method, make_method(method))

        # transforms gap_EnumName_value to EnumName.value
        for enum in directly_exported_enums:
            to_remove = len('gap_') + len(enum) + 1
            l_mbrs = [i[to_remove:] for i in dir(getattr(_gap, enum)) if i.startswith('gap_')]
            l_vals = [getattr(_gap, "gap_"+enum+"_"+m) for m in l_mbrs]
            T = namedtuple(enum, " ".join(l_mbrs))
            t = T(*l_vals)
            setattr(self, enum, t)

    def __del__(self):
        _gap.free(self._state)

    @property
    def meta_status(self):
        try:
            return self._call('meta_status') == self.Status.ok
        except:
            return False

    @property
    def has_device(self):
        try:
            return self._call('device_status') == self.Status.ok
        except:
            return False

    def _call(self, method, *args):
        res = getattr(_gap, method)(self._state, *args)
        if isinstance(res, _gap.Status) and res != self.Status.ok:
            raise Exception(
                "Error while calling %s: %s " % (method, str(res))
            )
        return res

    def login(self, email, password):
        self.email = email
        pw_hash = self._call('hash_password', email, password)
        self._call('login', email, pw_hash)

    def register(self, fullname, email, password, dev_name, activation_code):
        self.email = email
        pw_hash = self._call('hash_password', email, password)
        self._call('register', fullname, email, pw_hash, dev_name, activation_code)

if __name__ == "__main__":
    import doctest
    doctest.testmod()
