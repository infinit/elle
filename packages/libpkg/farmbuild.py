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

    def __init__(self, infos, hash, tarballs):
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

    class Env(BuildEnv):
        def __init__(self, build, architecture, platform, tarball):
            BuildEnv.__init__(self, build, architecture, platform)
            self._tarball = tarball
            self._release_dir = None
            self._dir = None

        def prepare(self):
            if self._dir is not None:
                return
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
                    print('\r - %s: %.2f' % (self._tarball, float(size) / (1024.0 * 1024.0)), 'MB', end='')
                    sys.stdout.flush()
                    f.write(data)
            print()


            print(' -', self._tarball + ": extracting")
            with tarfile.open(t) as archive:
                archive.extractall(self._dir)
            self._release_dir = os.path.join(self._dir, self._tarball[:-4])
            assert os.path.isdir(self._release_dir)

        def cleanup(self):
            if self._dir is not None:
                self.removeDirectory(self._dir)

        @property
        def is_client(self):
            return 'infinit-client' in self._tarball

        @property
        def is_server(self):
            return 'infinit-server' in self._tarball

        @property
        def directory(self):
            assert self._release_dir is not None
            return self._release_dir


    def prepareEnvList(self, architecture, platform):
        envlist = []
        for t in self._tarballs:
            if farm.getTarballArchitecture(t) == architecture and \
               farm.getTarballPlatform(t) == platform:
                envlist.append(self.Env(self, architecture, platform, t))
        return envlist

    def hasClientBuild(self, arch, platform):
        return any(farm.isClientTarball(t) for t in self._getEnvList(arch, platform))

