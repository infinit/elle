# -*- encoding: utf-8 -*-

import os
import shutil
import tarfile

from libpkg import constants
from libpkg import tools
from libpkg.buildenv import BuildEnv
from libpkg.packager import Packager as BasePackager


class Packager(BasePackager):
    """Archive packager.
    """

    def __init__(self, built_architectures, built_platforms):
        BasePackager.__init__(
            self, "Archive", 'tbz',
            built_architectures, built_platforms
        )


    def buildClientPackage(self, build_env, dest_dir):
        params = {
            'architecture': {
                constants.Architectures.AMD64: 'x86_64',
                constants.Architectures.I386: 'i386',
            }[build_env.architecture],
            'version_name': build_env.build.infos['version_name'],
            'version': build_env.build.infos['version'],
            'extension': self.extension,
        }
        filename = "infinit-%(version_name)s-%(version)s-%(architecture)s.%(extension)s" % params
        path = os.path.join(dest_dir, filename)
        with tarfile.open(path, "w:bz2") as tar:
            tar.add(build_env.directory, arcname='client')
        return filename
