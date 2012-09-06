# -*- encoding: utf-8 -*-

import os
import shutil
import sys
import subprocess

from libpkg import constants
from libpkg import tools
from libpkg.buildenv import BuildEnv
from libpkg.packager import Packager as BasePackager

def cmd(*args, **kwargs):
    return subprocess.check_output(list(args), **kwargs)

class Packager(BasePackager):
    """MacOSX dmg packager.
    """

    def __init__(self, built_architectures, built_platforms):
        BasePackager.__init__(
            self, "MacOSX", 'dmg',
            built_architectures, built_platforms
        )

    @property
    def is_available(self):
        return tools.which('hdiutil') is not None

    @property
    def compatible_platforms(self):
        return [constants.Platforms.MACOSX]

    def buildClientPackage(self, build_env, dest_dir):
        tempdir = BuildEnv.makeTemporaryDirectory()
        try:
            pkgdir = os.path.join(tempdir, 'pkg')
            os.mkdir(pkgdir)
            shutil.copytree(
                os.path.join(build_env.directory, 'Infinit.app'),
                os.path.join(pkgdir, 'Infinit.app'),
                symlinks=True,
            )
            for dir_ in ['lib', 'bin']:
                shutil.copytree(
                    os.path.join(build_env.directory, dir_),
                    os.path.join(pkgdir, 'Infinit.app/Contents/Resources', dir_),
                )
            rel_fuse_lib = os.path.join(pkgdir, "Infinit.app/Contents/Resources/lib", "libfuse4x.2.dylib")
            rel_gap_lib = os.path.join(pkgdir, "Infinit.app/Contents/Resources/lib", "libgap.dylib")
            shutil.copyfile("/usr/local/lib/libfuse4x.2.dylib", rel_fuse_lib)
            cmd(
                "install_name_tool",
                "-change", "/usr/local/lib/libfuse4x.2.dylib",
                os.path.join("@loader_path", "libfuse4x.2.dylib"),
                rel_gap_lib
            )  

            params = {
                'architecture': {
                    constants.Architectures.AMD64: 'x86_64',
                    constants.Architectures.I386: 'i386',
                }[build_env.architecture],
                'version_name': build_env.build.infos['version_name'],
                'version': build_env.build.infos['version'],
            }
            filename = "infinit-%(version_name)s-%(version)s-%(architecture)s.dmg" % params
            path = os.path.join(tempdir, filename)

            DIR_SOURCE = os.path.join(os.path.dirname(sys.argv[0]), "libpkg/packagers")

            res = os.system('''%(dir_source)s/macosx/dmger/create-dmg \
                --window-size %(window_size_x)i %(window_size_y)i \
                --background "%(background)s" \
                --icon-size %(icon_size)i \
                --volname "%(volname)s" \
                --app-drop-link %(app_rect_x)i %(app_rect_y)i \
                --icon Infinit.app %(infinit_rect_x)i %(infinit_rect_y)i \
                "%(dmgfile)s" "%(pkgdir)s"
                ''' % {
                'dir_source' : DIR_SOURCE,
                'window_size_x' : 600,
                'window_size_y' : 400,
                'background' : DIR_SOURCE + '/macosx/dmger/infinit-dmg-bg.png',
                'icon_size' : 100,
                'volname' : 'Infinit',
                'app_rect_x' : 450,
                'app_rect_y' : 300,
                'infinit_rect_x' : 220,
                'infinit_rect_y' : 140,
                'dmgfile': path,
                'pkgdir': pkgdir,
            })
            
            if res != 0:
                raise Exception("Couldn't create the dmg file")
            shutil.copyfile(
                os.path.join(tempdir, filename),
                os.path.join(dest_dir, filename)
            )
            return filename
        finally:
            BuildEnv.removeDirectory(tempdir)
