#!/usr/bin/python

import drake, os.path, sys, __builtin__

i = int(sys.argv[1])

try:
    drake.set_srctree('.')
    root = drake.raw_include('drakefile.py')
    dst = drake.Node.nodes['dst']

    if i == 1:
        if os.path.exists(str(dst)):
            os.remove(str(dst))
        with open('src', 'w') as file:
            print >> file, 'foo'
        # Check it's built
        dst.build()
        assert root.run
        assert root.deps
        assert os.path.exists(str(dst))
    elif i == 2:
        # Check it's not rebuilt
        dst.build()
        assert not root.run
        assert not root.deps
    elif i == 3:
        # Check it's rebuilt
        with open('src', 'w') as file:
            print >> file, 'bar'
        dst.build()
        assert root.run
        assert root.deps
except drake.Exception, e:
    print '%s: %s' % (sys.argv[0], e)
    exit(1)
