# -*- encoding: utf-8 -*-

"""Handle local builds
"""

import os

from libpkg import constants
from libpkg import tools
from libpkg.build import Build
from libpkg.buildenv import BuildEnv

class LocalBuild(Build):
    def __init__(self, infos, build_dir):
        Build.__init__(self, infos)
        self._build_dir = build_dir
        self._hash = 'HEAD'
        self._dir_to_platform = {
            'linux64':  constants.Platforms.LINUX,
            'linux32':  constants.Platforms.LINUX,
            'macosx32': constants.Platforms.MACOSX,
            'macosx64': constants.Platforms.MACOSX,
            'win32':    constants.Platforms.WINDOWS,
            'win64':    constants.Platforms.WINDOWS,
        }
        self._dir_to_arch = {
            'linux32': constants.Architectures.I386,
            'linux64': constants.Architectures.AMD64,
            'macosx32': constants.Architectures.I386,
            'macosx64': constants.Architectures.AMD64,
            'win32': constants.Architectures.I386,
            'win64': constants.Architectures.AMD64,
        }
        self._environments = None
        # XXX to be improved !
        self.client_date = "now"
        self.server_date = "now"

    @property
    def is_available(self):
        return (
            tools.which('patchelf') is not None and
            tools.which('ldd') is not None
        )


    @property
    def hash(self):
        return self._hash

    @property
    def has_client(self):
        return True

    @property
    def has_server(self):
        return True

    @property
    def architectures(self):
        return list(
            set(self._dir_to_arch[d] for d in os.listdir(self._build_dir) if d in self._dir_to_arch)
        )

    @property
    def platforms(self):
        return list(
            set(self._dir_to_platform[d] for d in os.listdir(self._build_dir) if d in self._dir_to_platform)
        )

    class Env(BuildEnv):
        def __init__(self, build, architecture, platform, build_dir, type_):
            BuildEnv.__init__(self, build, architecture, platform)
            self._build_dir = build_dir
            self._dir = None
            assert type_ in ('client', 'server')
            self._type = type_

        def prepare(self):
            if self._dir is not None:
                return
            self._dir = self.makeTemporaryDirectory()
            res = os.system('"%(script)s" "%(build_dir)s" "%(dest_dir)s"' % {
                'script': constants.PREPARE_LOCAL_BUILD_SCRIPT,
                'build_dir': self._build_dir,
                'dest_dir': self._dir,
            })
            if res != 0:
                raise Exception("Cannot create the install dir of `%s'" % self._build_dir)
            self._release_dir = os.path.join(self._dir, self._type)
            assert(os.path.isdir(self._release_dir))

        @property
        def directory(self): return self._release_dir

        @property
        def is_client(self): return self._type == 'client'

        @property
        def is_server(self): return self._type == 'server'

        def cleanup(self):
            if self._dir is not None:
                self.removeDirectory(self._dir)

    def prepareEnvList(self, architecture, platform):
        """Returns a client environment for the targetted combination."""
        envlist = []
        for d in os.listdir(self._build_dir):
            if d not in self._dir_to_arch:
                continue
            if self._dir_to_arch[d] == architecture and self._dir_to_platform[d] == platform:
                path = os.path.join(self._build_dir, d)
                envlist.append(self.Env(self, architecture, platform, path, 'client'))
                envlist.append(self.Env(self, architecture, platform, path, 'server'))
        return envlist

    def hasClientBuild(self, arch, platform):
        return any(
            (
                self._dir_to_arch[d] == architecture and
                self._dir_to_platform[d] == platform
            )
            for d in os.listdir(self._build_dir)
            if d in self._dir_to_arch
        )
