#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import argparse
import os
import re
import urllib.request
import sys

parser = argparse.ArgumentParser(description="Deploy infinit")
parser.add_argument(
    "--match", '-m',
    help="filter with a simple pattern",
    action="store"
)
parser.add_argument(
    '--last', '-l',
    help="Install the last tarball in the results",
    action='store_true',
    default=False
)
parser.add_argument(
    '--print', '-p',
    help="Only print matching releases",
    action='store_true',
    default=False
)
parser.add_argument(
    '--yes', '-y',
    help="Assume yes to all question",
    action='store_true',
    default=False,
)
parser.add_argument(
    '--download-only', '-d',
    help="Only download matching archives",
    action='store_true',
    default=False
)


def getHash(tb):
    return tb.split('-')[-1].split('.')[0]

def dumpReleases(releases):
    for hash, tarballs in releases.items():
        print('-', hash, ':', tarballs)

def yesno(s, default=False):
    res = None
    if default:
        s += ' (Y/n): '
    else:
        s += ' (y/N): '
    while res is None:
        v = input(s).strip()
        if not v: return default
        elif v in ['y', 'Y', 'o', 'O']:
            return True
        elif v in ['n', 'N']:
            return False

def urlget(uri):
    authinfo = urllib.request.HTTPPasswordMgrWithDefaultRealm() #
    authinfo.add_password(realm=None,
                          uri=uri,
                          user='farm',
                          passwd='iegh_aJ2')
    hdlr = urllib.request.HTTPBasicAuthHandler(authinfo)
    opener = urllib.request.build_opener(hdlr)
    urllib.request.install_opener(opener)
    return urllib.request.urlopen(uri)

def deployClientTarball(tarball, platform, arch):
    os.system('scp %s oracle@infinit.im:www/infinit.im/downloads' % tarball)
    cmd = ' && '.join([
        "cd www/infinit.im/downloads",
        "mkdir -p %(dir)s",
        "tar --extract --file=%(tarball)s --strip-components=1 --directory=%(dir)s",
        "rm %(tarball)s",
        "chmod -R a+rX %(dir)s",
    ]) % {
        'dir': platform + arch,
        'tarball': tarball,
    }
    os.system('ssh oracle@infinit.im "%s"' % cmd)

def deployServerTarball(tarball):
    os.system('scp  %s oracle@infinit.im:www/infinit.im/' % tarball)
    cmd = ' && '.join([
        "cd www/infinit.im",
        "tar --extract --file=%(tarball)s --strip-components=1 --directory=.",
        "rm %(tarball)s",
    ]) % {
        'tarball': tarball,
    }
    os.system('ssh oracle@infinit.im "%s"' % cmd)


def deployTarball(tarball):
    print("Deploying", tarball)
    if 'x86_64' in tarball:
        arch = '64'
    else:
        arch = '32'

    if 'linux' in tarball:
        platform = 'linux'
    else:
        raise Exception("CHECK FOR OTHER PLATFORM")

    if tarball.startswith("infinit-client"):
        deployClientTarball(tarball, platform, arch)
    elif tarball.startswith("infinit-server"):
        assert 'x86_64' in tarball
        assert 'linux' in tarball
        deployServerTarball(tarball)
    else:
        raise Exception("Unknown tarball type!")
    os.system('rm -rf "%s"' % tarball)



if __name__ == '__main__':
    args = parser.parse_args()

    releases_uri = 'https://farm.infinit.io/releases/?C=M;O=D'
    html = str(urlget(releases_uri).read())

    pattern = re.compile(r'href="(infinit-[^"]+\.tbz)"')

    results = pattern.findall(html)

    tarballs = [str(tarball) for tarball in results]
    if args.match:
        pattern = re.compile('.*' + args.match + '.*')
        tarballs = [str(tarball) for tarball in results if pattern.match(tarball)]

    to_install = None
    if args.last and tarballs:
        to_install = getHash(tarballs[0])

    releases = {}
    for t in tarballs:
        h = getHash(t)
        releases.setdefault(h, [])
        releases[h].append(t)

    if args.print:
        dumpReleases(releases)
        sys.exit(0)

    if not to_install:
        if not len(releases):
            print("No release found")
        elif len(releases) == 1:
            to_install = list(releases.keys())[0]
        else:
            print("More than one release match:")
            dumpReleases(releases)
            print("Use --match with a pattern or --last")

    if not to_install:
        sys.exit(1)


    print("Tarballs to be deployed:")
    tarballs = releases[to_install]
    for t in tarballs:
        print("\t-", t)

    if not args.yes and not yesno("Are you ok with that ?", True):
        sys.exit(0)

    print("Here we go!")
    base_uri = 'https://farm.infinit.io/releases/'
    for t in tarballs:
        print("Downloading", t)
        dl = urlget(base_uri + t)
        with open(t, 'wb') as f:
            while True:
                data = dl.read(4096)
                if not data:
                    break
                f.write(data)
        if not args.download_only:
            deployTarball(t)


