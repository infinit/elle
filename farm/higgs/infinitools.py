#!/usr/bin/env python3
# -*- python -*-

import cli
import os

import subprocess

def run_cmd(cmd, stdin=None):
    p = None
    if stdin == None:
        return subprocess.check_call(cmd)
    else:
        p = subprocess.Popen(cmd, stdin=stdin);
    return p

def patch_satellites_path():
    from pprint import pprint
    path = os.environ["PATH"]
    base_path = os.environ["INFINIT_ROOT"]
    new_paths = [path, base_path]
    for i in ("authority", "network", "passport", "user"):
        bin_name = "8{0}".format(i)
        bin_path = os.path.join(base_path, "satellites", i, bin_name)
        if os.path.exists(bin_path):
            new_paths.append(os.path.join(base_path, "satellites", i))
    os.environ["PATH"]= ":".join(new_paths)

