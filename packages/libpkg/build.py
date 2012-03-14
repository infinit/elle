# -*- encoding: utf-8 -*-

from libpkg import constants

class Build:
    """Build base class. Documents what subclasses' methods should implement,
    and their default behavior.
    """

    ##
    ## Methods to implement
    ##
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

    def getClientEnv(self, architecture, platform):
        """Returns a client environment for the targetted pair."""
        raise NotImplemented()


    ##
    ## Generic methods
    ##
    @property
    def architectures_strings(self):
        return constants.Architectures.toStrings(self.architectures)

    @property
    def platforms_strings(self):
        return constants.Platforms.toStrings(self.platforms)

