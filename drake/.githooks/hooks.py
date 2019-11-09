'''List of hooks for the drake repository.'''

import os
import pipes
import re
import subprocess

import git

def run(*args, **kwargs):
  '''Run a command and return its output.'''
  return subprocess.check_output(*args, **kwargs).decode().strip()

def is_python(path):
  '''Whether the given file is Python.'''
  if path is not None:
    return run(['file', path]).find('Python') >= 0
  else:
    return False

PYLINT_RE = re.compile('^Your code has been rated at ([0-9.]+)/10')

def pylint(path, err, args=None):
  '''Py-lint the given file.'''
  command = ['pylint', path]
  if args is not None:
    command += args
  if not err:
    command.append('--exit-zero')
    output = run(command, env=dict(os.environ, PYTHONPATH='src'))
    for line in output.split('\n'):
      match = re.search(PYLINT_RE, line)
      if match:
        return float(match.group(1)), output
    raise Exception('failed to parse pylint output ({})'.format(
      ' '.join(map(pipes.quote, command))))
  command.append('--score=n')
  output = run(command,
               env=dict(os.environ, PYTHONPATH='src')).split('\n')
  return 10., output

def no_pylint_regression(before, after, path):
  '''Check there are no pylint score regression between before and after.

  If there was no previous file, check the file lints perfectly.'''
  command = []
  prefix_lint = '{}: lint: '.format(path)
  prefix_disable = '{}: pylint-disable: '.format(path)
  for line in git.git(['check-attr', '-a', path]).split('\n'):
    if line.startswith(prefix_lint):
      if line[len(prefix_lint):] == 'false':
        return
    if line.startswith(prefix_disable):
      command.append('--disable={}'.format(line[len(prefix_disable):]))
  if is_python(after):
    if is_python(before):
      (score_b, _), (score_a, output_a) = (
        pylint(f, False, command) for f in [before, after])
      if score_a < score_b:
        error = Exception('pylint regression: {} -> {}'.format(score_b, score_a))
        error.output = output_a
        raise error
    else:
      try:
        pylint(after, True, command)
      except subprocess.CalledProcessError as process_error:
        error = Exception('pylint errors')
        error.output = process_error.output.decode()
        raise error from None

REGRESSION_HOOKS = [
  no_pylint_regression,
]
