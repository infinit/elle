# -*- encoding: utf-8 -*-

import shutil
import tempfile

class BuildEnv:
    """Contract for BuildEnv-like classes.

    A build env is created by any build class in order to package a specific
    pair (architecture, platform). It gives access to files and ensures they
    are properly cleaned.

    This class also provides some tools in order to set up and clean up the
    temporary environment.

    """

    def __init__(self, architecture, platform):
        self.platform = platform
        self.architecture = architecture

    @property
    def directory(self):
        """Return the release root directory"""
        raise NotImplemented()

    def cleanup(self):
        """Remove tempary files."""
        raise NotImplemented()

    ##
    ## Tools
    ##
    @staticmethod
    def makeTemporaryDirectory():
        return tempfile.mkdtemp()

    @staticmethod
    def removeDirectory(path, ignore_errors=True):
        shutil.rmtree(path, ignore_errors=ignore_errors)
