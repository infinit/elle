"""A caching template loader that allows disk-based templates to be used."""


import os, sys
from abc import ABCMeta, abstractmethod


class TemplateDoesNotExist(Exception):
    
    """A named template could not be found."""


class Source(metaclass=ABCMeta):
    
    """A source of template data."""
    
    __slots__ = ()
    
    @abstractmethod
    def load_source(self, template_name):
        """
        Loads the template source code for the template of the given name.
        
        If no source code can be found, returns None.
        """
        
        
class MemorySource(Source):

    """A template loader that loads from memory."""
    
    __slots__ = ("templates",)
    
    def __init__(self, templates):
        """Initializes the MemorySource from a dict of template source strings."""
        self.templates = templates
        
    def load_source(self, template_name):
        """Loads the source from the memory template dict."""
        return self.templates.get(template_name)
        
    def __str__(self):
        """Returns a string representation."""
        return "<memory>"
        
        
class DirectorySource(Source):
    
    """A template loader that loads from a directory on disk."""
    
    __slots__ = ("dirname")
    
    def __init__(self, dirname):
        """
        Initializes the DirectorySource.
        
        On windows, the dirname should be specified using forward-slashes.
        """
        self.dirname = dirname
        
    def load_source(self, template_name):
        """Loads the source from disk."""
        template_path = os.path.normpath(os.path.join(self.dirname, template_name))
        if os.path.exists(template_path):
            with open(template_path, "r") as template_file:
                return template_file.read()
        return None
        
    def __str__(self):
        """Returns a string representation."""
        return self.dirname


class DebugLoader:

    """
    A template loader that doesn't cache compiled templates.
    
    Terrible performance, but great for debugging.
    """
    
    __slots__ = ("_sources", "_parser",)
    
    def __init__(self, sources, parser):
        """
        Initializes the Loader.
        
        When specifying template_dirs on Windows,the forward slash '/' should be used as a path separator.
        """
        self._sources = list(reversed(sources))
        self._parser = parser
    
    def compile(self, template, name="__string__", params=None, meta=None):
        """Compiles the given template source."""
        default_meta = {
            "__loader__": self
        }
        default_meta.update(meta or {})
        return self._parser.compile(template, name, params, default_meta)
    
    def _load_all(self, template_name):
        """Loads and returns all the named templates from the sources."""
        # Load from all the template sources.
        templates = []
        for source in self._sources:
            template_src = source.load_source(template_name)
            if template_src is not None:
                meta = {
                    "__super__": templates and templates[-1] or None,
                }
                templates.append(self.compile(template_src, template_name, {}, meta))
        return templates
    
    def load(self, *template_names):        
        """
        Loads and returns the named template.
        
        If more than one template name is given, then the first template that exists will be used.
        
        On Windows, the forward slash '/' should be used as a path separator.
        """
        if not template_names:
            raise ValueError("You must specify at least one template name.")
        for template_name in template_names:
            templates = self._load_all(template_name)
            if templates:
                return templates[-1]
        # Raise an error.
        template_name_str = ", ".join(repr(template_name) for template_name in template_names)
        source_name_str = ", ".join(str(source) for source in self._sources)
        raise TemplateDoesNotExist("Could not find a template named {} in any of {}.".format(template_name_str, source_name_str))
        
    def render(self, *template_names, **params):
        """
        Loads and renders the named template.
        
        If more than one template name is given, then the first template that exists will be used.
        
        On Windows, the forward slash '/' should be used as a path separator.
        """
        return self.load(*template_names).render(**params)


class Loader(DebugLoader):
    
    """
    A template loader.
    
    Compiled templates are cached for performance.
    """
    
    __slots__ = ("_cache",)
    
    def __init__(self, sources, parser):
        """Initializes the loader."""
        super(Loader, self).__init__(sources, parser)
        self._cache = {}
    
    def clear_cache(self, ):
        """Clears the template cache."""
        self._cache.clear()
        
    def _load_all(self, template_name):
        """A caching version of the debug loader's load method."""
        if template_name in self._cache:
            return self._cache[template_name]
        template = super(Loader, self)._load_all(template_name)
        self._cache[template_name] = template
        return template