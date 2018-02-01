#!/usr/bin/env python3

import drake, os.path, sys

i = int(sys.argv[1])

print(i)

try:
    drake.set_srctree('.')
    root = drake.raw_include('drakefile.py')
    tgt = drake.Node.nodes['tgt']

    if i == 1:
        if os.path.exists(str(tgt)):
            os.remove(str(tgt))
        assert not os.path.exists(str(tgt))
        assert not root.run
        # Check it's built
        tgt.build()
        assert os.path.exists(str(tgt))
        assert root.run
    elif i == 2:
        # Check it's not rebuilt
        tgt.build()
        assert os.path.exists(str(tgt))
        assert not root.run
        os.remove(str(tgt))
except drake.Exception as e:
    print('%s: %s' % (sys.argv[0], e))
    exit(1)
