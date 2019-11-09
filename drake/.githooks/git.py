'''Git commands helpers.'''

import subprocess

def pipe(cmd):
  '''Run a git command and return its pipable output.'''
  return subprocess.Popen(['git'] + cmd, stdout=subprocess.PIPE).stdout

def git(cmd):
  '''Run a git command.'''
  return subprocess.check_output(['git'] + cmd).decode().strip()

def head():
  '''Current git HEAD.'''
  try:
    return git(['rev-parse', '--verify', 'HEAD'])
  except subprocess.CalledProcessError:
    # Initial commit: return an empty tree object
    return '4b825dc642cb6eb9a060e54bf8d69288fbee4904'
