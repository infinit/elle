# -*- encoding: utf-8 -*-

import os
import sys
import tarfile

from libpkg import farm
from libpkg.build import Build
from libpkg.buildenv import BuildEnv

class FarmBuild(Build):
    """Manipulate builds from the buildfarm
    """

    def __init__(self, infos, tarballs):
        Build.__init__(self, infos)
        self._hash = hash
        self._tarballs = tarballs
        self._environments = None

    @property
    def is_available(self): return True

    @property
    def hash(self): return self._hash

    @property
    def has_client(self):
        return any(farm.isClientTarball(t) for t in self._tarballs)

    @property
    def has_server(self):
        return any(farm.isServerTarball(t) for t in self._tarballs)

    @property
    def architectures(self):
        return list(
            set(farm.getTarballArchitecture(t) for t in self._tarballs)
        )

    @property
    def platforms(self):
        return list(
            set(farm.getTarballPlatform(t) for t in self._tarballs)
        )

    class ClientEnv(BuildEnv):
        def __init__(self, build, architecture, platform, tarball):
            BuildEnv.__init__(self, build, architecture, platform)
            self._tarball = tarball
            self._release_dir = None
            self._dir = self.makeTemporaryDirectory()
            dl = farm.downloadTarball(self._tarball)
            t = os.path.join(self._dir, self._tarball)

            size = 0
            with open(t, 'wb') as f:
                while True:
                    data = dl.read(4096 * 4)
                    if not data:
                        break
                    size += len(data)
                    print('\r * %s: %.2f' % (self._tarball, float(size) / (1024.0 * 1024.0)), 'MB', end='')
                    sys.stdout.flush()
                    f.write(data)
            print()

            with tarfile.open(t) as archive:
                archive.extractall(self._dir)
            print('Temp env:', self._dir)
            self._release_dir = os.path.join(self._dir, self._tarball[:-4])
            assert os.path.isdir(self._release_dir)

        @property
        def directory(self):
            assert self._release_dir is not None
            return self._release_dir

        def cleanup(self):
            self.removeDirectory(self._dir)

    def getClientEnv(self, architecture, platform):
        """Returns a client environment for the targetted combination."""
        assert architecture in self.architectures
        assert platform in self.platforms
        assert self._environments is not None # can only be called in a with clause
        env = self._environments.get((architecture, platform))
        if env is None:
            for t in self._tarballs:
                if farm.isClientTarball(t) and \
                   farm.getTarballArchitecture(t) == architecture and \
                   farm.getTarballPlatform(t) == platform:
                    env = self.ClientEnv(self, architecture, platform, t)
                    break
            assert env is not None
            self._environments[(architecture, platform)] = env
        return env

    def hasClientBuild(self, arch, platform):
        return any(
            (
                farm.isClientTarball(t) and
                farm.getTarballArchitecture(t) == architecture and
                farm.getTarballPlatform(t) == platform
            ) for t in self._tarballs
        )

    def __enter__(self):
        """Enters in a 'with' clause"""
        self._environments = {}

    def __exit__(self, type, value, traceback):
        """exits the 'with' clause"""
        for env in self._environments.values():
            env.cleanup()
        self._environments = None

