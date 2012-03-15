# -*- encoding: utf-8 -*-

class Package:

    def __init__(kind, file, path, architecture, platform):
        self.kind = kind
        self.file = file
        self.path = path
        self.architecture = architecture
        self.platform = platform

    def __str__(self):
        return "%(kind)s package for %(platform)s-%(architecture)s (%(file)s)" % {
            'kind': self.kind,
        }

