import re
import os.path
import sys

_macro_matcher = re.compile(r'(.*\()(\S+)(,.*\))')

def replacer(match):
    field = match.group(2)
    return match.group(1) + "'" + field + "'" + match.group(3)

def ERR_CODE(name, value, comment):
    globals()[name.upper()] = [value, comment]

filepath = os.path.abspath(os.path.join(os.path.dirname(__file__), 'error_code.hh.inc'))

configfile = open(filepath, 'r')
for line in configfile:
    eval(_macro_matcher.sub(replacer, line))
