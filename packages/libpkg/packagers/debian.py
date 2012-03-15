# -*- encoding: utf-8 -*-

import os
import shutil

from libpkg import constants
from libpkg import tools
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
        return tools.which('dpkg') is not None

    @property
    def compatible_platforms(self):
        return [constants.Platforms.LINUX]

    @property
    def compatible_architectures(self):
        return self.built_architectures

    def buildClientPackage(self, build_env, dest_dir):
        tempdir = BuildEnv.makeTemporaryDirectory()
        try:
            pkgdir = os.path.join(tempdir, 'pkg')
            shutil.copytree(build_env.directory, pkgdir)
            # XXX copy only what is needed
            os.system('rm -rf "%s"/lib' % pkgdir)
            os.system('rm -f "%s"/manifest.xml' % pkgdir)
            debian_dir = os.path.join(pkgdir, 'DEBIAN')
            os.mkdir(debian_dir)
            params = {
                'architecture': {
                    constants.Architectures.AMD64: 'amd64',
                    constants.Architectures.I386: 'i386',
                }[build_env.architecture],
                'version_name': build_env.build.infos['version_name'],
                'version': build_env.buid.infos['version'],
            }
            with open(os.path.join(debian_dir, "control"), 'w') as f:
                f.write(self._control_template % params)
            filename = "infinit-%(version_name)s-%(version)s-%(architecture)s.deb" % params
            path = os.path.join(dest_dir, filename)
            os.system('dpkg -b "%s" "%s"' % (pkgdir, filename))
            return filename
        finally:
            BuildEnv.removeDirectory(tempdir)
