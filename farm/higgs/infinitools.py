#!/usr/bin/env python3
# -*- python -*-

import os
import cli
import subprocess as sp

def run_cmd(cmd, stdin=None):
    p = None
    if stdin == None:
        return sp.check_call(cmd)
    else:
        p = sp.Popen(cmd, stdin=stdin);
    return p

def patch_satellites_path(dir=None):

    path = os.environ["PATH"]
    if dir == None:
        base_path = os.environ["INFINIT_BUILD"]
    else:
        base_path = dir

    new_paths = [path, base_path]
    for i in ("authority", "network", "passport", "user"):
        bin_name = "8{0}".format(i)
        bin_path = os.path.join(base_path, "satellites", i, bin_name)
        if os.path.exists(bin_path):
            new_paths.append(os.path.join(base_path, "satellites", i))
    os.environ["PATH"]= ":".join(new_paths)

def render_mail_template(stats, templatefile):
    import moody
    with open(templatefile, "r") as f:
        template = f.read()
        result = moody.render(template, it=1, **stats)
        return result

if __name__ ==  "__main__":
    with open("report", "r") as reportfile:
        stats = eval(reportfile.read())
        print(render_mail_template(stats, "mail_template.in"))
