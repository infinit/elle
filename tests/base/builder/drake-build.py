#!/usr/bin/python

import drake, os.path, sys


try:
    drake.set_srctree('.')
    root = drake.raw_include('drakefile.py')
    tgt = drake.Node.nodes['tgt']
    # Remove stray target file
    if os.path.exists(str(tgt)):
        os.remove(str(tgt))
    assert not os.path.exists(str(tgt))
    assert not drake.run
    # Check it's built
    tgt.build()
    assert os.path.exists(str(tgt))
    assert drake.run
    # Check it's not rebuilt
    drake.run = False
    tgt.build()
    assert os.path.exists(str(tgt))
    assert not drake.run
    os.remove(str(tgt))
except drake.Exception, e:
    print '%s: %s' % (sys.argv[0], e)
    exit(1)
