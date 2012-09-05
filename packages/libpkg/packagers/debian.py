# -*- encoding: utf-8 -*-

import os
import shutil

from libpkg import constants
from libpkg import tools
from libpkg.buildenv import BuildEnv
from libpkg.packager import Packager as BasePackager

class Packager(BasePackager):
    """Debian packager.
    """
    _control_template = """
Package: infinit-%(version_name)s
Version: %(version)s
Section: otherosfs
Priority: optional
Architecture: %(architecture)s
Installed-Size: %(installed_size)d
Essential: no
Depends: libfuse2, python3 (>= 3.2), libpython3 (>= 3.2)
Maintainer: Infinit.io <debian@infinit.io>
Description: Provide a secure, distributed and cross-platform filesystem.
"""

    def __init__(self, built_architectures, built_platforms):
        BasePackager.__init__(
            self, "Debian", 'deb',
            built_architectures, built_platforms
        )

    @property
    def is_available(self):
        return tools.which('dpkg') is not None

    @property
    def compatible_platforms(self):
        return [constants.Platforms.LINUX]

    def buildClientPackage(self, build_env, dest_dir):
        tempdir = BuildEnv.makeTemporaryDirectory()
        try:
            pkgdir = os.path.join(tempdir, 'pkg')
            os.mkdir(pkgdir)
            infinit_dir = os.path.join(pkgdir, 'opt/infinit')

            for dir_ in ['lib', 'bin', 'python']:
                shutil.copytree(
                    os.path.join(build_env.directory, dir_),
                    os.path.join(infinit_dir, dir_)
                )
            link_dir = os.path.join(pkgdir, 'usr/bin')
            os.makedirs(link_dir)
            os.symlink(
                os.path.relpath(
                    os.path.join(infinit_dir, 'bin', 'infinit-cli'),
                    start=link_dir
                ),
                os.path.join(link_dir, 'infinit')
            )

            debian_dir = os.path.join(pkgdir, 'DEBIAN')
            os.mkdir(debian_dir)
            params = {
                'architecture': {
                    constants.Architectures.AMD64: 'amd64',
                    constants.Architectures.I386: 'i386',
                }[build_env.architecture],
                'version_name': build_env.build.infos['version_name'],
                'version': build_env.build.infos['version'],
                'installed_size': build_env.total_size / 1024,
            }
            with open(os.path.join(debian_dir, "control"), 'w') as f:
                f.write(self._control_template % params)

            filename = "infinit-%(version_name)s-%(version)s-%(architecture)s.deb" % params
            path = os.path.join(dest_dir, filename)
            os.system('dpkg -b "%s" "%s"' % (pkgdir, filename))
            return filename
        finally:
            BuildEnv.removeDirectory(tempdir)
