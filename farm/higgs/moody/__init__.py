"""
The moody templating system.

Fast and powerful. It gets the job done.

Developed by Dave Hall <dave@etianen.com>.
"""

from moody.errors import TemplateError, TemplateCompileError, TemplateRenderError
from moody.macros import DEFAULT_MACROS
from moody.parser import Parser
from moody.loader import Loader, DebugLoader, Source, DirectorySource, TemplateDoesNotExist


__all__ = ("default_parser", "compile", "render", "make_loader",)


default_parser = Parser(DEFAULT_MACROS)

compile = default_parser.compile


def render(template, **params):
    """
    Compiles and renders the given template string with the given parameters.
    
    This is just a shortcut for moody.compile(template).render(**params).
    """
    return compile(template).render(**params)


def make_loader(*sources, loader_cls=Loader, parser=default_parser, **kwargs):
    """Factory method for creating loaders."""
    # Create the sources.
    source_objs = []
    for source in sources:
        if isinstance(source, Source):
            source_objs.append(source)
        elif isinstance(source, str):
            source_objs.append(DirectorySource(source))
        else:
            raise TypeError("A source should be a str or a Source instance, not {!r}.".format(source))
    # Instantiate the loader.
    return loader_cls(source_objs, parser, **kwargs)