# -*- encoding: utf-8 -*-

from libpkg import constants

class Package:

    def __init__(self, kind, file, path, architecture, platform):
        self.kind = kind
        self.file = file
        self.path = path
        self.architecture = architecture
        self.platform = platform

    def __str__(self):
        return "%(kind)s package for %(platform)s-%(architecture)s (%(file)s)" % {
            'kind': self.kind,
            'platform': constants.Platforms.toString(self.platform),
            'architecture': constants.Architectures.toString(self.architecture),
            'file': self.file,
        }

