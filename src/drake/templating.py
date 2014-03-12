# Copyright (C) 2013, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake

class Context:

  current = None

  def __init__(self,
               content = {},
               sources = [],
               pythonpath = (),
               hooks = {},
             ):
    self.content = content
    self.sources = sources
    self.pythonpath = pythonpath
    self.hooks = hooks

  def __enter__(self):
    self.__previous = Context.current
    Context.current = self

  def __exit__(self, *args, **kwargs):
    Context.current = self.__previous

class Template(drake.Node):

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    context = Context.current
    if context is not None:
      Renderer(self,
               content = context.content,
               sources = context.sources,
               pythonpath = context.pythonpath,
               hooks = context.hooks
             )

drake.Node.extensions['tmpl'] = Template


class Renderer(drake.Converter):

  def __init__(self,
               template,
               content = {},
               sources = [],
               pythonpath = (),
               hooks = {}):
    self.__template = template
    self.__hooks = hooks
    dst = template.name().without_last_extension()
    self.__target = drake.node(dst)
    super().__init__(self.__template,
                     self.__target,
                     additional_sources = sources)
    self.__content = content
    self.__pythonpath = []
    for path in pythonpath:
      self.__pythonpath.append(drake.path_source(path))
      self.__pythonpath.append(drake.path_build(path))

  def execute(self):
    self.output('Render %s' % self.__target)
    import mako.template
    import mako.runtime
    import sys
    tpl = str(self.__template.path(absolute = True))
    previous = sys.path
    sys.path = sys.path[:]
    modules = set(sys.modules)
    for source in self.__hooks.keys():
      for hook in self.__hooks[source]:
        hook(self.__content, source)
    try:
      sys.path = [str(path) for path in  self.__pythonpath] + sys.path
      tpl = mako.template.Template(filename = tpl)
      with drake.WritePermissions(self.__target):
        with open(str(self.__target.path()), 'w') as f:
          ctx = mako.runtime.Context(f, **self.__content)
          tpl.render_context(ctx)
      return True
    finally:
      # Restore path.
      sys.path = previous
      # Unload modules to avoid side effects.
      for added in set(sys.modules) - modules:
        del sys.modules[added]

  @property
  def rendered(self):
    return self.__target

  def hash(self):
    return {
      'content': self.__content,
      'pythonpath': self.__pythonpath,
    }

  def __str__(self):
    return 'rendering of %s' % self.__target
