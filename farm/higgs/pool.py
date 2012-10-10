#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import re
import os
import time
import shutil
import tempfile
import traceback
import subprocess as sp

from . import cli
from . import home
from . import conf
from . import infinitools


class PoolEngine:
    def __init__(self, low_port, high_port):
        self.low_port = low_port
        self.high_port = high_port

    def __exit__(self, type, value, trace):
        shutil.rmtree(self._basedir, ignore_errors=True)

    def __enter__(self):
        import pprint
        if "INFINIT_HOMEDIR" in os.environ:
            homedir = os.environ["INFINIT_HOMEDIR"]
        else:
            homedir = tempfile.mkdtemp(prefix="infinit.")

        if not os.path.exists(os.path.join(homedir, "infinit.auth")):
            os.environ["INFINIT_HOME"] = homedir
            auth = cli.Authority.create()

        self._basedir = homedir
        # Creat a new homedir for infinit
        new_home = os.path.join(homedir, "origin")
        os.environ["INFINIT_HOME"] = new_home
        os.mkdir(new_home)

        # Create the conf file
        c = conf.Conf()
        c.set_listen_port(self.low_port)
        c.set_timeout(1000)
        conf_file_path = os.path.join(new_home, "infinit.conf")
        with open(conf_file_path, "w") as conf_file:
            c.commit(file=conf_file)

        # Copy the generated auth into the new home
        auth_file_path = os.path.join(homedir, "infinit.auth")
        shutil.copy(auth_file_path, new_home)

        base = home.Home(auth, "user_test", "slug", "nettest", new_home, c);
        base.create()
        self._base = base
        return self

    @property
    def port(self):
        return self._base.port

    @property
    def basehomedir(self):
        return self._basedir

    def duplicate(self, dst_dir):
        return self._base.duplicate(dst_dir)

class Pool:
    def __init__(self, engine, number_of_networks):
        self.number_of_networks = number_of_networks
        self.l_home = []
        self.l_infinit_inst = []
        self.stderrs = []
        self._engine = engine

    @property
    def pids(self):
        return [I.pid for I in self.l_infinit_inst]

    @property
    def homedirs(self):
        return [H.home for H in self.l_home]

    @property
    def mountpoints(self):
        return [I.mountpoint for I in self.l_infinit_inst]

    @property
    def low_port(self):
        return self._engine.low_port

    @property
    def high_port(self):
        return self._engine.low_port

    def wait_ready(self):
        """
        Check the mount fs to see if we are ready to start the tests
        """
        while True:
            mounts = sp.check_output(["mount"]).split(b"\n")
            number_of_fs_mounted = 0
            for fs in mounts:
                for mountpoint in self.mountpoints:
                    if re.search(mountpoint.encode("ascii"), fs) != None:
                        number_of_fs_mounted += 1
            if number_of_fs_mounted == len(self.mountpoints):
                break
            print(number_of_fs_mounted)
            time.sleep(1)

    def __enter__(self):
        # Duplicate the home directories from H
        H = self._engine
        homedir = self._engine.basehomedir
        last_assigned_port = H.port
        for i in range(self.number_of_networks):
            new_homedir = tempfile.mkdtemp(dir=homedir)
            os.rmdir(new_homedir)
            new_H = H.duplicate(new_homedir)
            new_H.conf.set_listen_port(last_assigned_port + 1);
            new_conf_file_path = os.path.join(new_H.home, "infinit.conf")
            with open(new_conf_file_path, "w") as conffile:
                new_H.conf.commit(conffile)
            self.l_home.append(new_H)
            last_assigned_port = new_H.port

        # Fill the .set files
        l_addrs = ["127.0.0.1:%d" % i for i in range(self.low_port + 1,
            last_assigned_port + 1)]
        for h in self.l_home:
            h.add_peer([
                addr for addr in l_addrs if int(addr.split(':')[1]) != h.port
            ])

            # Lauch infinit
        for h in self.l_home:
            mountpoint_path = os.path.join(h.home, "mountpoint_{0}".format(h.network))
            os.mkdir(mountpoint_path)
            infinit = cli.Infinit(h, mountpoint_path)
            infinit.launch()
            # Wooh ! Don't start too fast !
            time.sleep(2)
            self.l_infinit_inst.append(infinit)

        # Aggregate the stderrs
        for inf in self.l_infinit_inst:
            self.stderrs.append(inf.stderr)

        return self

    def __exit__(self, type, value, trace):
        if trace != None:
            traceback.print_tb(trace)
            print(type)

        for I in self.l_infinit_inst:
            print("stop instance (pid={0}) {1}".format(I.pid, I))
            I.stop()

        # Is infinit still alive ..?
        if sp.call(["pgrep", "infinit"]) != 1:
            print("Something went really wrong: Infinit is still alive !!")

        # We do it after to be sure that all the instance are stoped
        for H in self.l_home:
            H.destroy()

        # If the mountpoint is still here, we unmount it
        for I in self.l_infinit_inst:
            if os.path.exists(I.mountpoint):
                sp.check_call(["fusermount", "-u", I.mountpoint])

        self.l_home = []
        self.l_infinit_inst = []
        return False
