# -*- encoding: utf-8 -*-

from libpkg import farm
from libpkg.farmbuild import FarmBuild

from libpkg.packagers import debian

PACKAGERS = [
    debian.Packager
]
