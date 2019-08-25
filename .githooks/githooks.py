'''Git hooks common helpers.'''

import sys

import git
import hooks

from log import log

def print_exc(error, file):
  '''Recursively print exceptions messages.'''
  print(': {}'.format(error), file=file, end='')
  if error.__cause__ is not None:
    print_exc(error.__cause__, file)
  else:
    print('', file=file)
    if hasattr(error, 'output'):
      for line in error.output.split('\n'):
        print('  {}'.format(line), file=file)

def main(entrypoint):
  '''Run hook, catching and printing errors.'''
  try:
    entrypoint()
  except Exception as error:
    print('fatal error', file=sys.stderr, end='')
    print_exc(error, sys.stderr)
    exit(1)

def status_to_string(status):
  '''Convert git single-letter status to a human readable one.'''
  if status == 'A':
    return 'added'
  elif status == 'C':
    return 'copied'
  elif status == 'M':
    return 'modified'
  elif status.startswith('R'):
    return 'renamed'
  else:
    raise Exception('unrecognized git status {!r}'.format(status))

def run_regression_hooks(before, before_sha, after, after_sha):
  '''Run regressions hook on the given before/after file versions.'''
  for line in git.git(['diff', '--name-status', '--diff-filter=ACMR', '{}..{}'.format(
      before_sha, after_sha)]).split('\n'):
    if line == '':
      continue
    status, file = line.split('\t', maxsplit=1)
    if status == 'A':
      before_f = None
      after_f = '{}/{}'.format(before, file)
    elif status in ['C', 'M']:
      before_f = '{}/{}'.format(before, file)
      after_f = '{}/{}'.format(after, file)
    elif status.startswith('R'):
      before_f, after_f = file.split('\t')
      file = after_f
      before_f = '{}/{}'.format(before, file)
      after_f = '{}/{}'.format(after, file)
    log.info('validate {status} file {path!r}', status=status_to_string(status), path=file)
    for hook in hooks.REGRESSION_HOOKS:
      try:
        hook(before_f, after_f, file)
      except Exception as error:
        raise Exception('error on {}'.format(file)) from error
