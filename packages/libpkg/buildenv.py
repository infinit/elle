# -*- encoding: utf-8 -*-

import xml.etree.ElementTree as etree

import os
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

    def __init__(self, build, architecture, platform):
        self.build = build
        self.platform = platform
        self.architecture = architecture

    @property
    def directory(self):
        """Return the release root directory"""
        raise NotImplemented()

    def cleanup(self):
        """Remove tempary files."""
        raise NotImplemented()

    def prepare(self):
        """Prepare the environment (may be called multiple times)."""
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

    @property
    def total_size(self):
        manifest = os.path.join(self.directory, 'manifest.xml')
        tree = etree.parse(manifest)
        root = tree.getroot()
        try:
            return int(root.attrib['size'])
        except:
            # XXX
            return 42
