# -*- encoding: utf-8 -*-

import itertools
import os

from libpkg import constants
from libpkg.package import Package

class Packager:
    """Base class for every packager"""

    def __init__(self, name, extension, built_architectures, built_platforms):
        self._name = name
        self._extension = extension
        self._built_architectures = built_architectures
        self._built_platforms = built_platforms

    ##
    ## Generic methods
    ##

    @property
    def name(self):
        """The name of the packager."""
        return self._name

    @property
    def extension(self):
        """Returns the package file extension."""
        return self._extension
    @property
    def built_architectures(self):
        """Architectures in which the packager might be involved."""
        return self._built_architectures

    @property
    def built_platforms(self):
        """Platforms in which the packager might be involved."""
        return self._built_platforms

    @property
    def status(self):
        if self.is_available:
            return (
                "Available for " +
                ', '.join(self.available_platforms_strings) +
                " on " +
                ', '.join(self.available_architectures_strings)
            )
        return "Not available"

    @property
    def available_platforms(self):
        """The intersection between compatible platforms and available builds.
        """
        return list(
            set(self.compatible_platforms).intersection(self.built_platforms)
        )

    @property
    def available_platforms_strings(self):
        return constants.Platforms.toStrings(self.available_platforms)

    @property
    def available_architectures(self):
        """The intersection between compatible architectures and available builds.
        """
        return list(
            set(self.compatible_architectures).intersection(self.built_architectures)
        )

    @property
    def available_architectures_strings(self):
        return constants.Architectures.toStrings(self.available_architectures)

    def buildClientPackages(self, build, dest_dir='.'):
        """Try to package the client binaries with specified build. Returns the
        list for package files.
        """
        pairs = itertools.product(
            self.available_architectures,
            self.available_platforms
        )
        packages = []
        for architecture, platform in pairs:
            build_env = build.getClientEnv(architecture, platform)
            if build_env is None:
                print(
                    "Warning: cannot prepare " + self.name +
                    " package for (" +
                    constants.Architectures.toString(architecture) + ", " +
                    constants.Platforms.toString(platform) +
                    ")"
                )
                continue
            res = self.buildClientPackage(build_env, dest_dir)
            assert res is not None
            assert isinstance(res, str)
            path = os.path.join(dest_dir, res)
            assert os.path.isfile(path)
            packages.append(Package(
                kind=self.name,
                file=res,
                path=path,
                architecture=architecture,
                platform=platform,
            ))
        return packages

    ##
    ## To be implemented in subclasses
    ##

    @property
    def is_available(self):
        """Check wether or not the packager should work on this system."""
        return True

    @property
    def compatible_platforms(self):
        """Return the list of platforms that would be packaged."""
        return self.built_platforms

    @property
    def compatible_architectures(self):
        """Return the list of architectures that would be packaged."""
        return self.built_architectures

    def buildClientPackage(self, build_env, dest_dir):
        """Effectively build the package from the build env. Returns the
        name of the package file.
        """
        raise NotImplemented()
