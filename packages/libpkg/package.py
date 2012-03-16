# -*- encoding: utf-8 -*-

from libpkg import constants

class Package:

    def __init__(self, kind, file_, path, architecture, platform, type_):
        self.kind = kind
        self.file_ = file_
        self.path = path
        self.architecture = architecture
        self.platform = platform
        self.type_ = type_

    def __str__(self):
        return "%(kind)s %(type)s package for %(platform)s-%(architecture)s (%(file)s)" % {
            'kind': self.kind,
            'type': self.type_,
            'platform': constants.Platforms.toString(self.platform),
            'architecture': constants.Architectures.toString(self.architecture),
            'file': self.file_,
        }

