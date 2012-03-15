# -*- encoding: utf-8 -*-

from libpkg import farm
from libpkg.farmbuild import FarmBuild
from libpkg.localbuild import LocalBuild

from libpkg.packagers import debian
from libpkg.packagers import archlinux

PACKAGERS = [
    debian.Packager,
    archlinux.Packager,
]
