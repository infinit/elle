# -*- encoding: utf-8 -*-

import os
import shutil

from libpkg import constants
from libpkg.buildenv import BuildEnv
from libpkg.packager import Packager

class Packager(Packager):
    """Debian packager.
    """
    _control_template = """
Package: infinit
Version: 1.0
Section: otherosfs
Priority: optional
Architecture: %(architecture)s
Essential: no
Depends: libqt4-core (>= 4:4.7), libqt4-network, libqt4-xml, libqt4-gui,
         libssl1.0.0, libfuse2, libqjson0, libreadline5
Maintainer: Infinit.io <debian@infinit.io>
Description: Provide a secure, distributed and cross-platform filesystem.
"""

    @property
    def name(self): return "Debian"

    @property
    def package_extension(self): return 'deb'

    @property
    def is_available(self):
        # XXX and patchelf
        return os.system('which dpkg') == 0

    @property
    def compatible_platforms(self):
        return [constants.Platforms.LINUX]

    @property
    def compatible_architectures(self):
        return self.built_architectures

    def buildClientPackage(self, build_env, filename):
        tempdir = BuildEnv.makeTemporaryDirectory()
        try:
            pkgdir = os.path.join(tempdir, 'pkg')
            shutil.copytree(build_env.directory, pkgdir)
            # XXX copy only what is needed
            os.system('rm -rf "%s"/lib' % pkgdir)
            os.system('rm -f "%s"/manifest.xml' % pkgdir)
            debian_dir = os.path.join(pkgdir, 'DEBIAN')
            os.mkdir(debian_dir)
            with open(os.path.join(debian_dir, "control"), 'w') as f:
                f.write(self._control_template % {
                    'architecture': {
                        constants.Architectures.AMD64: 'amd64',
                        constants.Architectures.I386: 'i386',
                    }[build_env.architecture]
                })
            os.system('dpkg -b "%s" "%s"' % (pkgdir, filename))
        finally:
            BuildEnv.removeDirectory(tempdir)
