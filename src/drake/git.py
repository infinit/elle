# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

from datetime import date
import drake
import os
from . import VirtualNode, cmd_output, Path

class Git(VirtualNode):

    """An iterable node with information on a git repository.

    # With the following dummy git repository.
    >>> git = 'git'
    >>> os.chdir('/tmp')
    >>> path = Path('.drake.git')
    >>> path.remove()
    >>> os.environ['GIT_AUTHOR_NAME'] = 'mefyl'
    >>> os.environ['GIT_AUTHOR_EMAIL'] = 'mefyl@gruntech.org'
    >>> if 'GIT_DIR' in os.environ:
    ...   del os.environ['GIT_DIR']
    >>> os.system('git init %s' % path)
    0
    >>> os.chdir(str(path))
    >>> Path('somefile').touch()
    >>> os.system('%s add somefile' % git)
    0
    >>> os.system('%s commit -m "Commit message."' % git)
    0
    >>> node = Git(path)

    # >>> d = node.author_date().split(' ')[0]
    # >>> today = date.today()
    # >>> assert d == '%i-%02i-%i' % (today.year, today.month, today.day)

    # >>> node.description()
    """
    def __init__(self, path = None):
        """Create a GitVersion.

        path -- path to the repository; the source dir by default.
        """
        if path is None:
            self.__path = drake.path_src('.')
        else:
            self.__path = drake.path_src(path)
        VirtualNode.__init__(self, Path('git/version') / self.__path)
        self.__author_date = None
        self.__revision    = None
        self.__description = None
        self.__version    = None
        self.__message     = None

    def __cmd(self, args):
        cmd = ['git'] + args
        return cmd_output(cmd, cwd = str(self.__path)).decode('utf-8').strip()

    def ls_files(self, *paths):
        """Run git ls-files and return the list of Paths.

        path -- the pathes to list
        """
        if not paths:
            paths = [Path('.')]
        out = self.__cmd(['ls-files'] + list(map(str, paths)))
        out = out.split('\n')
        return list(map(drake.Path, out))

    def hash(self):
        return self.__cmd(['rev-parse', 'HEAD'])[:-1]

    def author_date(self):
        """The author date, as given by git %ai format."""
        if self.__author_date is None:
            self.__author_date = self.__cmd(
                ['log', '--pretty=format:%ai', '-n', '1'])
        return self.__author_date

    def description(self):
        """The git describe output."""
        if self.__description is None:
            self.__description = self.__cmd(['describe'])
        return self.__description

    def version(self):
        """The git describe --long output."""
        if self.__version is None:
            self.__version = self.__cmd(['describe', '--long'])
        return self.__version

    def revision(self):
        """The revision short sha1."""
        if self.__revision is None:
            self.__revision = self.__cmd(
                [ 'log', '--pretty=format:%h', '-n', '1'])
        return self.__revision

    def message(self):
        """The commit message."""
        if self.__message is None:
            self.__message = self.__cmd(
                ['log', '--pretty=format:%s', '-n', '1'])
        return self.__message

    def __iter__(self):

        yield ('git-author-date',  self.author_date())
        yield ('git-description',  self.description())
        yield ('git-version',      self.version())
        yield ('git-revision',     self.revision())
        yield ('git-message',      self.message())
