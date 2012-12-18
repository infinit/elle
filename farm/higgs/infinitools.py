#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import os
import subprocess as sp

from collections import OrderedDict

def run_cmd(cmd, stdin=None):
    p = None
    print("run", cmd)
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

    new_paths = [path, os.path.join(base_path, "bin")]
    os.environ["PATH"]= ":".join(new_paths)

def render_mail_template(stats, templatefile):
    import moody
    with open(templatefile, "r") as f:
        template = f.read()
        result = moody.render(template, it=1, dict=stats, **stats)
        return result

if __name__ ==  "__main__":
    with open("report", "r") as reportfile:
        stats = eval(reportfile.read())
        print(render_mail_template(stats, "mail_template.in"))
