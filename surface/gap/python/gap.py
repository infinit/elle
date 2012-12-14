# -*- encoding: utf-8 -*-

import _gap
from collections import namedtuple
import sys

"""
gap library binding module

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
            'enable_debug',
            'set_permissions',
            'set_device_name',

            'logout',
            'connect',
            'invite_user',
            'send_message',
            # Users.
            'search_users',
            'get_swaggers',
            'user_fullname',
            'user_email',

            # Networks.
            'launch_watchdog',
            'stop_watchdog',
            'create_network',
            'refresh_networks',
            'networks',
            'network_add_user',
            'network_name',
            'network_mount_point',

            # Transaction
            'send_files',
            'set_output_dir',
            'get_output_dir',
            'update_transaction',
            'transactions',
            'transaction_sender_id',
            'transaction_sender_fullname',
            'transaction_sender_device_id',
            'transaction_recipient_id',
            'transaction_recipient_fullname',
            'transaction_recipient_device_id',
            'transaction_network_id',
            'transaction_first_filename',
            'transaction_files_count',
            'transaction_total_size',
            'transaction_is_directory',
            'transaction_status',

            # Notifications
            'poll',
            'pull_notifications',
            'notifications_read',

            # Callback.
            'transaction_callback',
            'transaction_status_callback',
            'message_callback',
        ]

        def make_method(m):
            method = lambda *args: (
                self._call(m, *args)
            )
            method.__doc__ = getattr(_gap, m).__doc__
            return method

        for method in directly_exported_methods:
            setattr(self, method, make_method(method))

        self.Status = getattr(_gap, "Status")
        self.TransactionStatus = getattr(_gap, "TransactionStatus")

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

    @property
    def logged(self):
        return self._call('is_logged')

    def register(self, fullname, email, password, dev_name, activation_code):
        self.email = email
        pw_hash = self._call('hash_password', email, password)
        self._call('register', fullname, email, pw_hash, dev_name, activation_code)

    @property
    def _id(self):
        return self._call('_id');

if __name__ == "__main__":
    import doctest
    doctest.testmod()
