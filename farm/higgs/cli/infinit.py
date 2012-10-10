#!/usr/bin/env python3
# -*- python -*-

import os
import re
import sys
import copy
import time
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
            stderr=subprocess.STDOUT,
            stdout=subprocess.PIPE,
            env=self.environ
        )

    def __enter__(self):
        self.launch()
        return self

    def __exit__(self, type, value, trace):
        self.stop()

    def stop(self):
        print("Send SIGTERM to {0}".format(self.pid))
        self._infinit.terminate()
        #self._infinit.send_signal(2)
        i = 0
        while self._infinit.poll() is None:
            print("Still waiting (pid = {0})(i = {1})".format(self.pid, i))
            time.sleep(1)
            i += 1
            if i == 10:
                break

        print("Finished polling")
        if self._infinit.returncode == None:
            print("Sending SIGKILL to", self.pid)
            self._infinit.kill()
            print("Waiting... (", self.pid, ")")
            self.wait()

    def wait(self):
        self._infinit.wait()

    @property
    def pid(self):
        return self._infinit.pid

    @property
    def stderr(self):
        return self._infinit.stderr

