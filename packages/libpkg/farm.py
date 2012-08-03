# -*- encoding: utf-8 -*-

"""Abstract the access to built tarballs.

Accessing build farm through https requires some
work. Authentication, and urls are defined in
`libpkg.constants'.

XXX The whole parsing HTML thing sucks

"""

import re
import sys
import urllib.request

from libpkg import constants

def _urlget(uri):
    """Returns a file like object representing the uri"""

    authinfo = urllib.request.HTTPPasswordMgrWithDefaultRealm()
    authinfo.add_password(
        realm=None,
        uri=uri,
        user=constants.FARM_USER,
        passwd=constants.FARM_PASSWORD
    )
    hdlr = urllib.request.HTTPBasicAuthHandler(authinfo)
    opener = urllib.request.build_opener(hdlr)
    urllib.request.install_opener(opener)
    return urllib.request.urlopen(uri)


def getTarballs(filter=None):
    """Get built tarballs list.

    If a filter is provided, it will returns only matching tarballs. The
    returned list is sorted from the most recent to the older tarball. The
    order is constrained by the url (constants.FARM_BUILDS_URI).

    >>> getTarballs()
    ['infinit-server.tbz', 'infinit-client.tbz']
    >>> getTarballs(filter="client")
    ['infinit-client.tbz']
    >>> getTarballs(filter=["client","server"])
    ['infinit-server.tbz', 'infinit-client.tbz']

    """

    print("Fetching tarballs list...", end='')
    sys.stdout.flush()
    html = str(_urlget(constants.FARM_BUILDS_URI).read())
    print(" Done")
    pattern = re.compile(r'href="(infinit-[^"]+\.tbz)"\S*\s+([\S]+ [\S]+)')
    results = pattern.findall(html)
    if filter is not None:
        if isinstance(filter, str):
            filter = [filter]
        for f in filter:
            pattern = re.compile('.*' + f + '.*')
            results = [
                tarball for tarball in results if pattern.match(tarball[0])
            ]


    return results

def getLastTarballs(filter=None):
    """Like getTarballs() but return only last ones (last git sha1 by date)"""

    tarballs = getTarballs(filter)
    if not tarballs:
        return []
    hash = getTarballHash(tarballs[0][0])
    return [
        tarball for tarball in tarballs if getTarballHash(tarball[0]) == hash
    ]

def downloadTarball(tarball):
    """Retreive a tarball with its name and returns a file-like object."""

    return _urlget(constants.FARM_URI + '/releases/' + tarball)

def getTarballHash(tarball):
    """Returns the git sha1 contained in the tarball name"""

    return (tarball.split('-')[-1].split('.')[0])[1:]

def isClientTarball(tarball):
    return 'infinit-client' in tarball

def isServerTarball(tarball):
    return 'infinit-server' in tarball

def getTarballArchitecture(tarball):
    return {
        'x86_64': constants.Architectures.AMD64,
    }[tarball.split('-')[2]]

def getTarballPlatform(tarball):
    return {
        'linux': constants.Platforms.LINUX,
        'linux_bleeding': constants.Platforms.LINUX,
    }[tarball.split('-')[3]]
