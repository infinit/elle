#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import os, sys
import argparse
def getParser():
    parser = argparse.ArgumentParser(description='replace code in c++')
    parser.add_argument(
        'names',
        metavar='N',
        nargs='+',
        help='name(s) to move or rename'
    )
    parser.add_argument(
        '--from-ns',
        default='elle',
        action='store',
        help='origin namespace'
    )
    parser.add_argument(
        '--to-ns',
        action='store',
        help='destination namespace'
    )
    parser.add_argument(
        '-p', '--print-commands',
        action='store_true',
        default=False,
        help='Print shell commands and exit'
    )
    parser.add_argument(
        '-r', '--replace',
        action='store_true',
        default=False,
        help="Replace everything inplace"
    )
    return parser

if __name__ == '__main__':
    os.chdir(os.path.dirname(__file__))
    parser = getParser()
    args = parser.parse_args()

    sed_cmds = [
        r"%(sed)s -e 's/const %(from_ns)s::\(%(joined_names)s\)\([^a-zA-Z0-9_]\)/%(to_ns)s::\1\2 const/g' %(filename)s",
        r"%(sed)s -e 's/const %(to_ns)s::\(%(joined_names)s\)/%(to_ns)s::\1 const/g' %(filename)s",
        r"%(sed)s -e 's/%(from_ns)s::\(%(joined_names)s\)\([^a-zA-Z0-9_]\)/%(to_ns)s::\1\2/g' %(filename)s",
        r"%(sed)s -e 's/%(from_ns)s::\(%(joined_names)s\)$/%(to_ns)s::\1/g' %(filename)s",
    ]
    params = {
        'sed': args.replace and 'sed -i' or 'sed',
        'joined_names': r"\|".join(args.names),
        'from_ns': args.from_ns,
        'to_ns': args.to_ns,
        'filename': '{}', # for find
    }
    for sed_cmd in sed_cmds:
        find_cmd = r"find . \( -name '*.hh' -or -name '*.hxx' -or -name '*.cc' \) -exec %s \;" % sed_cmd
        if args.print_commands:
            print(find_cmd % params)

        if args.replace or not args.print_commands:
            os.system(find_cmd % params)


