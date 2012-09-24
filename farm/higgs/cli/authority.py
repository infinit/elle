#!/usr/bin/env python3
# -*- python -*-

import os
import time
import shutil
import subprocess
from infinitools import run_cmd

class AuthorityFile:
    def __init__(self, passphrase):
        self._passphrase = passphrase

    def destroy(self):
        Authority.destroy(self._passphrase)

    @property
    def passphrase(self):
        return self._passphrase

class Authority:
    def __init__(self):
        pass

    @staticmethod
    def create(passphrase=""):
        # p = run_cmd([
        #     "8authority", "-c",
        #     ],
        #     stdin=subprocess.PIPE)
        # p.communicate("{auth}\n".format(auth=passphrase).encode("ascii"))
        auth_src = os.path.join(os.environ["INFINIT_ROOT"],
                "oracle/infinit.auth")
        auth_dst = os.path.join(os.environ["INFINIT_HOME"], "infinit.auth")
        shutil.copyfile(auth_src, auth_dst)
        return AuthorityFile(passphrase)

    @staticmethod
    def destroy(passphrase=""):
        run_cmd([
            "8authority", "-d",
            ])
        # We don't need the passphrase for deleting the authority

    @staticmethod
    def fetch(path, passphrase=""):
        return AuthorityFile(passphrase)
