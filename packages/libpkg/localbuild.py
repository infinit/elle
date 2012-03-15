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

    @property
    def is_available(self):
        return tools.which('patchelf') is not None

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

    class ClientEnv(BuildEnv):
        def __init__(self, build, architecture, platform, build_dir):
            BuildEnv.__init__(self, build, architecture, platform)
            self._dir = self.makeTemporaryDirectory()
            res = os.system('"%(script)s" "%(build_dir)s" "%(dest_dir)s"' % {
                'script': constants.PREPARE_LOCAL_BUILD_SCRIPT,
                'build_dir': build_dir,
                'dest_dir': self._dir,
            })
            if res != 0:
                raise Exception("Cannot create the install dir of `%s'" % build_dir)
            self._release_dir = os.path.join(self._dir, 'client')
            assert(os.path.isdir(self._release_dir))

        @property
        def directory(self): return self._release_dir

        def cleanup(self):
            self.removeDirectory(self._dir)

    def getClientEnv(self, architecture, platform):
        """Returns a client environment for the targetted combination."""
        assert architecture in self.architectures
        assert platform in self.platforms
        assert self._environments is not None # can only be called in a with clause
        env = self._environments.get((architecture, platform))
        if env is None:
            for d in os.listdir(self._build_dir):
                if d not in self._dir_to_arch:
                    continue
                if self._dir_to_arch[d] == architecture and self._dir_to_platform[d] == platform:
                    path = os.path.join(self._build_dir, d)
                    env = self.ClientEnv(self, architecture, platform, path)
                    break
            assert env is not None
            self._environments[(architecture, platform)] = env
        return env

    def hasClientBuild(self, arch, platform):
        return any(
            (
                self._dir_to_arch[d] == architecture and
                self._dir_to_platform[d] == platform
            )
            for d in os.listdir(self._build_dir)
            if d in self._dir_to_arch
        )

    def __enter__(self):
        """Enters in a 'with' clause"""
        self._environments = {}

    def __exit__(self, type, value, traceback):
        """exits the 'with' clause"""
        for env in self._environments.values():
            env.cleanup()
        self._environments = None
