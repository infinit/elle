# -*- encoding: utf-8 -*-

"""
pages module provides all page class
"""

from meta.resources import root
from meta.resources import user
from meta.resources import device
from meta.resources import network

from meta.page import Page

_modules = [root, user, device, network,]

ALL = []

for module in _modules:
    for obj in module.__dict__.values():
        try:
            if issubclass(obj, Page) and obj != Page and obj.__name__[0] != '_':
                ALL.append(obj)
        except:
            pass
