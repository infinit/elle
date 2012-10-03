#!/usr/bin/env python3
# -*- python -*-

import subprocess
import sys
import os

from ..infinitools import run_cmd

class UserAuth:
    def __init__(self, name, passphrase):
        self._name = name
        self._passphrase = passphrase

    def destroy(self):
        User.destroy(self._name)

    @property
    def passphrase(self):
        return self._passphrase

    @property
    def name(self):
        return self._name

class UserDoesntExist(Exception):
    def __init__(self, name):
        self._name = name

    def __str__(self):
        return self._name + "doesn't exist"

class User:
    def __init__(self):
        pass

    @staticmethod
    def create(auth, name, passphrase=""):
        p = run_cmd([
            "8user", "-c",
            "-n", name
            ],
            stdin=subprocess.PIPE)
        p.communicate("{auth}\n{user}\n".format(auth=auth.passphrase,
            user=passphrase).encode("ascii"))
        return UserAuth(name, passphrase)


    @staticmethod
    def destroy(name, passphrase=""):
        run_cmd([
            "8user", "-d",
            "-n", name
            ])
        # We don't need any passphrase for delete the user
        #p.communicate("{auth}\n{user}\n".format(auth=auth.passphrase,
        #    user=passphrase).encode("ascii"))

    @staticmethod
    def fetch(auth, name, passphrase=""):
        return UserAuth(name, passphrase)
