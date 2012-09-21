#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import os, sys, time

m1 = os.environ['MNT1']
m2 = os.environ['MNT2']

p1 = os.path.join(m1, 'pif')
p2 = os.path.join(m2, 'pif')

DATA = "And sunday comes afterward."

with open(p1, 'w') as f:
    f.write(DATA)

with open(p1, 'r') as f:
    assert f.read() == DATA

MAX_TRIES = 10

tries = 0
while tries < MAX_TRIES:
    if os.path.exists(p2):
        break
    tries += 1
    print("Waiting for the file to be replicated (%d sec)" % tries)
    time.sleep(1)

if tries == MAX_TRIES:
    raise("Maximum number of tries exceeded")

with open(p2, 'r') as f:
    assert f.read() == DATA
    os.unlink(p1)
    time.sleep(1)

assert not os.path.exists(p1)
assert not os.path.exists(p2)

