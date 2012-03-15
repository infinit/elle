# -*- encoding: utf-8 -*-


import os
import shutil

from libpkg import constants
from libpkg import tools
from libpkg.buildenv import BuildEnv
from libpkg.packager import Packager as BasePackager

class Packager(BasePackager):
    """Archlinux packager.
    """
    _PKGBUILD_template = """
# $Id$
# Maintainer: Infinit.io <archlinux@infinit.io>

pkgname=infinit
pkgver=%(version_name)s
pkgrel=%(version)s
pkgdesc="Infinit is a peer-to-peer file system"
arch=("%(architecture)s")
url="http://www.infinit.io"
license=('copyright 2011, 2012 Infinit.io')
depends=('qt' 'openssl' 'qjson' 'readline' 'curl' 'fuse')
source=()
sha1sums=()

build() {
    echo "Build! %(updater_bin)s"
}

package() {
  cp %(updater_bin)s $pkgdir/infinit
  chmod +x $pkgdir/infinit
}
"""

    def __init__(self, built_architectures, built_platforms):
        BasePackager.__init__(
            self, "Archlinux", 'pkg.tar.xz',
            built_architectures, built_platforms
        )

    @property
    def is_available(self):
        return tools.which('makepkg') is not None

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
            os.mkdir(pkgdir)
            params = {
                'architecture': {
                    constants.Architectures.AMD64: 'x86_64',
                    constants.Architectures.I386: 'i386',
                }[build_env.architecture],
                'updater_bin': os.path.join(build_env.directory, 'bin', '8updater'),
                'version_name': build_env.build.infos['version_name'],
                'version': build_env.build.infos['version'],
            }
            with open(os.path.join(pkgdir, "PKGBUILD"), 'w') as f:
                f.write(self._PKGBUILD_template % params)
            os.system('cd "%s" && makepkg -s' % pkgdir)
            filename = "infinit-%(version_name)s-%(version)s-%(architecture)s.pkg.tar.xz" % params
            shutil.copyfile(
                os.path.join(pkgdir, filename),
                os.path.join(dest_dir, filename)
            )
            return filename
        finally:
            BuildEnv.removeDirectory(tempdir)
