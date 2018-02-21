# Copyright (C) 2011-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import re

def re_map(f, regexp, string):
  """Find regexp in string and map f on the matches.

  Replace every occurence of regexp in string by the application of f
  to the match.

  >>> re_map(lambda x: '<%s>' % x.capitalize(), 'a|e|i|o|u', 'This was a triumph')
  'Th<I>s w<A>s <A> tr<I><U>mph'
  """
  match = re.search(regexp, string)
  while match:
    prefix = string[:match.span()[0]]
    suffix = string[match.span()[1]:]
    string = '%s%s%s' % (prefix, f(match.group()), suffix)
    match = re.search(regexp, string)
  return string

def camel_case(s):
  """Convert the given indentifier to camel case.

  Converts dashes or underscore separated identifiers to camel case.

  >>> camel_case('foo')
  'foo'
  >>> camel_case('foo-bar')
  'fooBar'
  >>> camel_case('foo_bar_baz_quux')
  'fooBarBazQuux'
  """
  return re_map(lambda s: s[1].capitalize(),
                re.compile('[-_][a-zA-Z]'), s)


def shell_escape(s):
    """Escape a string to be a single shell entity.

    s -- the string or string-convertible to espace.

    >>> shell_escape('foo')
    'foo'
    >>> shell_escape('foo bar')
    "'foo bar'"
    >>> shell_escape('foo$bar')
    "'foo$bar'"
    >>> shell_escape('foo"bar')
    '\\'foo"bar\\''
    >>> shell_escape('$foobar')
    "'$foobar'"
    >>> shell_escape("foo'bar")
    "'foo'\\\\''bar'"
    """
    s = str(s)
    for special in [' ', '$', '"', "'"]:
        if s.find(special) != -1:
            s = s.replace("'", "'\\''")
            s = "'%s'" % s
            break
    return s

def property_memoize(f):
  def result(self):
    prop = '_%s__%s' % (self.__class__.__name__, f.__name__)
    if not hasattr(self, prop):
      setattr(self, prop, f(self))
    return getattr(self, prop)
  return property(result)

def pretty_listing(c, any = False, quantifier = None):
  '''
  Format a collection of items into a human readable listing.

  >>> pretty_listing(['foo'])
  'foo'
  >>> pretty_listing(['foo', 'bar'])
  'foo and bar'
  >>> pretty_listing(['foo', 'bar', 'baz'])
  'foo, bar and baz'
  >>> pretty_listing(['foo', 'bar', 'baz'], any = True)
  'foo, bar or baz'
  >>> pretty_listing(['foo', 'bar', 'baz'], quantifier = True)
  'all of foo, bar and baz'
  >>> pretty_listing(['foo', 'bar', 'baz'],
  ...                any = True, quantifier = True)
  'any of foo, bar or baz'
  '''
  import types
  if isinstance(c, types.GeneratorType):
    c = list(c)
  elif not isinstance(c, (list, tuple)):
    return str(c)
  if len(c) == 1:
    return str(c[0])
  return '{}{} {} {}'.format(
    '{} of '.format('any' if any else 'all') if quantifier else '',
    ', '.join(map(str, c[:-1])), 'or' if any else 'and', c[-1])
