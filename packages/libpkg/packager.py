# -*- encoding: utf-8 -*-

import itertools
import os

from libpkg import constants

class Packager:
    """Base class for every packager"""

    def __init__(self, built_architectures, built_platforms):
        self._built_architectures = built_architectures
        self._built_platforms = built_platforms

    ##
    ## Generic methods
    ##
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
            filename = (
                'infinit-client-' +
                constants.Architectures.toString(architecture).lower() +
                '.' + self.package_extension
            )
            path = os.path.join(dest_dir, filename)
            self.buildClientPackage(build_env, path)
            packages.append(path)
        return packages

    ##
    ## To be implemented
    ##
    @property
    def name(self):
        """The name of the packager."""
        raise NotImplemented()

    @property
    def package_extension(self):
        """Returns the package file extension."""
        raise NotImplemented()

    @property
    def is_available(self):
        """Check wether or not the packager will work on this system."""
        raise NotImplemented()

    @property
    def compatible_platforms(self):
        """Return the list of platforms that would be packaged."""
        raise NotImplemented()

    @property
    def compatible_architectures(self):
        """Return the list of architectures that would be packaged."""
        raise NotImplemented()

    def buildClientPackage(self, build_env, dest_dir):
        """Effectively build the package from the build env. Returns the
        name of the package file.
        """
        raise NotImplemented()
