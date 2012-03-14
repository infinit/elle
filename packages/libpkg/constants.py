# -*- encoding: utf-8 -*-

FARM_URI = 'https://farm.infinit.io'
FARM_USER = 'farm'
FARM_PASSWORD = 'iegh_aJ2'
FARM_BUILDS_URI = FARM_URI + '/releases/?C=M;O=D'

class Architectures:
    """Architecture list"""

    AMD64 = 1
    I386 = 2

    @classmethod
    def toString(cls, arch):
        return dict(
            (v, k) for k, v in cls.__dict__.items() if '__' not in k
        )[arch]

    @classmethod
    def toStrings(cls, archs):
        return [cls.toString(arch) for arch in archs]

class Platforms:
    """Platforms list"""

    LINUX = 1

    @classmethod
    def toString(cls, platform):
        return dict(
            (v, k) for k, v in cls.__dict__.items() if '__' not in k
        )[platform]

    @classmethod
    def toStrings(cls, platforms):
        return [cls.toString(platform) for platform in platforms]
