from .. import Path, Exception, srctree
from .  import Config, StaticLib

class Sdl:

    def __init__(self, prefix = None):

        if prefix is None:
            test = ['/usr', '/usr/local']
        else:
            test = [prefix]

        for path in test:
            p = Path(path)
            if not p.absolute:
                p = srctree() / p
            if (p / 'include/SDL/SDL.h').exists():

                self.prefix = path

                self.cfg = Config()
                self.cfg.add_system_include_path('%s/include' % self.prefix)

                self.cfg_core = Config()
                self.cfg_core.lib_path('%s/lib' % self.prefix)
                self.cfg_core.lib('SDL')
                self.cfg_core.lib('SDLmain')

                self.cfg_ttf = Config()
                self.cfg_ttf.lib_path('%s/lib' % self.prefix)
                self.cfg_ttf.lib('SDL_ttf')

                self.cfg_image = Config()
                self.cfg_image.lib_path('%s/lib' % self.prefix)
                self.cfg_image.lib('SDL_image')

                return

        raise Exception('unable to find SDL/SDL.h in %s' % ', '.join(test))

    def config(self):

        return self.cfg


    def core(self):

        return self.cfg_core


    def ttf(self):

        return self.cfg_ttf


    def image(self):

        return self.cfg_image
