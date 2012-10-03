#!/usr/bin/env python3
# -*- python -*-

import os
import copy
import shutil
import tempfile
import subprocess as sp

from . import cli
from . import conf
from . import infinitools

class Home:
    def __init__(self, auth, user, mode, network, home, conf):
        self.auth = auth
        self.user = user
        self.mode = mode
        self.network = network
        self.home = home
        self.conf = conf

    def __enter__(self):
        self.create()
        return self

    def __exit__(self, type, value, trace):
        pass

    def __repr__(self):
        return "Home [{home}]".format(**self.__dict__)

    @property
    def port(self):
        return int(self.conf.conf["hole"]["{mode}.port".format(
            mode=self.mode
            )])

    def create(self):
        auth = self.auth
        user = self.user
        mode = self.mode
        network = self.network
        conf = self.conf

        # Nota bene: This is not a powerpoint !
        if not os.path.exists("{home}/infinit.ppt".format(**self.__dict__)):
            print("creating a new passport file")
            cli.Passport.create(auth)

        if not os.path.exists("{home}/users/{user}".format(**self.__dict__)):
            print("creating user {0}".format(user))
            self.user_auth = cli.User.create(auth, user)
        else:
            self.user_auth = cli.User.fetch(auth, user)

        if not os.path.exists("{home}/networks/{network}".format(**self.__dict__)):
            print("creating network {0}".format(network))
            cli.Network.create(auth, self.user_auth, network, mode, user)


    def add_peer(self, addr):
        if "dotset" not in self.__dict__:
            self.dotset = open(os.path.join(self.home,
                "networks",
                self.network,
                "{0}.set".format(self.network)), "a")
        print(" ".join(addr))
        self.dotset.write(" ".join(addr))
        self.dotset.close()

    def duplicate(self, dst_dir):
        """
        This function do a deep copy of the infinit's home
        """
        print("copy", self.home, "in", dst_dir)
        shutil.copytree(self.home, dst_dir)
        new_H = Home(self.auth,
                     self.user,
                     self.mode,
                     self.network,
                     dst_dir,
                     self.conf.clone())
        new_H.user_auth = self.user_auth;
        return new_H

    def destroy(self):
        #print("removing home at ", self)
        try:
            shutil.rmtree(self.home)
        except OSError as e:
            print(e)
        print("home removed at ", self)

def gen(home_names, auth=None):
    if "INFINIT_HOMEDIR" in os.environ:
        homedir = os.environ["INFINIT_HOMEDIR"]
    else:
        homedir = tempfile.mkdtemp(prefix="infinit.")

    if not os.path.exists(os.path.join(homedir, "infinit.auth")):
        os.environ["INFINIT_HOME"] = homedir
        auth = cli.Authority.create()

    homes = []
    for h in home_names:
        home = tempfile.mkdtemp(prefix=h + ".", dir=homedir)
        os.environ["INFINIT_HOME"] = home
        shutil.copy(os.path.join(homedir, "infinit.auth"), home)
        H = Home(auth, "{username}_{suffix}".format(username="user", suffix=h),
            "slug", "mezzo", home)
        H.create()
        homes.append(H)
    return homes
