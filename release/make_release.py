# -*- encoding: utf-8 -*-

import sys, os, hashlib

XML = """
<release major="%(major)s" minor="%(minor)s">
%(executables)s
</release>
"""

if __name__ == '__main__':
    dest_dir, major, minor = sys.argv[1:]
    assert os.path.isdir(dest_dir)
    bin_dir = os.path.join(dest_dir, "binaries")
    assert os.path.isdir(bin_dir)
    binaries = os.listdir(bin_dir)
    EXE_TAG = """\t<executable path="%s" md5sum="%s" />"""
    EXE_TAGS = []
    isexecutable = lambda f: os.access(f, os.X_OK) and not os.path.isdir(f)
    for binary in binaries:
        fpath = os.path.join(bin_dir, binary)
        if isexecutable(fpath):
            with open(fpath) as f:
                md5 = hashlib.md5(f.read()).hexdigest()
            EXE_TAGS.append(
                EXE_TAG % ("binaries/" + binary, md5)
            )
    with open(os.path.join(dest_dir, "release.xml"), "w") as f:
        f.write(XML % {
            'major': major, 'minor': minor,
            'executables': '\n'.join(EXE_TAGS)
        })
