#!/usr/bin/env python3
# -*- python -*-

import subprocess
import sys
import os

from ..infinitools import run_cmd

class NetworkCreationError(Exception):
    def __init__(self):
        pass
    def __str__(self):
        return "creation error";

class Network:
    def __init__(self):
        pass;

    @staticmethod
    def create(auth, user_auth, name, model="slug", admin=None):
        p = run_cmd([
            "8network", "-c",
            "-n", name,
            "-m", model,
            "-a", admin
            ],
            stdin=subprocess.PIPE)
        p.communicate("{auth}\n{user}\n".format(auth=auth.passphrase,
            user=user_auth.passphrase).encode("ascii"))

    @staticmethod
    def destroy(name):
        run_cmd([
            "8network", "-d",
            "-n", name
            ])

if __name__ == "__main__":
    try:
        cwd = os.environ["INFINIT_HOME"]
    except KeyError:
        print("Don't forget the INFINIT_HOME environment variable")
        sys.exit(1)
    n = Network()
    n.create(name="bite", admin="fabien")
    n.destroy(name="bite")
