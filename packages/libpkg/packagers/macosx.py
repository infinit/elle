# -*- encoding: utf-8 -*-

import os
import shutil

from libpkg import constants
from libpkg import tools
from libpkg.buildenv import BuildEnv
from libpkg.packager import Packager as BasePackager

class Packager(BasePackager):
    """MacOSX dmg packager.
    """

    def __init__(self, built_architectures, built_platforms):
        BasePackager.__init__(
            self, "macosx", 'dmg',
            built_architectures, built_platforms
        )

    @property
    def is_available(self):
        return tools.which('hdiutil') is not None

    @property
    def compatible_platforms(self):
        return [constants.Platforms.MACOSX]

    @property
    def compatible_architectures(self):
        return self.built_architectures

    def buildClientPackage(self, build_env, dest_dir):
        tempdir = BuildEnv.makeTemporaryDirectory()
        try:
            pkgdir = os.path.join(tempdir, 'pkg')
            os.mkdir(pkgdir)
            shutil.copyfile(
                os.path.join(build_env.directory, 'bin', '8updater'),
                os.path.join(pkgdir, 'infinit'),
            )
            params = {
                'architecture': {
                    constants.Architectures.AMD64: 'x86_64',
                    constants.Architectures.I386: 'i386',
                }[build_env.architecture],
                'updater_bin': os.path.join(build_env.directory, 'bin', '8updater'),
                'version_name': build_env.build.infos['version_name'],
                'version': build_env.build.infos['version'],
            }
            filename = "infinit-%(version_name)s-%(version)s-%(architecture)s.dmg" % params
            path = os.path.join(tempdir, filename)

            res = os.system('hdiutil create "%(dmgfile)s" -volname "%(window_title)s" -fs HFS+ -srcfolder "%(pkgdir)s"' % {
                'dmgfile': path,
                'window_title': 'Infinit.io',
                'pkgdir': pkgdir,
            })
            if res != 0:
                raise Exception("Couldn't create the dmg file")

            shutil.copyfile(
                os.path.join(tempdir, filename),
                os.path.join(dest_dir, filename)
            )
            return filename
        finally:
            BuildEnv.removeDirectory(tempdir)
