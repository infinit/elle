#!/usr/bin/python

import drake, drake.cxx, drake.cxx.qt, sys

try:
    drake.set_srctree('.')
    cxx_tk = drake.cxx.GccToolkit()
    qt = drake.cxx.qt.Qt()
    root = drake.raw_include('drakefile.py', cxx_tk, qt)
    tgt = drake.Node.nodes['test']
#    tgt.clean()
    tgt.build()
    drake.dot()
except drake.Exception, e:
    print '%s: %s' % (sys.argv[0], e)
    exit(1)
