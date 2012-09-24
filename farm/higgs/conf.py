#!/usr/bin/env python3
# -*- python -*-

import sys
import configparser
import copy

class Conf:
    def __init__(self):
        self._conf = configparser.ConfigParser()
        self._conf["hole"] = { }

    def set_listen_port(self, port, mode="slug"):
        self._conf["hole"]["{0}.port".format(mode)] = str(port)

    def set_timeout(self, timeout, mode="slug"):
        self._conf["hole"]["{0}.timeout".format(mode)] = str(timeout)

    @property
    def conf(self):
        return self._conf

    def dump(self):
        import pprint
        pprint.pprint(self._conf)

    def commit(self, file=sys.stdout):
        self.conf.write(file)

    def clone(self):
        new_c = Conf()
        for (key, val) in self._conf.items():
            new_c._conf[key] = val
        return new_c


if __name__ == "__main__":
    c = Conf()
    c.set_timeout(1000)
    c.set_listen_port(5656)
    c.commit()


