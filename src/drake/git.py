# Copyright (C) 2009-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

from datetime import date
import drake
import os
import subprocess
from . import VirtualNode, Path

class Git(VirtualNode):

    """An iterable node with information on a git repository.

    # With the following dummy git repository.
    >>> def run(cmd):
    ...   import subprocess
    ...   subprocess.check_output(cmd)
    >>> os.chdir('/tmp')
    >>> path = Path('.drake.git')
    >>> path.remove()
    >>> if 'GIT_DIR' in os.environ:
    ...   del os.environ['GIT_DIR']
    >>> import subprocess
    >>> run(['git', 'init', str(path)])
    >>> os.chdir(str(path))
    >>> run(['git', 'config', 'user.name', 'mefyl'])
    >>> run(['git', 'config', 'user.email', 'mefyl@gruntech.org'])
    >>> Path('somefile').touch()
    >>> run(['git', 'add', 'somefile'])
    >>> run(['git', 'commit', '-m', 'Commit message.'])
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
            path = '.'
        self.__path = drake.path_source(path)
        self.__name = drake.path_build(path)
        VirtualNode.__init__(self, self.__name / 'git')
        self.__author_date = None
        self.__revision    = None
        self.__description = None
        self.__version     = None
        self.__message     = None

    def run(self, args, raw = False):
        return self.__cmd(args, raw)

    def __cmd(self, args, raw = False):
        cmd = ['git', '-C', str(self.__path)] + args
        p = subprocess.run(cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        if p.returncode:
            raise Exception('failed: {}: {}'.format(' '.join(cmd), p.stderr))
        return p.stdout if raw else p.stdout.decode('utf-8').strip()

    def ls_files(self, *paths):
        """Run git ls-files and return the list of Paths.

        path -- the paths to list
        """
        if not paths:
            paths = [Path('.')]
        out = self.__cmd(['ls-files'] + list(map(str, paths)))
        out = out.split('\n')
        return list(map(drake.Path, out))

    def hash(self):
      import hashlib
      hasher = hashlib.sha1()
      hasher.update(self.__cmd(['rev-parse', 'HEAD'], raw = 1))
      hasher.update(self.version().encode('utf-8'))
      return hasher.hexdigest()

    def rev_parse(self, revision = 'HEAD', short = False):
      cmd = ['rev-parse']
      if short:
        cmd += ['--short']
      cmd += [revision]
      return self.__cmd(cmd)

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
