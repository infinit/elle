#!/usr/bin/python

import drake, drake.cxx, drake.cxx.qt, sys

try:
    drake.set_srctree('.')
    cxx_tk = drake.cxx.GccToolkit()
    root = drake.raw_include('drakefile.py', cxx_tk)
    tgt = drake.Node.nodes['test']
    tgt.clean()
    tgt.build()
except drake.Exception, e:
    print '%s: %s' % (sys.argv[0], e)
    exit(1)
