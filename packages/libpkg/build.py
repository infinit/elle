# -*- encoding: utf-8 -*-

from libpkg import constants

class Build:
    """Build base class. Documents what subclasses' methods should implement,
    and their default behavior.
    """

    def __init__(self, infos):
        assert isinstance(infos, dict)
        self._infos = infos

    @property
    def infos(self): return self._infos

    ##
    ## Generic methods
    ##
    @property
    def architectures_strings(self):
        return constants.Architectures.toStrings(self.architectures)

    @property
    def platforms_strings(self):
        return constants.Platforms.toStrings(self.platforms)

    def getClientEnv(self, architecture, platform):
        """Returns a client environment for the targetted combination."""
        for e in self._getEnvList(architecture, platform):
            if e.is_client:
                return e

    def getServerEnv(self):
        """Returns a server environment for the targetted combination."""
        for e in self._getEnvList(self.infos['server_architecture'], self.infos['server_platform']):
            if e.is_server:
                return e

    def _getEnvList(self, architecture, platform):
        assert architecture in self.architectures
        assert platform in self.platforms
        assert self._environments is not None # can only be called in a with clause
        envlist = self._environments.get((architecture, platform))
        if envlist is None:
            envlist = self.prepareEnvList(architecture, platform)
            assert isinstance(envlist, list)
            self._environments[(architecture, platform)] = envlist
        return envlist


    def __enter__(self):
        """Enters in a 'with' clause"""
        self._environments = {}

    def __exit__(self, type, value, traceback):
        """exits the 'with' clause"""
        for envlist in self._environments.values():
            for env in envlist:
                env.cleanup()
        self._environments = None

    ##
    ## Methods to implement in subclasses
    ##
    @property
    def is_available(self):
        """Check if the builder should work on this platform."""
        raise NotImplemented()

    @property
    def has_client(self):
        """Are the client binaries present ?"""
        raise NotImplemented()

    @property
    def has_server(self):
        """Are the server binaries present ?"""
        raise NotImplemented()

    @property
    def architectures(self):
        """The list of available architectures (see constants.Architectures)"""
        raise NotImplemented()

    @property
    def platforms(self):
        """The list of available platforms (see constants.Platforms)"""
        raise NotImplemented()

    def prepareEnvList(self, architecture, platform):
        """Returns environments for the targetted pair."""
        raise NotImplemented()

