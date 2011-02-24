# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

from . import VirtualNode, cmd_output

class GitVersion(VirtualNode):

    def __init__(self):

        VirtualNode.__init__(self, 'git/version')
        self._author_date = None
        self._revision    = None
        self._description = None
        self._revision    = None
        self._subject     = None

    def hash(self):

        return cmd_output('git rev-parse HEAD')[:-1]

    def author_date(self):
        if self._author_date is None:
            self._author_date = cmd_output('git log --pretty=format:%%ai HEAD^..HEAD')
        return self._author_date

    def description(self):

        if self._description is None:
            self._description = cmd_output('git describe')[:-1]
        return self._description

    def revision(self):

        if self._revision is None:
            self._revision = cmd_output('git log --pretty=format:%%h HEAD^..HEAD')
        return self._revision

    def subject(self):

        if self._subject is None:
            self._subject = cmd_output('git log --pretty=format:%%s HEAD^..HEAD')
        return self._subject

    def __iter__(self):

        yield ('git-author-date',  self.author_date())
        yield ('git-description',  self.description())
        yield ('git-revision', self.revision())
