#!/usr/bin/python

import drake, os.path, sys

i = int(sys.argv[1])

try:
    drake.set_srctree('.')
    root = drake.raw_include('drakefile.py')
    dst = drake.Node.nodes['dst']
    src = drake.Node.nodes['src']

    if i == 1:
        if os.path.exists(str(dst)):
            os.remove(str(dst))
        with open(str(src), 'w') as f:
            print >> f, 'foo'
        assert not os.path.exists(str(dst))
        assert not root.run
        # Check it's built
        dst.build()
        assert os.path.exists(str(dst))
        assert root.run
    elif i == 2:
        # Check it's not rebuilt
        dst.build()
        assert not root.run
    elif i == 3:
        # Check it's rebuilt
        with open(str(src), 'w') as f:
            print >> f, 'bar'
        dst.build()
        assert root.run
        os.remove(str(dst))
except drake.Exception, e:
    print '%s: %s' % (sys.argv[0], e)
    exit(1)
