#!/usr/bin/python

import drake, drake.cxx, drake.cxx.qt, subprocess, sys

i = int(sys.argv[1])

try:
    drake.set_srctree('.')
    cxx_tk = drake.cxx.GccToolkit()
    root = drake.raw_include('drakefile.py', cxx_tk)
    tgt = drake.Node.nodes['test']
    if i == 1:
        tgt.build()
        assert subprocess.check_output(['./test']) == 'test!\n'
    elif i == 2:
        with open('test.hh', 'w') as f:
            print >> f, '#define MESSAGE "updated"'
        tgt.build()
        assert subprocess.check_output(['./test']) == 'updated!\n'
except drake.Exception, e:
    print '%s: %s' % (sys.argv[0], e)
    exit(1)
