# -*- encoding: utf-8 -*-

from libpkg import farm
from libpkg.farmbuild import FarmBuild
from libpkg.localbuild import LocalBuild

from libpkg.packagers import archive
from libpkg.packagers import archlinux
from libpkg.packagers import debian
from libpkg.packagers import macosx

PACKAGERS = [
    debian.Packager,
    archlinux.Packager,
    macosx.Packager,
    archive.Packager,
]
