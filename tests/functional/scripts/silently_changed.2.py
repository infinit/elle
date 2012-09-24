#!/usr/bin/env python3

import os

mnt1 = os.environ['MNT1']
mnt2 = os.environ['MNT2']

path1 = os.path.join(mnt1, 'pif')
path2 = os.path.join(mnt2, 'pif')

def write_serial(path1, path2):
    with open(path1, 'w') as f1:
        f1.write('bite')
    with open(path2, 'w')  as f2:
        f2.write('pif')

def write_interlaced(path1, path2):
    with open(path1, 'w') as f1:
        with open(path2, 'w')  as f2:
            f2.write('pif')
        f1.write('bite')

def write_interlaced_slow(path1, path2):
    import time
    with open(path1, 'w') as f1:
        time.sleep(2)
        with open(path2, 'w')  as f2:
            time.sleep(2)
            f2.write('pif')
            time.sleep(2)
        time.sleep(2)
        f1.write('bite')
        time.sleep(2)



def check_convergence(path1, path2):
    with open(os.path.join(mnt1, 'pif'), 'r') as f1:
        with open(os.path.join(mnt2, 'pif'), 'r')  as f2:
            data1, data2 = f1.read(), f2.read()
            if data1 != data2:
                raise Exception("'%s' != '%s'" % (data1, data2))

for test in [write_serial,
             write_interlaced,
             write_interlaced_slow]:
    try:
        test(path1, path2)
        check_convergence(path1, path2)
        print(test.__name__, "passes")
    except Exception as err:
        print("test '%s'" % test.__name__, "failed:", str(err))

