import os

def check_convergence(path1, path2):
    with open(path1, 'r') as f1:
        with open(path2, 'r')  as f2:
            data1, data2 = f1.read(), f2.read()
            if data1 != data2:
                raise Exception("'%s' != '%s'" % (data1, data2))
