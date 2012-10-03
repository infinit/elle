#!/usr/bin/env python3
# -*- python -*-

import os
import re
import sys
import copy
import shutil
import subprocess

from ..infinitools import run_cmd

ip = re.compile(b"[0-9]+(?:\.[0-9]+){3}:[0-9]{1,5}")

class Infinit:
    def __init__(self, home, mountpoint, *args):
        """
        This class is a wrapper over the infinit binary.
        """
        self.user = home.user
        self.network = home.network
        self.mountpoint = mountpoint
        self.home = home
        self.environ = copy.deepcopy(os.environ)
        for ext in ["idy", "dic"]:
            shutil.copyfile(
                    "{home}/users/{user}/{user}.{ext}".format(
                        home=self.home.home,
                        user=self.user,
                        ext=ext),
                    "{home}/infinit.{ext}".format(
                        home=self.home.home,
                        ext=ext)
                    );
        self.environ["INFINIT_HOME"] = home.home


    def launch(self):
        self._infinit = subprocess.Popen([
            "8infinit",
            "-u", self.user,
            "-n", self.network,
            "-m", self.mountpoint
            ],
            #stderr=subprocess.PIPE,
            env=self.environ)

    def __enter__(self):
        self.launch()
        return self

    def __exit__(self, type, value, trace):
        self.stop()

    def stop(self):
        self._infinit.terminate()
        self.wait()
        if self._infinit.returncode == None:
            self._infinit.kill()
            self.wait()

    def wait(self):
        self._infinit.wait()

    @property
    def pid(self):
        return self._infinit.pid

    @property
    def stderr(self):
        return self._infinit.stderr

