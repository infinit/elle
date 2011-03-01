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
        self.__author_date = None
        self.__revision    = None
        self.__description = None
        self.__version    = None
        self.__message     = None

    def hash(self):

        return cmd_output('git rev-parse HEAD')[:-1]

    def author_date(self):
        if self.__author_date is None:
            self.__author_date = cmd_output('git log --pretty=format:%%ai -n 1')
        return self.__author_date

    def description(self):

        if self.__description is None:
            self.__description = cmd_output('git describe')[:-1]
        return self.__description

    def version(self):

        if self.__version is None:
            self.__version = cmd_output('git describe --long')[:-1]
        return self.__version

    def revision(self):

        if self.__revision is None:
            self.__revision = cmd_output('git log --pretty=format:%%h -n 1')
        return self.__revision

    def message(self):

        if self.__message is None:
            self.__message = cmd_output('git log --pretty=format:%%s -n 1')
        return self.__message

    def __iter__(self):

        yield ('git-author-date',  self.author_date())
        yield ('git-description',  self.description())
        yield ('git-version',      self.version())
        yield ('git-revision',     self.revision())
        yield ('git-message',      self.message())
