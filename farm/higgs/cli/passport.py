#!/usr/bin/env python3
# -*- python -*-

import subprocess
from ..infinitools import run_cmd

class Passport:
    def __init__(self):
        pass

    @staticmethod
    def create(auth):
        """
        create an passport file
        """
        p = run_cmd([
            "8passport", "-c",
            ],
            stdin=subprocess.PIPE)
        p.communicate("{auth}\n".format(auth=auth.passphrase).encode("ascii"))


    @staticmethod
    def destroy(auth):
        run_cmd([
            "8passport", "-d",
            ])
        # We don't need the passphrase for deleting the passport
